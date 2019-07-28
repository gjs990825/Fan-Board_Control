#include "gui.h"
#include "delay.h"
#include "oled.h"
#include "timer.h"
#include <stdio.h>
#include "my_lib.h"

// GUI里面固定的只需要显示一次的界面内容
void OLED_GUI_ONCE(void)
{
    OLED_ShowStr(0, 4, "Angle:", 2, false);
    OLED_ShowStr(0, 4, "Target:", 2, false);
}

uint8_t angleString[10] = "";
uint8_t targetString[10] = "";

// Agnle文字
GUI_Component_t Text_Angle = {
    .x = 0,
    .y = 0,
    .isHighlight = false,
    .currentHighlight = false,
    .needBlink = true,
    .text = "Angle:",
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 1000,
    .lastTimeFlash = 0,
};

// Angle数值
GUI_Component_t AngleValue = {
    .x = 48,
    .y = 0,
    .isHighlight = false,
    .currentHighlight = false,
    .needBlink = false,
    .text = angleString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 100,
    .lastTimeFlash = 0,
};

// Target文字
GUI_Component_t Text_Target = {
    .x = 0,
    .y = 2,
    .isHighlight = false,
    .currentHighlight = false,
    .needBlink = false,
    .text = "Target:",
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 1000,
    .lastTimeFlash = 0,
};

// Target数值
GUI_Component_t TargetValue = {
    .x = 56,
    .y = 2,
    .isHighlight = false,
    .currentHighlight = false,
    .needBlink = true,
    .text = targetString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 100,
    .lastTimeFlash = 0,
};

// 控件集合
GUI_Component_t *componentsSet[] = {
    &Text_Angle,
    &Text_Target,
    &AngleValue,
    &TargetValue};

// 控件集合内容个数
uint16_t componentsNumber = GET_ARRAY_LENGEH(componentsSet);

void GUI_RefreashTask(GUI_Component_t *components[])
{
    for (uint16_t i = 0; i < componentsNumber; i++)
    {
        // 刷新任务
        if (IsTimeOut(components[i]->lastTimeRefresh, components[i]->refreshInterval))
        {
            components[i]->lastTimeRefresh = millis();

            OLED_ShowStr(components[i]->x,
                         components[i]->y,
                         components[i]->text,
                         components[i]->size,
                         components[i]->currentHighlight);
        }
        // 闪烁任务
        if (components[i]->needBlink && IsTimeOut(components[i]->lastTimeFlash, flashInterval))
        {
            components[i]->lastTimeFlash = millis();
            components[i]->currentHighlight = !components[i]->currentHighlight;

            OLED_ShowStr(components[i]->x,
                         components[i]->y,
                         components[i]->text,
                         components[i]->size,
                         components[i]->currentHighlight);
        }
    }
}

void OLED_GUI(void)
{
    OLED_CLS();

    // 需要刷新和闪烁的控件
    while (1)
    {
        sprintf((char *)AngleValue.text, "%5.3f", Get_CurrentAngle());
        sprintf((char *)TargetValue.text, "%5.3d", millis());
        

        GUI_RefreashTask(componentsSet);
        delay_ms(20);
    }
}
