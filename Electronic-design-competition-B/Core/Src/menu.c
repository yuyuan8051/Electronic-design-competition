//
// Created by wyy on 08/18 018.
//
#include "meun.h"
#include "oled.h"

void showState(int state)
{
    switch (state)
    {
    case 0:
        OLED_ShowChineseX(50, 0, "正在检测", 16, 1);
        break;
    case 1:
        OLED_ShowChineseX(50, 0, "结果保持", 16, 1);
        break;
    default:
        return;
    }
}

void showMeum()
{
    OLED_ShowChineseX(0, 0, "状态", 16, 1);
    OLED_ShowChar(33, 0, ':', 16, 1);

    OLED_ShowChineseX(0, 16, "长度", 16, 1);
    OLED_ShowChar(33, 16, ':', 16, 1);

    OLED_ShowChineseX(0, 32, "类型", 16, 1);
    OLED_ShowChar(33, 32, ':', 16, 1);

    OLED_ShowChineseX(0, 48, "参数", 16, 1);
    OLED_ShowChar(33, 48, ':', 16, 1);
}

