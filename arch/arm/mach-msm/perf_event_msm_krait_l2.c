/*
 * Copyright (c) 2011, 2012 Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/irq.h>
#include <asm/pmu.h>
#include <linux/platform_device.h>

#include <mach/msm-krait-l2-accessors.h>

#define MAX_L2_PERIOD	((1ULL << 32) - 1)
#define MAX_KRAIT_L2_CTRS 5

#define L2PMCCNTR 0x409
#define L2PMCCNTCR 0x408
#define L2PMCCNTSR 0x40A
#define L2CYCLE_CTR_BIT 31
#define L2CYCLE_CTR_EVENT_IDX 4
#define L2CYCLE_CTR_RAW_CODE 0xfe

#define L2PMOVSR	0x406

#define L2PMCR	0x400
#define L2PMCR_RESET_ALL	0x6
#define L2PMCR_GLOBAL_ENABLE	0x1
#define L2PMCR_GLOBAL_DISABLE	0x0

#define L2PMCNTENSET	0x403
#define L2PMCNTENCLR	0x402

#define L2PMINTENSET	0x405
#define L2PMINTENCLR	0x404

#define IA_L2PMXEVCNTCR_BASE	0x420
#define IA_L2PMXEVTYPER_BASE	0x424
#define IA_L2PMRESX_BASE	0x410
#define IA_L2PMXEVFILTER_BASE	0x423
#define IA_L2PMXEVCNTR_BASE	0x421

/* event format is -e rsRCCG See get_event_desc() */

#define EVENT_REG_MASK		0xf000
#define EVENT_GROUPSEL_MASK	0x000f
#define	EVENT_GROUPCODE_MASK	0x0ff0
#define EVENT_REG_SHIFT		12
#define EVENT_GROUPCODE_SHIFT	4

#define	RESRX_VALUE_EN	0x80000000

static u32 l2_orig_filter_prefix = 0x000f0030;

static u32 pmu_type;

static struct arm_pmu krait_l2_pmu;

static struct perf_event *l2_events[MAX_KRAIT_L2_CTRS];
static unsigned long l2_used_mask[BITS_TO_LONGS(MAX_KRAIT_L2_CTRS)];

static struct pmu_hw_events krait_l2_pmu_hw_events = {
	.events = l2_events,
	.used_mask = l2_used_mask,
	.pmu_lock = __RAW_SPIN_LOCK_UNLOCKED(krait_l2_pmu_hw_events.pmu_lock),
};

struct event_desc {
	int event_groupsel;
	int event_reg;
	int event_group_code;
};

static struct pmu_hw_events *krait_l2_get_hw_events(void)
{
	return &krait_l2_pmu_hw_events;
}

void get_event_desc(u64 config, struct event_desc *evdesc)
{
	/* L2PMEVCNTRX */
	evdesc->event_reg = (config & EVENT_REG_MASK) >> EVENT_REG_SHIFT;
	/* Group code (row ) */
	evdesc->event_group_code =
	    (config & EVENT_GROUPCODE_MASK) >> EVENT_GROUPCODE_SHIFT;
	/* Group sel (col) */
	evdesc->event_groupsel = (config & EVENT_GROUPSEL_MASK);

	pr_debug("%s: reg: %x, group_code: %x, groupsel: %x\n", __func__,
		 evdesc->event_reg, evdesc->event_group_code,
		 evdesc->event_groupsel);
}

static void set_evcntcr(int ctr)
{
	u32 evtcr_reg = (ctr * 16) + IA_L2PMXEVCNTCR_BASE;

	set_l2_indirect_reg(evtcr_reg, 0x0);
}

static void set_evtyper(int event_groupsel, int event_reg, int ctr)
{
	u32 evtype_reg = (ctr * 16) + IA_L2PMXEVTYPER_BASE;
	u32 evtype_val = event_groupsel + (4 * event_reg);

	set_l2_indirect_reg(evtype_reg, evtype_val);
}

static void set_evres(int event_groupsel, int event_reg, int event_group_code)
{
	u32 group_reg = event_reg + IA_L2PMRESX_BASE;
	u32 group_val =
		RESRX_VALUE_EN | (event_group_code << (8 * event_groupsel));
	u32 resr_val;
	u32 group_byte = 0xff;
	u32 group_mask = ~(group_byte << (8 * event_groupsel));

	resr_val = get_l2_indirect_reg(group_reg);
	resr_val &= group_mask;
	resr_val |= group_val;

	set_l2_indirect_reg(group_reg, resr_val);
}

static void set_evfilter_task_mode(int ctr)
{
	u32 filter_reg = (ctr * 16) + IA_L2PMXEVFILTER_BASE;
	u32 filter_val = l2_orig_filter_prefix | 1 << smp_processor_id();

	set_l2_indirect_reg(filter_reg, filter_val);
}

