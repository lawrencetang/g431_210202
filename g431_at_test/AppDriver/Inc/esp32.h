/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef __ESP32_H__
#define __ESP32_H__

#include "tos_hal.h"
#include "cmsis_os.h"

typedef enum esp32_net_mode {
    ESP32_NET_MODE_STA,
    ESP32_NET_MODE_AP,
    ESP32_NET_MODE_STA_AP,
} esp32_net_mode_t ;

typedef enum esp32_send_mode {
    ESP32_SEND_MODE_NORMAL,
    ESP32_SEND_MODE_TRANSPARENT,
} esp32_send_mode_t;

typedef enum esp32_multilink_state {
    ESP32_MULTILINK_STATE_ENABLE,
    ESP32_MULTILINK_STATE_DISABLE,
} esp32_multilink_state_t;

#define TOS_CFG_MODULE_SINGLE_LINK_EN       1u

int esp32_sal_init(hal_uart_port_t uart_port);

int esp32_join_ap(const char *ssid, const char *pwd);

int esp32_sal_deinit(void);

extern int esp32_restore(void);

extern int esp32_echo_close(void);

extern int esp32_net_mode_set(esp32_net_mode_t mode);

extern int esp32_send_mode_set(esp32_send_mode_t mode);

extern int esp32_multilink_set(esp32_multilink_state_t state);

#endif /* __ESP8266_H__ */

