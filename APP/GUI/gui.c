#include "gui.h"
#include "delay.h"
#include "oled.h"
#include "timer.h"
#include <stdio.h>
#include "my_lib.h"

// 文字储存buffer
uint8_t angleString[10] = "";
uint8_t targetString[10] = "";
uint8_t timeString[10] = "";
uint8_t stabilizedString[10] = "";

// Agnle文字
GUI_Component_t Text_Angle = {
    .x = 0,
    .y = 0,
    .currentHighlight = false,
    .needBlink = false,
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
    .currentHighlight = false,
    .needBlink = false,
    .text = angleString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 90,
    .lastTimeFlash = 0,
};

// Target文字
GUI_Component_t Text_Target = {
    .x = 0,
    .y = 2,
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
    .currentHighlight = false,
    .needBlink = false,
    .text = targetString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 90,
    .lastTimeFlash = 0,
};

// Stabilized文字
GUI_Component_t StabilizedText = {
    .x = 0,
    .y = 4,
    .currentHighlight = false,
    .needBlink = false,
    .text = "Stabilized:",
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 1000,
    .lastTimeFlash = 0,
};

// Stabilized数值
GUI_Component_t StabilizedValue = {
    .x = 88,
    .y = 4,
    .currentHighlight = false,
    .needBlink = false,
    .text = stabilizedString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 90,
    .lastTimeFlash = 0,
};

// Time文字
GUI_Component_t TimeText = {
    .x = 0,
    .y = 6,
    .currentHighlight = false,
    .needBlink = false,
    .text = "Time:",
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 1000,
    .lastTimeFlash = 0,
};
// Time数值
GUI_Component_t TimeValue = {
    .x = 40,
    .y = 6,
    .currentHighlight = false,
    .needBlink = false,
    .text = timeString,
    .size = 2,
    .lastTimeRefresh = 0,
    .refreshInterval = 90,
    .lastTimeFlash = 0,
};

// 控件集合
GUI_Component_t *componentsSet[] = {
    &Text_Angle,
    &AngleValue,
    &Text_Target,
    &TargetValue,
    &StabilizedText,
    &StabilizedValue,
    &TimeText,
    &TimeValue,
};

// 控件集合内容个数
uint16_t componentsNumber = GET_ARRAY_LENGEH(componentsSet);

// 设置闪烁状态
void GUI_SetFlash(GUI_Component_t *component, bool status)
{
}

// 更新可变数据
void OLED_UpdateData(void)
{
    sprintf((char *)AngleValue.text, "%5.2f", Get_CurrentAngle());
    sprintf((char *)TimeValue.text, "%5.2f", millis() / 1000.0);
    sprintf((char *)TargetValue.text, "%5.2f", Get_TargetAngle());
    sprintf((char *)StabilizedValue.text, "%s", Is_Stablilized() ? "YES" : "NO ");
}

// 闪烁控件
void OLED_FlashComponent(GUI_Component_t *components)
{
    components->lastTimeFlash = millis();
    components->currentHighlight = !components->currentHighlight;

    OLED_ShowStr(components->x,
                 components->y,
                 components->text,
                 components->size,
                 components->currentHighlight);
}

// 刷新控件
void OLED_RefreashComponent(GUI_Component_t *components)
{
    components->lastTimeRefresh = millis();

    OLED_ShowStr(components->x,
                 components->y,
                 components->text,
                 components->size,
                 components->currentHighlight);
}

// 界面刷新任务
void GUI_RefreashInterface(GUI_Component_t *components[], uint16_t compomentNumber)
{
    OLED_UpdateData();

    for (uint16_t i = 0; i < compomentNumber; i++)
    {
        // 刷新任务
        if (IsTimeOut(components[i]->lastTimeRefresh, components[i]->refreshInterval))
        {
            OLED_RefreashComponent(components[i]);

            // components[i]->lastTimeRefresh = millis();

            // OLED_ShowStr(components[i]->x,
            //              components[i]->y,
            //              components[i]->text,
            //              components[i]->size,
            //              components[i]->currentHighlight);
        }
        // 闪烁任务
        if (components[i]->needBlink && IsTimeOut(components[i]->lastTimeFlash, flashInterval))
        {
            OLED_FlashComponent(components[i]);
            // components[i]->lastTimeFlash = millis();
            // components[i]->currentHighlight = !components[i]->currentHighlight;

            // OLED_ShowStr(components[i]->x,
            //              components[i]->y,
            //              components[i]->text,
            //              components[i]->size,
            //              components[i]->currentHighlight);
        }
    }
}
