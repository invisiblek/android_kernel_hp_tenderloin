#ifndef MIPI_M4_H
#define MIPI_M4_H

#include <linux/pwm.h>
#include <linux/mfd/pm8xxx/pm8921.h>

int mipi_m4_device_register(struct msm_panel_info *pinfo,
                                 u32 channel, u32 panel);

#define M4_USE_CMDLISTS 1

#define BRI_SETTING_MIN                 30
#define BRI_SETTING_DEF                 142
#define BRI_SETTING_MAX                 255

#endif
