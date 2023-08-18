//
// Created by wyy on 08/18 018.
//

#include "system_init.h"

bool softRebootInit(void)
{
    TIM2_Flag = 0; // 状态变化
    TIM_i = 0;     // 为TIM2_PulseArr提供下标变化

    TIM2_startTime = 0;    // 捕获开始时间
    TIM2_overTime = 0;     // 捕获结束时间
    TIM2_AllPulse_add = 0; //  十次捕获测量相加
    TIM2_captureCount = 0; // 记录捕获过几次

    for (int i = 0; i < 10; i++)
    {
        TIM2_PulseArr[i] = 0;
    }

    Time = 0;
    line_long = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            allTime_line[i][j] = 0; // alltime[0]存第一次测试的数据，alltime[1]存第二次测试的数据
        }
    }
    // adc
    for (int i = 0; i < 16; i++)
    {
        sbuff[i] = 0;
    }

    Vlag_add = 0;
    retValg = 0;

    CAP = 0; // 存储电容计算值
    RES = 0; // 存储电阻计算值

    return false;
}

bool slow_softRebootInit(void)
{
    TIM2_Flag = 0; // 状态变化
    TIM_i = 0;     // 为TIM2_PulseArr提供下标变化

    TIM2_startTime = 0;    // 捕获开始时间
    TIM2_overTime = 0;     // 捕获结束时间
    TIM2_AllPulse_add = 0; //  十次捕获测量相加

    for (int i = 0; i < 10; i++)
    {
        TIM2_PulseArr[i] = 0;
    }

    Time = 0;
    line_long = 0;

    allTime_line[1][0] = 0;
    allTime_line[1][1] = 0;

    // adc
    for (int i = 0; i < 16; i++)
    {
        sbuff[i] = 0;
    }

    Vlag_add = 0;
    retValg = 0;

    CAP = 0; // 存储电容计算值
    RES = 0; // 存储电阻计算值

    return false;
}
