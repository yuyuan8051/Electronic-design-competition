#include "delay.h"


static uint32_t g_fac_us = 0;  



void delay_init(uint16_t sysclk)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    g_fac_us = sysclk;                                  
 
}
 

void delay_us(uint32_t nus)
{
    uint32_t temp;
    SysTick->LOAD = nus * g_fac_us; /* 时间加载 */
    SysTick->VAL = 0x00;            /* 清空计数器 */
    SysTick->CTRL |= 1 << 0 ;       /* 开始倒数 */

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); /* CTRL.ENABLE位必须为1, 并等待时间到达 */

    SysTick->CTRL &= ~(1 << 0) ;    /* 关闭SYSTICK */
    SysTick->VAL = 0X00;            /* 清空计数器 */
}


void delay_ms(uint16_t nms)
{
    uint32_t repeat = nms / 30;     
    uint32_t remain = nms % 30;

    while (repeat)
    {
        delay_us(30 * 1000);       
        repeat--;
    }

    if (remain)
    {
        delay_us(remain * 1000);  
    }
}



void HAL_Delay(uint32_t Delay)
{
    delay_ms(Delay);
}









