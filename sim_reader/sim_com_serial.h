
#ifndef __SIM_COM_SERIAL_H__
#define __SIM_COM_SERIAL_H__

#include "eos_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIMDATA_IO_UART 0
//#define SIMDATA_IO_BLE 0

#define SIM_COM_VIFI_UART_TTYS1 "/dev/ttyS1"
#define REG_DEV_NAME "/dev/regopt"

#define REG_GPIO1_MODE  0x10000060
#define REG_GPIO2_MODE  0x10000064

//GPIO control
#define REG_GPIO_CTRL0  0x10000600
#define REG_GPIO_CTRL1  0x10000604
#define REG_GPIO_DATA0  0x10000620
#define REG_GPIO_DATA1  0x10000624

#define GPIO_DIR_OUT 1
#define GPIO_DIR_IN  0

//sim clk ->PWM0
#define PWM0_ENABLE_ADD     0x10005000
#define PWM0_CON_ADD        0x10005010
#define PWM0_WAVE_NUM_ADD   0x10005038
#define PWM0_DATA_WIDTH_ADD 0x1000503C
#define PWM0_THRESH_ADD     0x10005040

//sim rst
#define SIM_COM_SIM_RST_IO      23
//sim detect
#define SIM_COM_SIM_DET_IOB     22  //MDI_TP_P3,GPIO#22,SD_MODE=01
#define SIM_COM_SIM_DET_IOS     42  //EPHY_LED1_N_JTD1,GPIO#42,P1_LED_AN_MODE =01


_INT32 SimComSerialInit(_VOID);
_INT32 SimComSerialUninit(_INT32 fd);
_INT32 SimComSerialWrite(_INT32 fd, _UCHAR8* p_data, _UINT32 length);
_INT32 SimComSerialRead(_INT32 fd, _UCHAR8* p_data);
_INT32 SimComCardReset(_INT32 fd);
_INT32 SimComCardDetect(_UINT32 *pRetVal);

#ifdef __cplusplus
}
#endif

#endif

