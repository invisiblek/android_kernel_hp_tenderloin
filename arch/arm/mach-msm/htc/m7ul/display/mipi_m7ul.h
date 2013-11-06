#ifndef MIPI_M7UL_H
#define MIPI_M7UL_H

#include <linux/pwm.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include "../board-m7.h"

int mipi_m7_device_register(struct msm_panel_info *pinfo,
                                 u32 channel, u32 panel);

#define BRI_SETTING_MIN                 30
#define BRI_SETTING_DEF                 142
#define BRI_SETTING_MAX                 255

#endif
