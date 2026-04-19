#pragma once

#include <graphics.h>
#include <vector>
#include <string>
#include <Windows.h>

// ============ 常量定义 ============
#define GRID_NUM        32
#define CELL_SIZE       20
#define CANVAS_W        (GRID_NUM * CELL_SIZE)
#define CANVAS_H        (GRID_NUM * CELL_SIZE)
#define UI_WIDTH        220
#define WIN_W           (CANVAS_W + UI_WIDTH)
#define WIN_H           CANVAS_H
#define MAX_HISTORY     50

// 颜色定义
#define COLOR_BG        RGB(40, 44, 52)
#define COLOR_GRID      RGB(60, 60, 60)
#define COLOR_BTN       RGB(70, 75, 85)
#define COLOR_ACT       RGB(100, 180, 255)
#define COLOR_HOVER     RGB(90, 95, 105)
#define COLOR_TRANSPARENT RGB(255, 0, 255)

// 基础颜色
#ifndef WHITE
#define WHITE RGB(255,255,255)
#endif
#ifndef BLACK
#define BLACK RGB(0,0,0)
#endif
#ifndef RED
#define RED RGB(255,0,0)
#endif
#ifndef GREEN
#define GREEN RGB(0,255,0)
#endif
#ifndef BLUE
#define BLUE RGB(0,0,255)
#endif

// 工具类型
enum ToolType {
    TOOL_PEN,
    TOOL_ERASER,
    TOOL_BUCKET,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_CIRCLE
};

// 按钮ID
#define BTN_CLEAR       0
#define BTN_GRID        1
#define BTN_SAVE        2
#define BTN_UNDO        3
#define BTN_REDO        4
#define BTN_IMPORT      5
#define BTN_PEN         10
#define BTN_ERASER      11
#define BTN_BUCKET      12
#define BTN_LINE        13
#define BTN_RECTANGLE   14
#define BTN_CIRCLE      15
#define BTN_SIZE_1      20
#define BTN_SIZE_2      21
#define BTN_SIZE_3      22

// 滤镜按钮
#define BTN_BRIGHTNESS  30
#define BTN_CONTRAST    31
#define BTN_BLUR        32
#define BTN_SHARPEN     33
#define BTN_GRAYSCALE   34
#define BTN_INVERT      35

// 动画按钮
#define BTN_ADD_FRAME   40
#define BTN_REMOVE_FRAME 41
#define BTN_DUPLICATE_FRAME 42
#define BTN_PREV_FRAME  43
#define BTN_NEXT_FRAME  44
#define BTN_PLAY_ANIMATION 45
#define BTN_EXPORT_GIF  46

// ============ 数据结构 ============
struct Button {
    int x, y;
    int w, h;
    const wchar_t* text;
    int id;
    bool enabled;
    bool visible;
    bool isToggle;
    bool isToggled;
};

struct HistoryState {
    COLORREF data[GRID_NUM][GRID_NUM];
};
