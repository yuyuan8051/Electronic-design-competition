//
// Created by wyy on 08/18 018.
//


#include "measure_line.h"

void measure_linelong()
{
    line_long = (Time / 2.0) * (299792458 * 0.6760829 * 1.0e-7); // 计算线缆长度,单位cm

    if (line_long < 2030 && line_long >= 950 && Time != 0) // 测9.5~20m
    {
        line_long = (line_long * 0.97 + 38.936);
    }
    else if (line_long < 950 && line_long >= 470 && Time != 0) // 测4.7~10m
    {
        // line_long = (line_long + 23.326) / 0.9211;
        line_long = (line_long + 33.846) / 1.0185;
    }
    else if (line_long < 470 && line_long >= 120 && Time != 0) // 测1.1~4.7m
    {
        line_long = (line_long * 1.0818 - 22.568);
    }
    else if (line_long < 120 && Time != 0) // 测1m以下
    {
        line_long = (line_long * 0.6784 + 25.11);
    }


    //            line_long = (line_long + 33.846) / 1.0185;
}