static void set_evfilter_sys_mode(int ctr)
{
	u32 filter_reg = (ctr * 16) + IA_L2PMXEVFILTER_BASE;
	u32 filter_val = l2_orig_filter_prefix | 0xf;

	set_l2_indirect_reg(filter_reg, filter_val);
}

static void enable_intenset(u32 idx)
{
	if (idx == L2CYCLE_CTR_EVENT_IDX)
		set_l2_indirect_reg(L2PMINTENSET, 1 << L2CYCLE_CTR_BIT);
	else
		set_l2_indirect_reg(L2PMINTENSET, 1 << idx);
}

static void disable_intenclr(u32 idx)
{
	if (idx == L2CYCLE_CTR_EVENT_IDX)
		set_l2_indirect_reg(L2PMINTENCLR, 1 << L2CYCLE_CTR_BIT);
	else
		set_l2_indirect_reg(L2PMINTENCLR, 1 << idx);
}

static void enable_counter(u32 idx)
{
	if (idx == L2CYCLE_CTR_EVENT_IDX)
		set_l2_indirect_reg(L2PMCNTENSET, 1 << L2CYCLE_CTR_BIT);
	else
		set_l2_indirect_reg(L2PMCNTENSET, 1 << idx);
}

static void disable_counter(u32 idx)
{
	if (idx == L2CYCLE_CTR_EVENT_IDX)
		set_l2_indirect_reg(L2PMCNTENCLR, 1 << L2CYCLE_CTR_BIT);
	else
		set_l2_indirect_reg(L2PMCNTENCLR, 1 << idx);
}

static u32 krait_l2_read_counter(int idx)
{
	u32 val;
	u32 counter_reg = (idx * 16) + IA_L2PMXEVCNTR_BASE;

	if (idx == L2CYCLE_CTR_EVENT_IDX)
		val = get_l2_indirect_reg(L2PMCCNTR);
	else
		val = get_l2_indirect_reg(counter_reg);

	return val;
}

static void krait_l2_write_counter(int idx, u32 val)
{
	u32 counter_reg = (idx * 16) + IA_L2PMXEVCNTR_BASE;

	if (idx == L2CYCLE_CTR_EVENT_IDX)
		set_l2_indirect_reg(L2PMCCNTR, val);
	else
		set_l2_indirect_reg(counter_reg, val);
}

static void krait_l2_stop_counter(struct hw_perf_event *hwc, int idx)
{
	disable_intenclr(idx);
	disable_counter(idx);

	pr_debug("%s: event: %ld ctr: %d stopped\n", __func__,
			hwc->config_base, idx);
}

static void krait_l2_enable(struct hw_perf_event *hwc, int idx, int cpu)
{
	struct event_desc evdesc;
	unsigned long iflags;

	raw_spin_lock_irqsave(&krait_l2_pmu_hw_events.pmu_lock, iflags);

	if (hwc->config_base == L2CYCLE_CTR_RAW_CODE)
		goto out;

	set_evcntcr(idx);

	memset(&evdesc, 0, sizeof(evdesc));

	get_event_desc(hwc->config_base, &evdesc);

	set_evtyper(evdesc.event_groupsel, evdesc.event_reg, idx);

	set_evres(evdesc.event_groupsel, evdesc.event_reg,
		  evdesc.event_group_code);

	if (cpu < 0)
		set_evfilter_task_mode(idx);
	else
		set_evfilter_sys_mode(idx);

out:
	enable_intenset(idx);
	enable_counter(idx);

	raw_spin_unlock_irqrestore(&krait_l2_pmu_hw_events.pmu_lock, iflags);

	pr_debug("%s: ctr: %d group: %ld group_code: %lld started from cpu:%d\n",
	     __func__, idx, hwc->config_base, hwc->config, smp_processor_id());
}

static void krait_l2_disable(struct hw_perf_event *hwc, int idx)
{
	unsigned long iflags;

	raw_spin_lock_irqsave(&krait_l2_pmu_hw_events.pmu_lock, iflags);

	krait_l2_stop_counter(hwc, idx);

	raw_spin_unlock_irqrestore(&krait_l2_pmu_hw_events.pmu_lock, iflags);

	pr_debug("%s: event: %ld deleted\n", __func__, hwc->config_base);

}

static int krait_l2_get_event_idx(struct pmu_hw_events *cpuc,
				  struct hw_perf_event *hwc)
{
	int ctr = 0;

	if (hwc->config_base == L2CYCLE_CTR_RAW_CODE) {
		if (!test_and_set_bit(L2CYCLE_CTR_EVENT_IDX, cpuc->used_mask))
			return L2CYCLE_CTR_EVENT_IDX;
	}

