/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
//#include "system_init.h"
#include "measure_line.h"
#include "meun.h"
#include "oled.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t TIM2_Flag = 0; // 状态变化
uint8_t TIM_i = 0;     // 为TIM2_PulseArr提供下标变化
uint32_t TIM2_startTime = 0;    // 捕获开始时间
uint32_t TIM2_overTime = 0;     // 捕获结束时间
uint32_t TIM2_AllPulse_add = 0; //  十次捕获测量相加
uint32_t TIM2_captureCount = 0; // 记录捕获过几次

uint32_t TIM2_PulseArr[10] = {0};

double Time = 0;
double line_long = 0;

double allTime_line[2][2] = {0}; // alltime[0]存第一次测试的数据，alltime[1]存第二次测试的数据

// adc
uint16_t sbuff[16] = {0};
uint32_t Vlag_add = 0;
double retValg = 0;

double CAP = 0; // 存储电容计算值
double RES = 0; // 存储电阻计算值
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 冒泡排序
int i = 0, j = 0;
int k = 0;
uint32_t temp = 0;
void bubbling_sort(uint32_t *arr, int32_t sizeArr)
{
    i = 0, j = 0;
    temp = 0;
    for (i = 0; i < sizeArr - 1; i++)
    {
        for (j = 0; j < sizeArr - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_SPI1_Init();
    MX_ADC1_Init();
    /* USER CODE BEGIN 2 */

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    delay_init(400);
    OLED_Init();

    showMeum();

    // HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // 打开输入捕获中断

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        if (((key0_GPIO_Port->IDR & key0_Pin) == RESET)) // 检测长度按键被按下
        {
            while (((key0_GPIO_Port->IDR & key0_Pin) == RESET))
                ;

            while (softRebootInit())
                ; // 初始化所有变量
            while (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1) != HAL_OK)
                ; // 打开输入捕获中断

            while (TIM2_Flag <= 20)
                ; // 控制卡死cpu未捕获完就卡死在这

            OLED_Clear();
            showMeum();

            showState(0); // 0显示"正在检测"

            measure_linelong(); // 计算线缆长度
            allTime_line[0][0] = Time;
            allTime_line[0][1] = line_long;

            OLED_ShowDouble(50, 16, line_long, 4, 2, 16);
            OLED_ShowString(108, 16, (uint8_t *)"cm", 16);

            showState(1); // 1显示“结果保持”
            OLED_ShowChineseX(50, 32, "开路", 16, 1);
            OLED_ShowChineseX(50, 48, "无", 16, 1);
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if ((key1_GPIO_Port->IDR & key1_Pin) == RESET) // 检测负载被按下
        {
            while ((key1_GPIO_Port->IDR & key1_Pin) == RESET)
                ;

            // 判断是否电阻
            HAL_GPIO_WritePin(R_GPIO_Port, R_Pin, 0);
            HAL_Delay(100);
            k = 0;
            // ADC读取
            for (k = 0; k < 16; k++)
            {
                HAL_ADC_Start(&hadc1);
                HAL_ADC_PollForConversion(&hadc1, 50);
                sbuff[k] = HAL_ADC_GetValue(&hadc1);
                HAL_Delay(5);
            }
            Vlag_add = 0;
            k = 0;
            for (k = 4; k < 12; k++)
            {
                Vlag_add += sbuff[k];
            }
            retValg = ((Vlag_add / 8.0) * 3.3 / 65535); // 计算电压
            HAL_GPIO_WritePin(R_GPIO_Port, R_Pin, 1);

            if (retValg < 1) // 判断电阻，小于1v则是电阻
            {

                RES = (retValg / 0.010245); // 计算电阻阻值

                RES = (RES - 4.153657652120467 - 0.001027 * (allTime_line[0][1] - 995)) / 0.4362; // 计算电阻阻值

                OLED_ShowDouble(50, 16, allTime_line[0][1], 4, 2, 16);
                OLED_ShowDouble(50, 48, RES, 3, 3, 16);
                OLED_ShowChineseX(50, 32, "电阻", 16, 1);
                OLED_ShowChineseX(108, 48, "Ω", 16, 1);
                showState(1); // 1显示“结果保持”
            }
            else // 否则是电容或者空载
            {
                while (TIM2_captureCount < 4)
                {
                    if (TIM2_captureCount < 2)
                    {
                        OLED_Clear();
                        showMeum();
                        ++TIM2_captureCount;

                        showState(0); // 0显示"正在检测"
                        while (slow_softRebootInit())
                            ;
                        while (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1) != HAL_OK)
                            ; // 打开输入捕获中断
                    }
                    else // 减少语句执行带来第二次检测的影响
                    {
                        while (TIM2_Flag <= 20)
                            ;

                        measure_linelong(); // 计算第二次捕获的线缆长度

                        allTime_line[1][1] = line_long;
                        allTime_line[1][0] = Time;

                        if (line_long != 0 && allTime_line[0][1] != 0 &&
                            fabs(line_long - allTime_line[0][1]) > 5) // 判断是否电容
                        {
                            if (allTime_line[1][0] > 800)
                            {
                                allTime_line[1][0] = (allTime_line[1][0] + 2 * allTime_line[0][0]) - 1276;
                            }
                            CAP = (allTime_line[1][0] - allTime_line[0][0] - 15) * 9.166659027784143 + 100; // 计算电容容量

                            OLED_ShowDouble(50, 16, allTime_line[0][1], 4, 2, 16);
                            OLED_ShowDouble(50, 48, CAP, 3, 3, 16);
                            OLED_ShowChineseX(50, 32, "电容", 16, 1);
                            OLED_ShowString(108, 48, (uint8_t *)"pF", 16);
                            showState(1); // 1显示“结果保持”
                        }
                        else // 判断是否空载
                        {
                            OLED_ShowDouble(50, 16, line_long, 4, 2, 16);
                            OLED_ShowChineseX(50, 32, "开路", 16, 1);
                            OLED_ShowChineseX(50, 48, "无", 16, 1);
                            showState(1); // 1显示“结果保持”
                        }

                        OLED_ShowString(108, 16, (uint8_t *)"cm", 16);
                        ++TIM2_captureCount;
                    }
                }
                TIM2_captureCount = 1;
            }
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 110;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {
        {
            if (TIM2_Flag >= 20)
            {
                while (HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1) != HAL_OK)
                    ; // 关闭输入捕获
                TIM_i = 0;
                TIM2_AllPulse_add = 0;
                bubbling_sort(TIM2_PulseArr, 10);

                k = 0;
                for (k = 2; k < 8; k++)
                {
                    TIM2_AllPulse_add += TIM2_PulseArr[k];
                }
                TIM2_AllPulse_add /= 6;

                Time = (TIM2_AllPulse_add)*3.636363636363636 - 1272.7272727272727;
                if (Time < 0)
                    Time = 0;
                // OLED_ShowDouble(50, 16, (TIM2_AllPulse_add), 5, 4, 16);
                ++TIM2_captureCount;
                ++TIM2_Flag;
                // button_key = 1;
            }

            else if (TIM2_Flag % 2 == 0) // 捕获到上升沿
            {
                TIM2_startTime = (int32_t)HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
                ++TIM2_Flag;
                // button_key = 0;
            }
            else // 捕获到下升沿
            {
                TIM2_overTime = (int32_t)HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1); // 读取捕获通道的值
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);

                ++TIM2_Flag; // 更新状态机
                if (((TIM2_overTime - TIM2_startTime) >= 350) && ((TIM2_overTime - TIM2_startTime) <= 800))
                    TIM2_PulseArr[TIM_i++] = (TIM2_overTime - TIM2_startTime); // 十次的结果相加
                else
                    TIM2_Flag -= 2;

                TIM2_startTime = 0;
                TIM2_overTime = 0;
            }
        }
    }
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
