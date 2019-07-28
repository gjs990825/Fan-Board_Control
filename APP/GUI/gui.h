#if !defined(_GUI_H_)
#define _GUI_H_

#include "sys.h"

#define flashInterval 300

typedef struct GUI_Compoment
{
    uint8_t x;                // x坐标
    uint8_t y;                // y坐标
    bool isHighlight;         // 是否高亮
    bool currentHighlight;    // 当前高亮状态
    bool needBlink;           // 需要闪烁
    uint8_t *text;            // 文字内容
    uint8_t size;             // 文字大小
    uint32_t lastTimeRefresh; // 上次刷新时间
    uint16_t refreshInterval; // 刷新间隔
    uint32_t lastTimeFlash;   // 上次闪烁时间
    // uint16_t flashInterval;   // 闪烁间隔 // 取消，默认值
} GUI_Component_t;


void GUI_RefreashTask(GUI_Component_t *components[]);
void OLED_GUI(void);

#endif // _GUI_H_