	for (ctr = 0; ctr < MAX_KRAIT_L2_CTRS - 1; ctr++) {
		if (!test_and_set_bit(ctr, cpuc->used_mask))
			return ctr;
	}

	return -EAGAIN;
}

static void krait_l2_start(void)
{
	isb();
	set_l2_indirect_reg(L2PMCR, L2PMCR_GLOBAL_ENABLE);
}

static void krait_l2_stop(void)
{
	set_l2_indirect_reg(L2PMCR, L2PMCR_GLOBAL_DISABLE);
	isb();
}

u32 get_reset_pmovsr(void)
{
	int val;

	val = get_l2_indirect_reg(L2PMOVSR);
	/* reset it */
	val &= 0xffffffff;
	set_l2_indirect_reg(L2PMOVSR, val);

	return val;
}

static irqreturn_t krait_l2_handle_irq(int irq_num, void *dev)
{
	unsigned long pmovsr;
	struct perf_sample_data data;
	struct pt_regs *regs;
	struct perf_event *event;
	struct hw_perf_event *hwc;
	int bitp;
	int idx = 0;

	pmovsr = get_reset_pmovsr();

	if (!(pmovsr & 0xffffffff))
		return IRQ_NONE;

	regs = get_irq_regs();

	perf_sample_data_init(&data, 0);

	while (pmovsr) {
		bitp = __ffs(pmovsr);

		if (bitp == L2CYCLE_CTR_BIT)
			idx = L2CYCLE_CTR_EVENT_IDX;
		else
			idx = bitp;

		event = krait_l2_pmu_hw_events.events[idx];

		if (!event)
			goto next;

		if (!test_bit(idx, krait_l2_pmu_hw_events.used_mask))
			goto next;

		hwc = &event->hw;

		armpmu_event_update(event, hwc, idx);

		data.period = event->hw.last_period;

		if (!armpmu_event_set_period(event, hwc, idx))
			goto next;

		if (perf_event_overflow(event, &data, regs))
			disable_counter(hwc->idx);
next:
		pmovsr &= (pmovsr - 1);
	}

	irq_work_run();

	return IRQ_HANDLED;
}

static int krait_l2_map_event(struct perf_event *event)
{
	if (pmu_type > 0 && pmu_type == event->attr.type)
		return event->attr.config & 0xfffff;
	else
		return -ENOENT;
}

static int
krait_l2_pmu_generic_request_irq(int irq, irq_handler_t *handle_irq)
{
	return request_irq(irq, *handle_irq,
			IRQF_DISABLED | IRQF_NOBALANCING,
			"krait-l2-armpmu", NULL);
}

static void
krait_l2_pmu_generic_free_irq(int irq)
{
	if (irq >= 0)
		free_irq(irq, NULL);
}

static struct arm_pmu krait_l2_pmu = {
	.id		=	ARM_PERF_PMU_ID_KRAIT_L2,
	.type		=	ARM_PMU_DEVICE_L2CC,
	.name		=	"Krait L2CC PMU",
	.start		=	krait_l2_start,
	.stop		=	krait_l2_stop,
	.handle_irq	=	krait_l2_handle_irq,
	.request_pmu_irq	= krait_l2_pmu_generic_request_irq,
	.free_pmu_irq		= krait_l2_pmu_generic_free_irq,
	.enable		=	krait_l2_enable,
	.disable	=	krait_l2_disable,
	.get_event_idx	=	krait_l2_get_event_idx,
	.read_counter	=	krait_l2_read_counter,
	.write_counter	=	krait_l2_write_counter,
	.map_event	=	krait_l2_map_event,
	.max_period	=	(1LLU << 32) - 1,
	.get_hw_events	=	krait_l2_get_hw_events,
	.num_events	=	MAX_KRAIT_L2_CTRS,
};

static int __devinit krait_l2_pmu_device_probe(struct platform_device *pdev)
{
	krait_l2_pmu.plat_device = pdev;

	if (!armpmu_register(&krait_l2_pmu, "krait-l2", -1))
		pmu_type = krait_l2_pmu.pmu.type;

	return 0;
}

static struct platform_driver krait_l2_pmu_driver = {
	.driver		= {
		.name	= "l2-arm-pmu",
	},
	.probe		= krait_l2_pmu_device_probe,
};

static int __init register_krait_l2_pmu_driver(void)
{
	/* Reset all ctrs */
	set_l2_indirect_reg(L2PMCR, L2PMCR_RESET_ALL);

	/* Avoid spurious interrupt if any */
	get_reset_pmovsr();

	return platform_driver_register(&krait_l2_pmu_driver);
}
device_initcall(register_krait_l2_pmu_driver);