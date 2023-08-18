//
// Created by wyy on 08/18 018.
//

#ifndef __SYSTEM_INIT_H
#define __SYSTEM_INIT_H

#include "main.h"
#include <stdbool.h>

extern uint8_t TIM2_Flag; // 状态变化
extern uint8_t TIM_i;     // 为TIM2_PulseArr提供下标变化 //
extern uint32_t TIM2_startTime;    // 捕获开始时间
extern uint32_t TIM2_overTime;     // 捕获结束时间
extern uint32_t TIM2_AllPulse_add; // 十次捕获测量相加
extern uint32_t TIM2_captureCount; // 记录捕获过几次

extern uint32_t TIM2_PulseArr[10];

extern double Time;
extern double line_long;

extern double allTime_line[2][2]; // alltime[0]存第一次测试的数据，alltime[1]存第二次测试的数据

// adc
extern uint16_t sbuff[16];
extern uint32_t Vlag_add;
extern double retValg;

extern double CAP; // 存储电容计算值
extern double RES; // 存储电阻计算值

bool softRebootInit(void);
bool slow_softRebootInit(void);

#endif // CODE_VGT6_SYSTEM_INIT_H
