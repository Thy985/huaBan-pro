/*
 * 像素画编辑器 
 * 版本: 1.0
 * 开发者: Pixel Editor Team
 * 发布日期: 2025年
 * 版权所有: (C) 2024 Pixel Editor Team. 保留所有权利。
 *
 * 本程序是一个功能强大的像素画创作工具，专为像素艺术爱好者设计。
 * 主要特点包括：
 * - 32x32像素画布编辑
 * - 多种绘图工具（画笔、橡皮擦、油漆桶）
 * - 调色板系统
 * - 撤销/重做功能
 * - 画布缩放和拖动
 * - 图像导入导出
 * - 网格显示控制
 *
 * 使用说明：
 * 1. 选择左侧工具栏中的工具
 * 2. 从调色板中选择颜色
 * 3. 在画布上进行绘画
 * 4. 使用按钮保存作品或执行其他操作
 */

#include <graphics.h>  // EasyX 图形库，提供绘图功能
#include <conio.h>     // 控制台输入输出库
#include <stdio.h>     // 标准输入输出
#include <time.h>      // 时间函数，用于随机数生成等
#include <vector>      // 用于实现撤销栈和重做栈的数据结构
#include <windows.h>   // Windows API，为鼠标位置获取和屏幕坐标转换提供支持
#include <string>      // 字符串处理功能
#include <algorithm>   // 算法库，用于数组操作等
#include <unordered_map> // 哈希表，用于颜色统计
#include <map>         // 映射表，作为替代方案

// --- 常量定义 ---\n// 本部分定义程序中使用的各种常量，包括画布大小、UI尺寸、颜色等
// 常量定义使得程序更容易维护和修改
// 画布相关常量定义
#define GRID_NUM    32    // 网格数量（32x32）
#define CELL_SIZE   20    // 每个单元格的大小（像素）
#define CANVAS_W    (GRID_NUM * CELL_SIZE)  // 画布宽度
#define CANVAS_H    (GRID_NUM * CELL_SIZE)  // 画布高度
#define UI_WIDTH    220   // 用户界面宽度
#define WIN_W       (CANVAS_W + UI_WIDTH)   // 窗口总宽度
#define WIN_H       CANVAS_H                // 窗口高度

// 最大历史记录步数定义
#define MAX_HISTORY 50    // 最大撤销历史记录步数

// 界面颜色常量定义
#define COLOR_BG    RGB(40, 44, 52)    // 背景颜色
#define COLOR_GRID  RGB(60, 60, 60)    // 网格线颜色
#define COLOR_BTN   RGB(70, 75, 85)    // 按钮默认颜色
#define COLOR_ACT   RGB(100, 180, 255) // 选中工具的颜色
#define COLOR_HOVER RGB(90, 95, 105)   // 鼠标悬停时的按钮颜色

// 特殊颜色标记
#define COLOR_TRANSPARENT RGB(255, 0, 255)  // 透明色标记（在图像导入时使用）

// 画笔形状常量定义
#define BRUSH_CIRCLE      0   // 圆形画笔
#define BRUSH_SQUARE      1   // 方形画笔
#define BRUSH_DIAMOND     2   // 菱形画笔
#define BRUSH_STAR        3   // 星形画笔
#define BRUSH_SIZE        1   // 默认画笔大小

// 图案类型常量定义
#define PATTERN_GRID               0   // 网格图案
#define PATTERN_DOTS               1   // 点状图案
#define PATTERN_STRIPES_HORIZONTAL 2   // 水平条纹
#define PATTERN_STRIPES_VERTICAL   3   // 垂直条纹
#define PATTERN_CHECKERBOARD       4   // 棋盘格图案
#define PATTERN_SIZE               8   // 图案大小

// 设置类型常量定义
#define SETTING_TYPE_BOOL    0   // 布尔类型设置
#define SETTING_TYPE_INT     1   // 整数类型设置
#define SETTING_TYPE_FLOAT   2   // 浮点类型设置
#define SETTING_TYPE_STRING  3   // 字符串类型设置

// 确保基础颜色定义存在
#ifndef WHITE
#define WHITE RGB(255,255,255)   // 白色
#endif
#ifndef BLACK
#define BLACK RGB(0,0,0)         // 黑色
#endif
#ifndef RED
#define RED RGB(255,0,0)         // 红色
#endif
#ifndef GREEN
#define GREEN RGB(0,255,0)       // 绿色
#endif
#ifndef BLUE
#define BLUE RGB(0,0,255)        // 蓝色
#endif
#ifndef YELLOW
#define YELLOW RGB(255,255,0)    // 黄色
#endif
#ifndef CYAN
#define CYAN RGB(0,255,255)      // 青色
#endif
#ifndef MAGENTA
#define MAGENTA RGB(255,0,255)   // 品红
#endif

// 工具类型枚举定义
// 用于标识当前使用的绘图工具
enum ToolType {
    TOOL_PEN,     // 画笔工具
    TOOL_ERASER,  // 橡皮擦工具
    TOOL_BUCKET   // 油漆桶填充工具
};

// --- 全局变量 ---\n// 本部分定义程序中使用的全局变量
// 全局变量虽然方便但应当谨慎使用，这里主要用于存储编辑器的状态信息
// 画布数据
COLORREF gridData[GRID_NUM][GRID_NUM];  // 存储画布上每个像素的颜色数据

// 工具和颜色状态
COLORREF curColor = RED;                // 当前选中的颜色
bool showGrid = true;                   // 是否显示网格
ToolType currentTool = TOOL_PEN;        // 当前使用的工具
int brushSize = 1;                      // 画笔大小 (1-3)
int backgroundStyle = 0;                // 背景样式: 0-棋盘格, 1-浅色, 2-深色

// 最近使用颜色数组
COLORREF recentColors[8];               // 存储最近使用的8种颜色
int recentColorCount = 0;               // 最近使用颜色的数量

// 画布缩放和视图控制相关变量
int zoomLevel = 1;              // 缩放级别 (1: 100%, 2: 200%, 4: 400%)
int offsetX = 0, offsetY = 0;   // 视图偏移坐标
int prevMouseX = 0, prevMouseY = 0; // 上一次鼠标位置
bool isDraggingCanvas = false;  // 是否正在拖动画布

// 性能优化标志
bool needCanvasUpdate = true;    // 画布是否需要更新
bool needUIUpdate = true;        // UI是否需要更新

// 历史记录相关结构和变量
// 用于实现撤销和重做功能

// --- 图层结构体定义 ---
// 用于表示绘图应用中的图层概念
struct Layer {
    std::wstring name;             // 图层名称
    COLORREF data[GRID_NUM][GRID_NUM]; // 图层像素数据
    bool visible;                  // 图层是否可见
    float opacity;                 // 图层透明度 (0.0 - 1.0)
    
    // 构造函数
    Layer() : name(L"图层"), visible(true), opacity(1.0f) {
        // 初始化图层数据为透明色
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                data[x][y] = BLACK;
            }
        }
    }
};

// 动画帧结构体定义
struct AnimationFrame {
    COLORREF data[GRID_NUM][GRID_NUM]; // 帧像素数据
    int delay;                        // 帧延迟（毫秒）
    
    // 构造函数
    AnimationFrame() : delay(100) {
        // 初始化帧数据为透明色
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                data[x][y] = BLACK;
            }
        }
    }
};

// 文本样式结构体定义
struct TextStyle {
    int fontSize;             // 字体大小
    bool bold;                // 是否粗体
    bool italic;              // 是否斜体
    std::wstring fontFamily;  // 字体名称
    COLORREF textColor;       // 文本颜色
    
    // 构造函数
    TextStyle() : fontSize(12), bold(false), italic(false), fontFamily(L"宋体"), textColor(BLACK) {}
};

// 设置值结构体定义
struct SettingValue {
    int type;                 // 设置类型
    bool boolValue;           // 布尔值
    int intValue;             // 整数值
    std::wstring stringValue; // 字符串值
    float floatValue;         // 浮点数值
    
    // 构造函数
    SettingValue() : type(0), boolValue(false), intValue(0), floatValue(0.0f) {}
};

// 快捷键结构体定义
struct Shortcut {
    int key;                  // 键码
    int modifier;             // 修饰键
    int actionId;             // 动作ID
    
    // 构造函数
    Shortcut() : key(0), modifier(0), actionId(0) {}
};

// 历史状态结构定义
struct HistoryState {
    COLORREF data[GRID_NUM][GRID_NUM];  // 存储画布状态的二维数组
};

// 颜色分析结果结构体定义
struct ColorAnalysisResult {
    int totalPixels;       // 总像素数
    int transparentPixels; // 透明像素数
};

// 撤销栈和重做栈
std::vector<HistoryState> undoStack;  // 用于存储可撤销的历史状态
std::vector<HistoryState> redoStack;  // 用于存储可重做的历史状态

// 图层系统相关变量
std::vector<Layer> layers;            // 图层列表
int currentLayer = 0;                 // 当前活动图层的索引

// 图层管理函数声明
void UpdateLayersVisibility();        // 更新图层可见性

// 选区相关变量
bool selecting = false;               // 是否正在进行选择操作
bool isSelecting = false;             // 是否正在进行选择操作（别名，保持兼容性）
POINT selectionStart;                 // 选区的起始点（屏幕坐标）
POINT selectionEnd;                   // 选区的结束点（屏幕坐标）
POINT selectionGridStart;             // 选区的起始点（网格坐标）
POINT selectionGridEnd;               // 选区的结束点（网格坐标）
POINT SelectionStart;                 // 选区的起始点（屏幕坐标，大写版本）
POINT SelectionEnd;                   // 选区的结束点（屏幕坐标，大写版本）
bool hasSelection = false;            // 是否存在活动选区

// 剪贴板相关变量
COLORREF** clipboardData = nullptr;    // 剪贴板数据
int clipboardWidth = 0;               // 剪贴板宽度
int clipboardHeight = 0;              // 剪贴板高度

// 动画相关变量
std::vector<COLORREF**> animationFrames; // 动画帧列表
int currentFrameIndex = 0;           // 当前帧索引
bool isPreviewing = false;           // 是否正在预览动画
int previewFrameIndex = 0;           // 预览帧索引
clock_t previewStartTime = 0;        // 预览开始时间

// 自定义画笔数据
COLORREF* customBrushData = nullptr;  // 自定义画笔数据

// 设置相关
std::vector<COLORREF> colorSet;       // 颜色集合
std::vector<std::wstring> settings;   // 设置列表

// 缩放相关
float scaledCellSize = CELL_SIZE;     // 缩放后的单元格大小

// 调色板定义
// 固定的16种预设颜色
COLORREF palette[] = {
    BLACK, WHITE, RGB(200,200,200), RGB(100,100,100),  // 黑白灰系列
    RED, GREEN, BLUE, YELLOW,                         // 基本颜色
    RGB(255, 165, 0), RGB(128, 0, 128), RGB(0, 255, 255), RGB(255, 192, 203), // 扩展颜色
    RGB(139, 69, 19), RGB(34, 139, 34), RGB(75, 0, 130), RGB(255, 215, 0)     // 其他颜色
};

// 调色板颜色名称
const wchar_t* paletteNames[] = {
    L"黑色", L"白色", L"浅灰", L"深灰",
    L"红色", L"绿色", L"蓝色", L"黄色",
    L"橙色", L"紫色", L"青色", L"粉色",
    L"棕色", L"森林绿", L"靛蓝", L"金色"
};

// 自定义颜色数组
COLORREF customColors[16];          // 存储用户自定义的16种颜色
bool customColorUsed[16] = {false}; // 标记自定义颜色是否已使用

// 按钮结构定义
struct Button {
    int x, y;           // 按钮位置
    int w, h;           // 按钮宽度和高度
    const wchar_t* text;// 按钮文本
    int id;             // 按钮ID，用于识别不同功能
    bool enabled;       // 按钮是否可用
    bool visible;       // 按钮是否可见
    bool isToggle;      // 是否为开关按钮
    bool isToggled;     // 开关状态
};

// 按钮ID常量定义
#define BTN_CLEAR   0   // 清空画布
#define BTN_GRID    1   // 显示/隐藏网格
#define BTN_SAVE    2   // 保存作品
#define BTN_UNDO    3   // 撤销操作
#define BTN_REDO    4   // 重做操作
#define BTN_IMPORT  5   // 导入图像
#define BTN_PEN     10  // 画笔工具
#define BTN_ERASER  11  // 橡皮擦工具
#define BTN_BUCKET  12  // 油漆桶工具
#define BTN_SIZE_1  20  // 画笔大小1
#define BTN_SIZE_2  21  // 画笔大小2
#define BTN_SIZE_3  22  // 画笔大小3

// 按钮向量
std::vector<Button> btns;            // 存储所有UI按钮

// --- 函数声明 ---\n// 本部分声明程序中使用的所有函数
// 函数声明按照功能模块进行组织，便于代码维护
// 初始化和主循环相关函数
void InitGame();                    // 初始化游戏和界面
void MainLoop();                    // 主循环处理

// 绘制相关函数
void DrawInterface();               // 绘制用户界面
void UpdateCanvas();                // 更新和绘制画布
void DrawToolButtons();             // 绘制工具按钮
void DrawColorPalette();            // 绘制调色板
void DrawCanvasBorder();            // 绘制画布边框

// 输入处理函数
void HandleMouse();                 // 处理鼠标事件
void HandleKeys();                  // 处理键盘事件
void ProcessMouseClick(int mx, int my);  // 处理鼠标点击
void ProcessMouseDrag(int mx, int my);   // 处理鼠标拖动

// 文件操作函数
void SaveArtwork();                 // 保存作品
void ImportImage();                 // 导入图像

// 历史记录相关函数
void SaveHistory();                 // 保存当前状态到历史
void PerformUndo();                 // 执行撤销
void PerformRedo();                 // 执行重做
void ClearHistory();                // 清空历史记录

// 绘图工具函数
void FloodFill(int x, int y, COLORREF targetColor, COLORREF replaceColor); // 油漆桶填充算法
void DrawPixel(int x, int y, COLORREF color);  // 绘制单个像素
void DrawBrush(int gridX, int gridY, COLORREF color); // 绘制画笔效果
void EraseBrush(int gridX, int gridY);         // 擦除画笔效果

// UI交互函数
bool IsButtonClicked(Button b, int mx, int my); // 检查按钮是否被点击
Button* FindButtonById(int id);                // 通过ID查找按钮

// 辅助函数
void ResetCanvas();                 // 重置画布
void UpdateRecentColors(COLORREF color); // 更新最近使用的颜色
void ApplyZoom(int level);          // 应用缩放级别
void PanCanvas(int dx, int dy);     // 平移画布
void DrawBackgroundPattern();       // 绘制背景图案
void InitializeCustomColors();      // 初始化自定义颜色

// --- 主函数 ---
int main() {
    // 初始化图形环境
    initgraph(WIN_W, WIN_H);
    
    // 初始化游戏和界面
    InitGame();

    // 主循环
    while (true) {
        BeginBatchDraw();
        cleardevice();

        // 先绘制背景图案
        DrawBackgroundPattern();
        
        // 先绘制画布（底层），再绘制 UI（顶层）
        UpdateCanvas();
        DrawInterface();

        EndBatchDraw();

        // 处理用户输入
        HandleMouse();
        HandleKeys();

        // 短暂休眠，控制帧率
        Sleep(10);
    }
    
    // 清理图形环境
    closegraph();
    return 0;
}

// --- 主循环处理函数 ---
// 封装主循环的处理逻辑
void MainLoop() {
    // 主循环已在main函数中实现
    // 此函数作为逻辑封装，便于将来的扩展
}

// --- 绘制工具按钮函数 ---
// 绘制工具栏中的按钮
void DrawToolButtons() {
    // 绘制工具按钮（画笔、橡皮擦、油漆桶）
    for (auto& btn : btns) {
        if (!btn.visible) continue;
        
        // 设置按钮颜色
        COLORREF btnColor = btn.enabled ? COLOR_BTN : RGB(50, 55, 65);
        COLORREF textColor = WHITE;
        
        // 检查鼠标悬停状态
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(GetHWnd(), &mousePos);
        
        if (IsButtonClicked(btn, mousePos.x, mousePos.y) && btn.enabled) {
            btnColor = COLOR_HOVER;
        }
        
        // 检查是否为选中的工具按钮
        if (btn.id >= BTN_PEN && btn.id <= BTN_BUCKET) {
            if ((btn.id == BTN_PEN && currentTool == TOOL_PEN) ||
                (btn.id == BTN_ERASER && currentTool == TOOL_ERASER) ||
                (btn.id == BTN_BUCKET && currentTool == TOOL_BUCKET)) {
                btnColor = COLOR_ACT;
            }
        }
        
        // 检查画笔大小按钮
        if (btn.id >= BTN_SIZE_1 && btn.id <= BTN_SIZE_3) {
            int size = btn.id - BTN_SIZE_1 + 1;
            if (size == brushSize) {
                btnColor = COLOR_ACT;
            }
        }
        
        // 绘制按钮背景
        setfillcolor(btnColor);
        setlinecolor(RGB(100, 100, 100));
        fillrectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);
        
        // 绘制按钮文本
        settextcolor(textColor);
        setbkmode(TRANSPARENT);
        
        // 计算文本位置以居中
        RECT textRect;
        textRect.left = btn.x;
        textRect.top = btn.y;
        textRect.right = btn.x + btn.w;
        textRect.bottom = btn.y + btn.h;
        drawtext(btn.text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

// --- 绘制调色板函数 ---
// 绘制颜色选择器和调色板
void DrawColorPalette() {
    int paletteX = 20;
    int paletteY = 250;
    int colorSize = 24;
    int colorSpacing = 30;
    
    // 绘制标题
    settextcolor(WHITE);
    outtextxy(paletteX, paletteY - 25, L"调色板");
    
    // 绘制预设颜色
    for (int i = 0; i < 16; i++) {
        int x = paletteX + (i % 4) * colorSpacing;
        int y = paletteY + (i / 4) * colorSpacing;
        
        // 绘制颜色方块
        setfillcolor(palette[i]);
        setlinecolor(curColor == palette[i] ? COLOR_ACT : RGB(100, 100, 100));
        setlinestyle(curColor == palette[i] ? PS_SOLID : PS_SOLID, 2);
        fillrectangle(x, y, x + colorSize, y + colorSize);
        setlinestyle(PS_SOLID, 1);
        
        // 绘制颜色名称（可选择性显示）
        if (i % 4 == 0) {
            // 为了节省空间，只在第一列显示颜色名称
            settextcolor(WHITE);
            outtextxy(x + colorSize + 5, y, paletteNames[i]);
        }
    }
    
    // 绘制最近使用的颜色
    paletteY += 130;
    outtextxy(paletteX, paletteY - 25, L"最近使用");
    
    for (int i = 0; i < recentColorCount && i < 8; i++) {
        int x = paletteX + (i % 4) * colorSpacing;
        int y = paletteY + (i / 4) * colorSpacing;
        
        setfillcolor(recentColors[i]);
        setlinecolor(curColor == recentColors[i] ? COLOR_ACT : RGB(100, 100, 100));
        fillrectangle(x, y, x + colorSize, y + colorSize);
    }
}

// --- 绘制画布边框函数 ---
// 绘制画布的边框和信息
void DrawCanvasBorder() {
    // 绘制画布边框
    setlinecolor(RGB(150, 150, 150));
    setlinestyle(PS_SOLID, 2);
    rectangle(UI_WIDTH - 2, -2, WIN_W + 1, WIN_H + 1);
    setlinestyle(PS_SOLID, 1);
    
    // 显示当前工具和颜色信息
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    
    // 显示当前工具
    const wchar_t* toolName = L"";
    switch (currentTool) {
        case TOOL_PEN:
            toolName = L"工具: 画笔";
            break;
        case TOOL_ERASER:
            toolName = L"工具: 橡皮擦";
            break;
        case TOOL_BUCKET:
            toolName = L"工具: 油漆桶";
            break;
    }
    outtextxy(UI_WIDTH + 10, 10, toolName);
    
    // 显示当前颜色
    outtextxy(UI_WIDTH + 10, 30, L"当前颜色:");
    setfillcolor(curColor);
    setlinecolor(WHITE);
    fillrectangle(UI_WIDTH + 80, 30, UI_WIDTH + 100, 50);
    
    // 显示画笔大小
    wchar_t sizeText[50];
    swprintf(sizeText, 100, L"画笔大小: %d", brushSize);
    outtextxy(UI_WIDTH + 120, 30, sizeText);
    
    // 显示缩放级别
    wchar_t zoomText[50];
    swprintf(zoomText, 100, L"缩放: %d%%", zoomLevel * 100);
    outtextxy(WIN_W - 120, 10, zoomText);
}

// --- 处理鼠标点击函数 ---
// 处理鼠标点击事件的详细逻辑
void ProcessMouseClick(int mx, int my) {
    // 检查是否点击了UI区域
    if (mx < UI_WIDTH) {
        // 检查按钮点击
        for (auto& btn : btns) {
            if (IsButtonClicked(btn, mx, my) && btn.enabled) {
                // 处理按钮点击
                switch (btn.id) {
                    case BTN_CLEAR:
                        // 清空画布
                        SaveHistory();
                        ResetCanvas();
                        break;
                    case BTN_GRID:
                        // 切换网格显示
                        showGrid = !showGrid;
                        btn.isToggled = showGrid;
                        break;
                    case BTN_SAVE:
                        // 保存作品
                        SaveArtwork();
                        break;
                    case BTN_UNDO:
                        // 撤销操作
                        PerformUndo();
                        break;
                    case BTN_REDO:
                        // 重做操作
                        PerformRedo();
                        break;
                    case BTN_IMPORT:
                        // 导入图像
                        ImportImage();
                        break;
                    case BTN_PEN:
                        // 选择画笔工具
                        currentTool = TOOL_PEN;
                        break;
                    case BTN_ERASER:
                        // 选择橡皮擦工具
                        currentTool = TOOL_ERASER;
                        break;
                    case BTN_BUCKET:
                        // 选择油漆桶工具
                        currentTool = TOOL_BUCKET;
                        break;
                    case BTN_SIZE_1:
                    case BTN_SIZE_2:
                    case BTN_SIZE_3:
                        // 设置画笔大小
                        brushSize = btn.id - BTN_SIZE_1 + 1;
                        break;
                }
                return;
            }
        }
        
        // 检查调色板点击
        int paletteX = 20;
        int paletteY = 250;
        int colorSize = 24;
        int colorSpacing = 30;
        
        // 检查预设颜色点击
        for (int i = 0; i < 16; i++) {
            int x = paletteX + (i % 4) * colorSpacing;
            int y = paletteY + (i / 4) * colorSpacing;
            
            if (mx >= x && mx <= x + colorSize && my >= y && my <= y + colorSize) {
                curColor = palette[i];
                UpdateRecentColors(curColor);
                return;
            }
        }
        
        // 检查最近使用颜色点击
        paletteY += 130;
        for (int i = 0; i < recentColorCount && i < 8; i++) {
            int x = paletteX + (i % 4) * colorSpacing;
            int y = paletteY + (i / 4) * colorSpacing;
            
            if (mx >= x && mx <= x + colorSize && my >= y && my <= y + colorSize) {
                curColor = recentColors[i];
                UpdateRecentColors(curColor);
                return;
            }
        }
    } else {
        // 处理画布点击
        int gridX = (mx - UI_WIDTH - offsetX) / (CELL_SIZE * zoomLevel);
        int gridY = (my - offsetY) / (CELL_SIZE * zoomLevel);
        
        if (gridX >= 0 && gridX < GRID_NUM && gridY >= 0 && gridY < GRID_NUM) {
            SaveHistory();
            
            switch (currentTool) {
                case TOOL_PEN:
                    DrawBrush(gridX, gridY, curColor);
                    break;
                case TOOL_ERASER:
                    EraseBrush(gridX, gridY);
                    break;
                case TOOL_BUCKET:
                    FloodFill(gridX, gridY, gridData[gridX][gridY], curColor);
                    break;
            }
        }
    }
}

// --- 处理鼠标拖动函数 ---
// 处理鼠标拖动事件
void ProcessMouseDrag(int mx, int my) {
    if (isDraggingCanvas) {
        // 计算偏移量
        int dx = mx - prevMouseX;
        int dy = my - prevMouseY;
        
        // 平移画布
        PanCanvas(dx, dy);
        
        // 更新上一次鼠标位置
        prevMouseX = mx;
        prevMouseY = my;
    } else if (mx >= UI_WIDTH) {
        // 在画布上拖动进行绘画
        int gridX = (mx - UI_WIDTH - offsetX) / (CELL_SIZE * zoomLevel);
        int gridY = (my - offsetY) / (CELL_SIZE * zoomLevel);
        
        if (gridX >= 0 && gridX < GRID_NUM && gridY >= 0 && gridY < GRID_NUM) {
            switch (currentTool) {
                case TOOL_PEN:
                    DrawBrush(gridX, gridY, curColor);
                    break;
                case TOOL_ERASER:
                    EraseBrush(gridX, gridY);
                    break;
            }
        }
    }
}

// --- 导入图像函数 ---
// 从文件导入图像到画布
void ImportImage() {
    // 打开文件对话框
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = TEXT("");
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Image Files (*.bmp;*.jpg;*.png)\0*.bmp;*.jpg;*.png\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn) == TRUE) {
        // 保存历史记录
        SaveHistory();
        
        // 加载图像
        IMAGE img;
        if (loadimage(&img, szFile)) {
            // 获取图像宽度和高度
            int imgWidth = img.getwidth();
            int imgHeight = img.getheight();
            
            // 创建临时设备以获取像素数据
            HDC hdcTemp = GetImageHDC(&img);
            
            // 采样图像到网格
            for (int gridX = 0; gridX < GRID_NUM; gridX++) {
                for (int gridY = 0; gridY < GRID_NUM; gridY++) {
                    // 计算图像中的对应位置
                    int imgX = (int)(gridX * (float)imgWidth / GRID_NUM);
                    int imgY = (int)(gridY * (float)imgHeight / GRID_NUM);
                    
                    // 确保坐标在图像范围内
                    if (imgX >= 0 && imgX < imgWidth && imgY >= 0 && imgY < imgHeight) {
                        // 获取像素颜色
                        COLORREF color = GetPixel(hdcTemp, imgX, imgY);
                        
                        // 如果不是透明色，则设置到网格
                        if (color != COLOR_TRANSPARENT) {
                            gridData[gridX][gridY] = color;
                        }
                    }
                }
            }
            SetWorkingImage(NULL); // 恢复默认工作图像
            
            // 释放临时设备
            DeleteDC(hdcTemp);
            
            // 重置视图
            offsetX = 0;
            offsetY = 0;
            zoomLevel = 1;
            
            // 设置画布需要更新标志
            needCanvasUpdate = true;
        }
    }
}

// --- 清空历史记录函数 ---
// 清空撤销和重做历史
void ClearHistory() {
    undoStack.clear();
    redoStack.clear();
}

// --- 绘制单个像素函数 ---
// 在指定位置绘制单个像素
void DrawPixel(int x, int y, COLORREF color) {
    if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
        gridData[x][y] = color;
        needCanvasUpdate = true;
    }
}

// --- 绘制画笔效果函数 ---
// 根据当前画笔大小绘制像素
void DrawBrush(int gridX, int gridY, COLORREF color) {
    // 根据画笔大小绘制不同范围的像素
    for (int dx = -brushSize + 1; dx < brushSize; dx++) {
        for (int dy = -brushSize + 1; dy < brushSize; dy++) {
            int x = gridX + dx;
            int y = gridY + dy;
            
            // 检查是否在画布范围内
            if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
                // 计算到中心的距离，实现圆形画笔效果
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < brushSize - 0.5f) {
                    gridData[x][y] = color;
                }
            }
        }
    }
    
    needCanvasUpdate = true;
}

// --- 擦除画笔效果函数 ---
// 根据当前画笔大小擦除像素
void EraseBrush(int gridX, int gridY) {
    // 擦除操作就是将像素设置为透明色（或背景色）
    DrawBrush(gridX, gridY, BLACK);
}

// --- 查找按钮函数 ---
// 根据ID查找按钮指针
Button* FindButtonById(int id) {
    for (auto& btn : btns) {
        if (btn.id == id) {
            return &btn;
        }
    }
    return nullptr;
}

// --- 更新最近使用颜色函数 ---
// 将颜色添加到最近使用列表
void UpdateRecentColors(COLORREF color) {
    // 检查颜色是否已在列表中
    for (int i = 0; i < recentColorCount; i++) {
        if (recentColors[i] == color) {
            // 如果已存在，将其移到最前面
            if (i > 0) {
                for (int j = i; j > 0; j--) {
                    recentColors[j] = recentColors[j - 1];
                }
                recentColors[0] = color;
            }
            return;
        }
    }
    
    // 如果列表已满，移除最后一个
    if (recentColorCount >= 8) {
        recentColorCount = 7;
    }
    
    // 将现有颜色后移
    for (int i = recentColorCount; i > 0; i--) {
        recentColors[i] = recentColors[i - 1];
    }
    
    // 添加新颜色到最前面
    recentColors[0] = color;
    recentColorCount++;
}

// --- 应用缩放级别函数 ---
// 设置画布的缩放级别
void ApplyZoom(int level) {
    // 验证缩放级别
    if (level >= 1 && level <= 4) {
        // 计算缩放中心点（保持画布中心不变）
        int centerX = WIN_W / 2 - UI_WIDTH;
        int centerY = WIN_H / 2;
        
        // 调整偏移量以保持视觉中心不变
        float scaleRatio = (float)level / zoomLevel;
        offsetX = (int)(centerX - (centerX - offsetX) * scaleRatio);
        offsetY = (int)(centerY - (centerY - offsetY) * scaleRatio);
        
        // 应用缩放级别
        zoomLevel = level;
        
        // 设置更新标志
        needCanvasUpdate = true;
    }
}

// --- 平移画布函数 ---
// 移动画布视图
void PanCanvas(int dx, int dy) {
    offsetX += dx;
    offsetY += dy;
    
    // 限制画布移动范围，防止过多空白区域
    int maxOffsetX = (GRID_NUM * CELL_SIZE * zoomLevel - CANVAS_W) / 2 + 50;
    int maxOffsetY = (GRID_NUM * CELL_SIZE * zoomLevel - CANVAS_H) / 2 + 50;
    
    if (maxOffsetX < 0) maxOffsetX = 0;
    if (maxOffsetY < 0) maxOffsetY = 0;
    
    if (offsetX > maxOffsetX) offsetX = maxOffsetX;
    if (offsetX < -maxOffsetX) offsetX = -maxOffsetX;
    if (offsetY > maxOffsetY) offsetY = maxOffsetY;
    if (offsetY < -maxOffsetY) offsetY = -maxOffsetY;
    
    needCanvasUpdate = true;
}

// --- 绘制背景图案函数 ---
// 根据当前背景样式绘制画布背景
void DrawBackgroundPattern() {
    switch (backgroundStyle) {
        case 0: // 棋盘格背景
            // 绘制棋盘格背景
            setfillcolor(RGB(50, 50, 50));
            solidrectangle(0, 0, WIN_W, WIN_H);
            
            setfillcolor(RGB(60, 60, 60));
            for (int x = 0; x < WIN_W; x += 40) {
                for (int y = 0; y < WIN_H; y += 40) {
                    if ((x / 40 + y / 40) % 2 == 1) {
                        solidrectangle(x, y, x + 40, y + 40);
                    }
                }
            }
            break;
        
        case 1: // 浅色背景
            setfillcolor(RGB(240, 240, 240));
            solidrectangle(0, 0, WIN_W, WIN_H);
            break;
        
        case 2: // 深色背景
            setfillcolor(RGB(40, 44, 52));
            solidrectangle(0, 0, WIN_W, WIN_H);
            break;
    }
}

// --- 初始化自定义颜色函数 ---
// 设置默认的自定义颜色
void InitializeCustomColors() {
    // 初始化自定义颜色数组
    for (int i = 0; i < 16; i++) {
        customColors[i] = RGB(128, 128, 128); // 默认灰色
    }
    
    // 设置几个常用的自定义颜色
    customColors[0] = RGB(255, 0, 0);     // 红色
    customColors[1] = RGB(0, 255, 0);     // 绿色
    customColors[2] = RGB(0, 0, 255);     // 蓝色
    
    // 标记这些颜色为已使用
    customColorUsed[0] = true;
    customColorUsed[1] = true;
    customColorUsed[2] = true;
}

// --- 重置画布函数 ---
// 清空画布内容
void ResetCanvas() {
    // 清空画布数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = BLACK;
        }
    }
    
    needCanvasUpdate = true;
}

// --- 滤镜效果相关函数 ---
// 应用灰度滤镜
void ApplyGrayscaleFilter() {
    SaveHistory();
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = gridData[x][y];
            int gray = (GetRValue(color) * 30 + GetGValue(color) * 59 + GetBValue(color) * 11) / 100;
            gridData[x][y] = RGB(gray, gray, gray);
        }
    }
    
    needCanvasUpdate = true;
}

// 应用反相滤镜
void ApplyInvertFilter() {
    SaveHistory();
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = gridData[x][y];
            int r = 255 - GetRValue(color);
            int g = 255 - GetGValue(color);
            int b = 255 - GetBValue(color);
            gridData[x][y] = RGB(r, g, b);
        }
    }
    
    needCanvasUpdate = true;
}

// 应用亮度调整
void AdjustBrightness(int value) {
    SaveHistory();
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = gridData[x][y];
            int r = GetRValue(color) + value;
            int g = GetGValue(color) + value;
            int b = GetBValue(color) + value;
            
            // 确保值在有效范围内
            r = r < 0 ? 0 : (r > 255 ? 255 : r);
            g = g < 0 ? 0 : (g > 255 ? 255 : g);
            b = b < 0 ? 0 : (b > 255 ? 255 : b);
            
            gridData[x][y] = RGB(r, g, b);
        }
    }
    
    needCanvasUpdate = true;
}

// --- 图层管理相关函数 ---
// 添加新图层
void AddLayer(const wchar_t* name) {
    Layer newLayer;
    newLayer.name = name;  // 使用std::wstring的赋值运算符
    newLayer.visible = true;
    newLayer.opacity = 1.0f;
    
    // 初始化图层数据为透明
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            newLayer.data[x][y] = BLACK;
        }
    }
    
    // 添加到图层列表
    layers.push_back(newLayer);
    currentLayer = static_cast<int>(layers.size() - 1); // 显式类型转换，避免数据丢失
    
    needCanvasUpdate = true;
}

// 删除图层
void DeleteLayer(int index) {
    if (index >= 0 && index < layers.size() && layers.size() > 1) {
        layers.erase(layers.begin() + index);
        
        // 调整当前图层索引
        if (currentLayer >= static_cast<int>(layers.size())) { // 显式类型转换以避免警告
            currentLayer = layers.size() - 1;
        }
        
        needCanvasUpdate = true;
    }
}

// 切换图层可见性
void ToggleLayerVisibility(int index) {
    if (index >= 0 && index < layers.size()) {
        // 确保至少有一个可见图层
        if (!layers[index].visible) return;
        
        layers[index].visible = !layers[index].visible;
        needCanvasUpdate = true;
    }
}

// 合并可见图层
void MergeVisibleLayers() {
    if (layers.size() <= 1) return;
    
    SaveHistory();
    
    // 创建合并后的图层数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = BLACK; // 重置为背景色
            
            // 从下至上绘制可见图层
            for (size_t i = 0; i < layers.size(); i++) {
                if (layers[i].visible) {
                    COLORREF layerColor = layers[i].data[x][y];
                    if (layerColor != BLACK) { // 如果不是透明色
                        // 应用透明度
                        int opacity = layers[i].opacity;
                   // 计算透明度混合
                    if (opacity < 1.0f && gridData[x][y] != BLACK) {
                        // 混合颜色
                        int r1 = GetRValue(gridData[x][y]);
                        int g1 = GetGValue(gridData[x][y]);
                        int b1 = GetBValue(gridData[x][y]);
                        int r2 = GetRValue(layerColor);
                        int g2 = GetGValue(layerColor);
                        int b2 = GetBValue(layerColor);
                         
                        int r = static_cast<int>(r1 * (1.0f - opacity) + r2 * opacity);
                        int g = static_cast<int>(g1 * (1.0f - opacity) + g2 * opacity);
                        int b = static_cast<int>(b1 * (1.0f - opacity) + b2 * opacity);
                            
                            gridData[x][y] = RGB(r, g, b);
                        } else {
                            gridData[x][y] = layerColor;
                        }
                    }
                }
            }
        }
    }
    
    // 清空图层列表，保留一个背景层
    layers.clear();
    Layer bgLayer;
    bgLayer.name = L"背景层";  // 使用std::wstring的赋值运算符
    bgLayer.visible = true;
    bgLayer.opacity = 1.0f;
    
    // 复制合并后的数据到背景层
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            bgLayer.data[x][y] = gridData[x][y];
        }
    }
    
    layers.push_back(bgLayer);
    currentLayer = 0;
}

// --- 选区工具相关函数 ---
// 开始选择区域
void StartSelection(int x, int y) {
    selectionStart.x = x;
    selectionStart.y = y;
    selectionEnd.x = x;
    selectionEnd.y = y;
    isSelecting = true;
}

// 更新选择区域
void UpdateSelection(int x, int y) {
    if (isSelecting) {
        selectionEnd.x = x;
        selectionEnd.y = y;
    }
}

// 结束选择区域
void EndSelection() {
    isSelecting = false;
    
    // 确保选择区域有效
    if (selectionStart.x > selectionEnd.x) {
        std::swap(SelectionStart.x, SelectionEnd.x);
    }
    if (selectionStart.y > selectionEnd.y) {
        std::swap(SelectionStart.y, SelectionEnd.y);
    }
    
    // 计算网格坐标范围
    int startGridX = max(0, (selectionStart.x - UI_WIDTH - offsetX) / (CELL_SIZE * zoomLevel));
    int startGridY = max(0, (selectionStart.y - offsetY) / (CELL_SIZE * zoomLevel));
    int endGridX = min(GRID_NUM - 1, (selectionEnd.x - UI_WIDTH - offsetX) / (CELL_SIZE * zoomLevel));
    int endGridY = min(GRID_NUM - 1, (selectionEnd.y - offsetY) / (CELL_SIZE * zoomLevel));
    
    selectionGridStart.x = startGridX;
    selectionGridStart.y = startGridY;
    selectionGridEnd.x = endGridX;
    selectionGridEnd.y = endGridY;
}

// 绘制选择框
void DrawSelection() {
    if (isSelecting || hasSelection) {
        // 绘制选择框边框
        setlinecolor(RGB(0, 200, 255));
        setlinestyle(PS_DASH);
        
        int x1 = selectionStart.x;
        int y1 = selectionStart.y;
        int x2 = selectionEnd.x;
        int y2 = selectionEnd.y;
        
        rectangle(x1, y1, x2, y2);
        setlinestyle(PS_SOLID);
        
        // 绘制选择框内部的半透明覆盖
        setfillcolor(RGB(0, 200, 255, 50));
        setlinestyle(PS_NULL);
        solidrectangle(x1, y1, x2, y2);
        setlinestyle(PS_SOLID);
    }
}

// 取消选择
void ClearSelection() {
    isSelecting = false;
    hasSelection = false;
    selectionGridStart.x = 0;
    selectionGridStart.y = 0;
    selectionGridEnd.x = 0;
    selectionGridEnd.y = 0;
}

// 复制选中区域
void CopySelection() {
    if (!hasSelection) return;
    
    // 计算选择区域大小
    int width = selectionGridEnd.x - selectionGridStart.x + 1;
    int height = selectionGridEnd.y - selectionGridStart.y + 1;
    
    // 分配临时缓冲区
    if (clipboardData) {
        delete[] clipboardData;
    }
    clipboardData = new COLORREF[width * height];
    clipboardWidth = width;
    clipboardHeight = height;
    
    // 复制数据
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int gridX = selectionGridStart.x + x;
            int gridY = selectionGridStart.y + y;
            clipboardData[y * width + x] = gridData[gridX][gridY];
        }
    }
}

// 剪切选中区域
void CutSelection() {
    CopySelection();
    
    // 清除选中区域
    for (int y = selectionGridStart.y; y <= selectionGridEnd.y; y++) {
        for (int x = selectionGridStart.x; x <= selectionGridEnd.x; x++) {
            gridData[x][y] = BLACK;
        }
    }
    
    needCanvasUpdate = true;
}

// 粘贴选中区域
void PasteSelection(int targetX, int targetY) {
    if (!clipboardData) return;
    
    SaveHistory();
    
    int startGridX = max(0, targetX);
    int startGridY = max(0, targetY);
    
    // 粘贴数据，确保不超出边界
    for (int y = 0; y < clipboardHeight; y++) {
        int gridY = startGridY + y;
        if (gridY >= GRID_NUM) break;
        
        for (int x = 0; x < clipboardWidth; x++) {
            int gridX = startGridX + x;
            if (gridX >= GRID_NUM) break;
            
            gridData[gridX][gridY] = clipboardData[y * clipboardWidth + x];
        }
    }
    
    needCanvasUpdate = true;
}

// --- 几何图形绘制函数 ---
// 绘制直线
void DrawLine(int x1, int y1, int x2, int y2, COLORREF color) {
    // 简化版的 Bresenham 直线算法
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        DrawPixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// 绘制矩形
void DrawRectangle(int x1, int y1, int x2, int y2, COLORREF color, bool fill = false) {
    // 确保坐标正确
    if (x1 > x2) std::swap(x1, x2);
if (y1 > y2) std::swap(y1, y2);
    
    if (fill) {
        // 填充矩形
        for (int y = y1; y <= y2; y++) {
            for (int x = x1; x <= x2; x++) {
                DrawPixel(x, y, color);
            }
        }
    } else {
        // 绘制矩形边框
        for (int x = x1; x <= x2; x++) {
            DrawPixel(x, y1, color);
            DrawPixel(x, y2, color);
        }
        for (int y = y1 + 1; y < y2; y++) {
            DrawPixel(x1, y, color);
            DrawPixel(x2, y, color);
        }
    }
}

// 绘制圆形
void DrawCircle(int centerX, int centerY, int radius, COLORREF color, bool fill = false) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        // 绘制8个对称点
        if (fill) {
            // 填充圆形
            for (int i = -x; i <= x; i++) {
                DrawPixel(centerX + i, centerY + y, color);
                DrawPixel(centerX + i, centerY - y, color);
            }
            if (x != y) {
                for (int i = -y; i <= y; i++) {
                    DrawPixel(centerX + i, centerY + x, color);
                    DrawPixel(centerX + i, centerY - x, color);
                }
            }
        } else {
            // 绘制圆形边框
            DrawPixel(centerX + x, centerY + y, color);
            DrawPixel(centerX - x, centerY + y, color);
            DrawPixel(centerX + x, centerY - y, color);
            DrawPixel(centerX - x, centerY - y, color);
            DrawPixel(centerX + y, centerY + x, color);
            DrawPixel(centerX - y, centerY + x, color);
            DrawPixel(centerX + y, centerY - x, color);
            DrawPixel(centerX - y, centerY - x, color);
        }
        
        x++;
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
    }
}

// --- 辅助功能函数 ---
// 导出为指定格式
void ExportArtwork(const wchar_t* format) {
    // 创建一个临时图像用于导出
    IMAGE img(GRID_NUM * CELL_SIZE, GRID_NUM * CELL_SIZE);
    SetWorkingImage(&img);
    
    // 绘制背景
    setfillcolor(BLACK);
    solidrectangle(0, 0, GRID_NUM * CELL_SIZE, GRID_NUM * CELL_SIZE);
    
    // 绘制像素数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = gridData[x][y];
            if (color != BLACK) { // 跳过透明像素
                setfillcolor(color);
                solidrectangle(x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
            }
        }
    }
    
    // 恢复工作图像
    SetWorkingImage();
    
    // 打开保存对话框
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = TEXT("");
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = format;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn) == TRUE) {
        // 保存图像
        saveimage(szFile, &img);
    }
}

// 保存项目配置
void SaveSettings() {
    // 在实际应用中，这里应该将设置保存到文件
    // 目前仅作为功能占位符
}

// 加载项目配置
void LoadSettings() {
    // 在实际应用中，这里应该从文件加载设置
    // 目前仅作为功能占位符
    
    // 初始化默认设置
    backgroundStyle = 0;
    showGrid = true;
    brushSize = 1;
    currentTool = TOOL_PEN;
}

// 显示帮助信息
void ShowHelp() {
    // 在实际应用中，这里应该显示详细的帮助信息
    // 目前仅作为功能占位符
}

// 显示关于对话框
void ShowAbout() {
    // 在实际应用中，这里应该显示关于对话框
    // 目前仅作为功能占位符
}

// --- 动画帧管理相关函数 ---
// 添加新动画帧
void AddAnimationFrame() {
    // 创建当前画布的副本作为新帧
    AnimationFrame newFrame;
    newFrame.duration = 100; // 默认持续时间100ms
    
    // 复制当前画布数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            newFrame.data[x][y] = gridData[x][y];
        }
    }
    
    // 添加到帧列表
    animationFrames.push_back(newFrame);
    currentFrameIndex = animationFrames.size() - 1;
    
    // 显示帧添加成功提示
    wchar_t msg[100];
    swprintf(msg, 100, L"添加新帧成功，当前帧: %d/%d", currentFrameIndex + 1, animationFrames.size());
    // 在实际应用中，这里应该有一个消息显示机制
}

// 删除当前帧
void DeleteAnimationFrame() {
    if (animationFrames.size() <= 1) return; // 保留至少一帧
    
    // 删除当前帧
    animationFrames.erase(animationFrames.begin() + currentFrameIndex);
    
    // 调整当前帧索引
    if (currentFrameIndex >= animationFrames.size()) {
        currentFrameIndex = animationFrames.size() - 1;
    }
    
    // 加载当前帧内容到画布
    LoadAnimationFrame(currentFrameIndex);
}

// 加载指定动画帧
void LoadAnimationFrame(int index) {
    if (index < 0 || index >= animationFrames.size()) return;
    
    // 保存当前画布状态到历史
    SaveHistory();
    
    // 复制帧数据到画布
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = animationFrames[index].data[x][y];
        }
    }
    
    // 更新当前帧索引
    currentFrameIndex = index;
    needCanvasUpdate = true;
    
    // 显示当前帧信息
    wchar_t msg[100];
    swprintf(msg, 100, L"当前帧: %d/%d", currentFrameIndex + 1, animationFrames.size());
    // 在实际应用中，这里应该有一个消息显示机制
}

// 设置帧持续时间
void SetFrameDuration(int duration) {
    if (currentFrameIndex >= 0 && currentFrameIndex < animationFrames.size()) {
        animationFrames[currentFrameIndex].duration = duration;
    }
}

// 导出动画为GIF
void ExportAnimationToGIF() {
    // 在实际应用中，这里应该实现GIF导出功能
    // 目前仅作为功能占位符
    // 导出动画为GIF需要使用专门的GIF编码库
}

// 播放/预览动画
void PlayAnimationPreview() {
    // 在实际应用中，这里应该实现动画预览功能
    // 目前仅作为功能占位符
    
    // 预览模式标志
    isPreviewing = true;
    previewFrameIndex = 0;
    previewStartTime = GetTickCount();
    
    // 预览循环
    while (isPreviewing) {
        // 计算当前应该显示的帧
        DWORD currentTime = GetTickCount();
        DWORD elapsedTime = currentTime - previewStartTime;
        
        // 计算经过的时间总和，找到对应的帧
        DWORD totalDuration = 0;
        for (int i = 0; i < animationFrames.size(); i++) {
            totalDuration += animationFrames[i].duration;
            if (elapsedTime < totalDuration) {
                if (i != previewFrameIndex) {
                    previewFrameIndex = i;
                    // 加载当前预览帧
                    for (int x = 0; x < GRID_NUM; x++) {
                        for (int y = 0; y < GRID_NUM; y++) {
                            gridData[x][y] = animationFrames[i].data[x][y];
                        }
                    }
                    needCanvasUpdate = true;
                }
                break;
            }
        }
        
        // 如果播放完一轮，重置
        if (elapsedTime >= totalDuration) {
            previewStartTime = currentTime;
            previewFrameIndex = 0;
            // 加载第一帧
            for (int x = 0; x < GRID_NUM; x++) {
                for (int y = 0; y < GRID_NUM; y++) {
                    gridData[x][y] = animationFrames[0].data[x][y];
                }
            }
            needCanvasUpdate = true;
        }
        
        // 绘制当前预览帧
        BeginBatchDraw();
        cleardevice();
        DrawBackgroundPattern();
        UpdateCanvas();
        DrawInterface();
        EndBatchDraw();
        
        // 短暂休眠，避免CPU占用过高
        Sleep(16);
        
        // 检查用户是否停止预览
        // 这里需要一个机制来检测用户输入
    }
}

// --- 自定义画笔相关函数 ---
// 创建自定义画笔形状
void CreateCustomBrush(int shapeId) {
    // 清空当前自定义画笔数据
    memset(customBrushData, 0, sizeof(customBrushData));
    
    // 根据形状ID创建不同的画笔形状
    switch (shapeId) {
        case BRUSH_CIRCLE:
            // 创建圆形画笔
            for (int x = 0; x < BRUSH_SIZE; x++) {
                for (int y = 0; y < BRUSH_SIZE; y++) {
                    float dx = x - BRUSH_SIZE / 2 + 0.5f;
                    float dy = y - BRUSH_SIZE / 2 + 0.5f;
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance < BRUSH_SIZE / 2) {
                        customBrushData[x][y] = 1;
                    }
                }
            }
            break;
            
        case BRUSH_SQUARE:
            // 创建方形画笔
            for (int x = 0; x < BRUSH_SIZE; x++) {
                for (int y = 0; y < BRUSH_SIZE; y++) {
                    customBrushData[x][y] = 1;
                }
            }
            break;
            
        case BRUSH_DIAMOND:
            // 创建菱形画笔
            for (int x = 0; x < BRUSH_SIZE; x++) {
                for (int y = 0; y < BRUSH_SIZE; y++) {
                    float dx = abs(x - BRUSH_SIZE / 2 + 0.5f);
                    float dy = abs(y - BRUSH_SIZE / 2 + 0.5f);
                    if (dx + dy < BRUSH_SIZE / 2) {
                        customBrushData[x][y] = 1;
                    }
                }
            }
            break;
            
        case BRUSH_STAR:
            // 创建星形画笔（简化版）
            for (int x = 0; x < BRUSH_SIZE; x++) {
                for (int y = 0; y < BRUSH_SIZE; y++) {
                    float dx = x - BRUSH_SIZE / 2 + 0.5f;
                    float dy = y - BRUSH_SIZE / 2 + 0.5f;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    // 简化的星形判断
                    if (distance < BRUSH_SIZE / 4 || 
                        (abs(dx) < BRUSH_SIZE / 8 && distance < BRUSH_SIZE / 2) || 
                        (abs(dy) < BRUSH_SIZE / 8 && distance < BRUSH_SIZE / 2)) {
                        customBrushData[x][y] = 1;
                    }
                }
            }
            break;
    }
}

// 使用自定义画笔绘制
void UseCustomBrush(int gridX, int gridY, COLORREF color) {
    // 计算画笔中心点
    int center = BRUSH_SIZE / 2;
    
    // 使用自定义画笔形状绘制
    for (int x = 0; x < BRUSH_SIZE; x++) {
        for (int y = 0; y < BRUSH_SIZE; y++) {
            if (customBrushData[x][y]) {
                int targetX = gridX + x - center;
                int targetY = gridY + y - center;
                
                // 检查是否在画布范围内
                if (targetX >= 0 && targetX < GRID_NUM && targetY >= 0 && targetY < GRID_NUM) {
                    gridData[targetX][targetY] = color;
                }
            }
        }
    }
    
    needCanvasUpdate = true;
}

// --- 渐变填充相关函数 ---
// 线性渐变填充
void LinearGradientFill(int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2) {
    SaveHistory();
    
    // 计算向量长度
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length == 0) return;
    
    // 单位向量
    float unitX = dx / length;
    float unitY = dy / length;
    
    // 对画布中的每个像素计算渐变颜色
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            // 计算点到起点的向量
            float pointX = x - x1;
            float pointY = y - y1;
            
            // 计算投影长度（点积）
            float projection = pointX * unitX + pointY * unitY;
            
            // 确保投影在0到长度之间
            float ratio = max(0.0f, min(1.0f, projection / length));
            
            // 计算插值颜色
            int r1 = GetRValue(color1);
            int g1 = GetGValue(color1);
            int b1 = GetBValue(color1);
            
            int r2 = GetRValue(color2);
            int g2 = GetGValue(color2);
            int b2 = GetBValue(color2);
            
            int r = (int)(r1 + (r2 - r1) * ratio);
            int g = (int)(g1 + (g2 - g1) * ratio);
            int b = (int)(b1 + (b2 - b1) * ratio);
            
            gridData[x][y] = RGB(r, g, b);
        }
    }
    
    needCanvasUpdate = true;
}

// 径向渐变填充
void RadialGradientFill(int centerX, int centerY, int radius, COLORREF color1, COLORREF color2) {
    SaveHistory();
    
    if (radius <= 0) return;
    
    // 对画布中的每个像素计算渐变颜色
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            // 计算到中心点的距离
            float dx = x - centerX;
            float dy = y - centerY;
            float distance = sqrt(dx * dx + dy * dy);
            
            // 计算距离比例
            float ratio = min(1.0f, distance / radius);
            
            // 计算插值颜色
            int r1 = GetRValue(color1);
            int g1 = GetGValue(color1);
            int b1 = GetBValue(color1);
            
            int r2 = GetRValue(color2);
            int g2 = GetGValue(color2);
            int b2 = GetBValue(color2);
            
            int r = (int)(r1 + (r2 - r1) * ratio);
            int g = (int)(g1 + (g2 - g1) * ratio);
            int b = (int)(b1 + (b2 - b1) * ratio);
            
            gridData[x][y] = RGB(r, g, b);
        }
    }
    
    needCanvasUpdate = true;
}

// --- 像素艺术特效相关函数 ---
// 应用像素化效果
void ApplyPixelateEffect(int pixelSize) {
    SaveHistory();
    
    // 创建临时缓冲区保存原始数据
    COLORREF tempData[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[x][y] = gridData[x][y];
        }
    }
    
    // 应用像素化效果
    for (int x = 0; x < GRID_NUM; x += pixelSize) {
        for (int y = 0; y < GRID_NUM; y += pixelSize) {
            // 计算该块的平均颜色
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;
            
            for (int dx = 0; dx < pixelSize; dx++) {
                for (int dy = 0; dy < pixelSize; dy++) {
                    int px = x + dx;
                    int py = y + dy;
                    
                    if (px < GRID_NUM && py < GRID_NUM) {
                        COLORREF color = tempData[px][py];
                        rSum += GetRValue(color);
                        gSum += GetGValue(color);
                        bSum += GetBValue(color);
                        count++;
                    }
                }
            }
            
            // 计算平均颜色
            if (count > 0) {
                int r = rSum / count;
                int g = gSum / count;
                int b = bSum / count;
                COLORREF avgColor = RGB(r, g, b);
                
                // 填充整个块
                for (int dx = 0; dx < pixelSize; dx++) {
                    for (int dy = 0; dy < pixelSize; dy++) {
                        int px = x + dx;
                        int py = y + dy;
                        
                        if (px < GRID_NUM && py < GRID_NUM) {
                            gridData[px][py] = avgColor;
                        }
                    }
                }
            }
        }
    }
    
    needCanvasUpdate = true;
}

// 应用马赛克效果
void ApplyMosaicEffect(int tileSize) {
    SaveHistory();
    
    // 创建临时缓冲区保存原始数据
    COLORREF tempData[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[x][y] = gridData[x][y];
        }
    }
    
    // 应用马赛克效果（与像素化类似，但使用随机块）
    for (int x = 0; x < GRID_NUM; x += tileSize) {
        for (int y = 0; y < GRID_NUM; y += tileSize) {
            // 为每个块随机选择一个像素颜色
            int randX = x + rand() % tileSize;
            int randY = y + rand() % tileSize;
            
            // 确保随机位置在边界内
            if (randX >= GRID_NUM) randX = GRID_NUM - 1;
            if (randY >= GRID_NUM) randY = GRID_NUM - 1;
            
            COLORREF tileColor = tempData[randX][randY];
            
            // 填充整个块
            for (int dx = 0; dx < tileSize; dx++) {
                for (int dy = 0; dy < tileSize; dy++) {
                    int px = x + dx;
                    int py = y + dy;
                    
                    if (px < GRID_NUM && py < GRID_NUM) {
                        gridData[px][py] = tileColor;
                    }
                }
            }
        }
    }
    
    needCanvasUpdate = true;
}

// --- 图像处理算法相关函数 ---
// 应用模糊滤镜
void ApplyBlurFilter(int radius) {
    SaveHistory();
    
    // 创建临时缓冲区保存原始数据
    COLORREF tempData[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[x][y] = gridData[x][y];
        }
    }
    
    // 应用盒式模糊
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;
            
            // 计算邻域内的颜色平均值
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius; dy <= radius; dy++) {
                    int px = x + dx;
                    int py = y + dy;
                    
                    if (px >= 0 && px < GRID_NUM && py >= 0 && py < GRID_NUM) {
                        COLORREF color = tempData[px][py];
                        rSum += GetRValue(color);
                        gSum += GetGValue(color);
                        bSum += GetBValue(color);
                        count++;
                    }
                }
            }
            
            // 计算平均颜色
            if (count > 0) {
                int r = rSum / count;
                int g = gSum / count;
                int b = bSum / count;
                gridData[x][y] = RGB(r, g, b);
            }
        }
    }
    
    needCanvasUpdate = true;
}

// 应用锐化滤镜
void ApplySharpenFilter() {
    SaveHistory();
    
    // 创建临时缓冲区保存原始数据
    COLORREF tempData[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[x][y] = gridData[x][y];
        }
    }
    
    // 锐化卷积核：
    // 0 -1 0
    //-1 5 -1
    // 0 -1 0
    const int kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    
    // 应用锐化滤镜
    for (int x = 1; x < GRID_NUM - 1; x++) {
        for (int y = 1; y < GRID_NUM - 1; y++) {
            int rSum = 0, gSum = 0, bSum = 0;
            
            // 应用卷积核
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int px = x + dx;
                    int py = y + dy;
                    int weight = kernel[dx + 1][dy + 1];
                    
                    COLORREF color = tempData[px][py];
                    rSum += GetRValue(color) * weight;
                    gSum += GetGValue(color) * weight;
                    bSum += GetBValue(color) * weight;
                }
            }
            
            // 确保值在有效范围内
            int r = max(0, min(255, rSum));
            int g = max(0, min(255, gSum));
            int b = max(0, min(255, bSum));
            
            gridData[x][y] = RGB(r, g, b);
        }
    }
    
    needCanvasUpdate = true;
}

// --- 辅助算法函数 ---
// 计算颜色相似度
int CalculateColorSimilarity(COLORREF color1, COLORREF color2) {
    int r1 = GetRValue(color1);
    int g1 = GetGValue(color1);
    int b1 = GetBValue(color1);
    
    int r2 = GetRValue(color2);
    int g2 = GetGValue(color2);
    int b2 = GetBValue(color2);
    
    // 计算RGB差值的平方和
    int dr = r1 - r2;
    int dg = g1 - g2;
    int db = b1 - b2;
    
    return dr * dr + dg * dg + db * db;
}

// 找到最接近的调色板颜色
COLORREF FindClosestPaletteColor(COLORREF color) {
    int minDistance = INT_MAX;
    COLORREF closestColor = color;
    
    for (int i = 0; i < 16; i++) {
        int distance = CalculateColorSimilarity(color, palette[i]);
        if (distance < minDistance) {
            minDistance = distance;
            closestColor = palette[i];
        }
    }
    
    return closestColor;
}

// 限制颜色到调色板
void LimitToPalette() {
    SaveHistory();
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = FindClosestPaletteColor(gridData[x][y]);
        }
    }
    
    needCanvasUpdate = true;
}

// --- 高级编辑功能函数 ---
// 翻转画布（水平）
void FlipCanvasHorizontal() {
    SaveHistory();
    
    for (int y = 0; y < GRID_NUM; y++) {
        for (int x = 0; x < GRID_NUM / 2; x++) {
            std::swap(gridData[x][y], gridData[GRID_NUM - 1 - x][y]);
        }
    }
    
    needCanvasUpdate = true;
}

// 翻转画布（垂直）
void FlipCanvasVertical() {
    SaveHistory();
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM / 2; y++) {
            std::swap(gridData[x][y], gridData[x][GRID_NUM - 1 - y]);
        }
    }
    
    needCanvasUpdate = true;
}

// 旋转画布（90度顺时针）
void RotateCanvasClockwise() {
    SaveHistory();
    
    // 创建临时缓冲区
    COLORREF tempData[GRID_NUM][GRID_NUM];
    
    // 执行旋转
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[y][GRID_NUM - 1 - x] = gridData[x][y];
        }
    }
    
    // 复制回原数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = tempData[x][y];
        }
    }
    
    needCanvasUpdate = true;
}

// 旋转画布（90度逆时针）
void RotateCanvasCounterclockwise() {
    SaveHistory();
    
    // 创建临时缓冲区
    COLORREF tempData[GRID_NUM][GRID_NUM];
    
    // 执行旋转
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[GRID_NUM - 1 - y][x] = gridData[x][y];
        }
    }
    
    // 复制回原数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            gridData[x][y] = tempData[x][y];
        }
    }
    
    needCanvasUpdate = true;
}

// --- 批量处理函数 ---
// 批量应用效果到所有动画帧
void ApplyEffectToAllFrames(void (*effectFunction)()) {
    // 保存当前帧索引
    int currentIndex = currentFrameIndex;
    
    // 对每个帧应用效果
    for (int i = 0; i < animationFrames.size(); i++) {
        // 加载当前帧
        LoadAnimationFrame(i);
        
        // 应用效果
        effectFunction();
        
        // 保存回帧数据
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                animationFrames[i].data[x][y] = gridData[x][y];
            }
        }
    }
    
    // 恢复原来的帧
    LoadAnimationFrame(currentIndex);
}

// --- 性能优化相关函数 ---
// 优化画布渲染
void OptimizeCanvasRendering() {
    // 在实际应用中，这里应该实现渲染优化逻辑
    // 例如只重绘修改的区域，使用缓存等
    
    // 标记需要完整更新
    needCanvasUpdate = true;
}

// 计算画布复杂度
int CalculateCanvasComplexity() {
    // 统计不同颜色的数量作为复杂度指标
    unordered_set<COLORREF> colorSet;
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            if (gridData[x][y] != BLACK) { // 不计算透明色
                colorSet.insert(gridData[x][y]);
            }
        }
    }
    
    return colorSet.size();
}

// --- 图层管理增强功能 ---
// 复制当前图层
void DuplicateCurrentLayer() {
    if (currentLayer < 0 || currentLayer >= layers.size()) return;
    
    // 创建图层副本
    Layer newLayer = layers[currentLayer];
    newLayer.name = newLayer.name + L"_副本";
    
    // 复制图层数据
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            newLayer.data[x][y] = layers[currentLayer].data[x][y];
        }
    }
    
    // 插入到当前图层之后
    layers.insert(layers.begin() + currentLayer + 1, newLayer);
    currentLayer++;
    
    // 更新图层显示
    UpdateLayersVisibility();
    
    // 显示提示消息
    wchar_t msg[100];
    swprintf(msg, 100, L"图层 '%ls' 已复制", newLayer.name.c_str());
    // 在实际应用中，这里应该有一个消息显示机制
}

// 重命名图层
void RenameLayer(int layerIndex, const wstring& newName) {
    if (layerIndex < 0 || layerIndex >= layers.size()) return;
    
    layers[layerIndex].name = newName;
    
    // 显示提示消息
    wchar_t msg[100];
    swprintf(msg, 100, L"图层已重命名为 '%ls'", newName.c_str());
    // 在实际应用中，这里应该有一个消息显示机制
}

// 调整图层透明度
void SetLayerOpacity(int layerIndex, float opacity) {
    if (layerIndex < 0 || layerIndex >= layers.size()) return;
    
    // 限制透明度在0-1之间
    opacity = max(0.0f, min(1.0f, opacity));
    layers[layerIndex].opacity = opacity;
    
    // 更新图层显示
    UpdateLayersVisibility();
}

// 移动图层（上移/下移）
void MoveLayer(int layerIndex, bool moveUp) {
    if (layerIndex < 0 || layerIndex >= layers.size()) return;
    
    int targetIndex = moveUp ? layerIndex - 1 : layerIndex + 1;
    
    // 检查目标位置是否有效
    if (targetIndex < 0 || targetIndex >= layers.size()) return;
    
    // 交换图层位置
    std::swap(layers[layerIndex], layers[targetIndex]);
    
    // 如果移动的是当前图层，更新当前图层索引
    if (layerIndex == currentLayer) {
        currentLayer = targetIndex;
    } else if (targetIndex == currentLayer) {
        currentLayer = layerIndex;
    }
    
    // 更新图层显示
    UpdateLayersVisibility();
}

// 合并选中的图层
void MergeSelectedLayers(const vector<int>& layerIndices) {
    if (layerIndices.size() < 2) return;
    
    // 创建新的合并图层
    Layer mergedLayer;
    mergedLayer.name = L"合并图层";
    mergedLayer.visible = true;
    mergedLayer.opacity = 1.0f;
    
    // 初始化合并图层为透明色
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            mergedLayer.data[x][y] = BLACK;
        }
    }
    
    // 按顺序合并图层
    for (int index : layerIndices) {
        if (index < 0 || index >= layers.size()) continue;
        
        const Layer& layer = layers[index];
        if (!layer.visible) continue;
        
        // 应用图层到合并图层
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                COLORREF layerColor = layer.data[x][y];
                if (layerColor != BLACK) { // 只合并非透明像素
                    // 应用透明度
                    int r = GetRValue(layerColor);
                    int g = GetGValue(layerColor);
                    int b = GetBValue(layerColor);
                    
                    // 简单的覆盖模式
                    mergedLayer.data[x][y] = layerColor;
                }
            }
        }
    }
    
    // 删除已合并的图层
    sort(layerIndices.rbegin(), layerIndices.rend()); // 从后往前删除
    for (int index : layerIndices) {
        if (index < 0 || index >= layers.size()) continue;
        layers.erase(layers.begin() + index);
    }
    
    // 添加合并后的图层
    layers.insert(layers.begin(), mergedLayer);
    currentLayer = 0;
    
    // 更新图层显示
    UpdateLayersVisibility();
}

// --- 文字工具相关功能 ---
// 向画布添加文字
void AddTextToCanvas(int x, int y, const wstring& text, COLORREF color, int fontSize) {
    SaveHistory();
    
    // 在实际应用中，这里应该使用图形库的文字绘制功能
    // 目前仅作为功能占位符
    
    // 模拟文字绘制（将文字绘制到画布上的指定位置）
    // 这里简化处理，实际实现需要考虑字体、间距等
    
    // 显示提示消息
    wchar_t msg[200];
    swprintf(msg, 100, L"文字 '%ls' 已添加到位置 (%d,%d)", text.c_str(), x, y);
    // 在实际应用中，这里应该有一个消息显示机制
    
    needCanvasUpdate = true;
}

// 创建文本样式
TextStyle CreateTextStyle(COLORREF color, int fontSize, bool bold, bool italic, bool underline) {
    TextStyle style;
    style.color = color;
    style.fontSize = fontSize;
    style.bold = bold;
    style.italic = italic;
    style.underline = underline;
    
    // 在实际应用中，这里应该初始化字体对象
    return style;
}

// 更新文本样式
void UpdateTextStyle(TextStyle& style, COLORREF color, int fontSize, bool bold, bool italic, bool underline) {
    style.color = color;
    style.fontSize = fontSize;
    style.bold = bold;
    style.italic = italic;
    style.underline = underline;
    
    // 在实际应用中，这里应该更新字体对象
}

// --- 快捷键系统 ---
// 处理键盘快捷键
bool ProcessKeyboardShortcut(WPARAM wParam, LPARAM lParam) {
    bool handled = false;
    
    // 检查Ctrl键修饰符
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;
    
    // 根据按键组合执行相应功能
    switch (wParam) {
        case 'Z':
            if (ctrlPressed && !shiftPressed) {
                Undo();
                handled = true;
            }
            else if (ctrlPressed && shiftPressed) {
                Redo();
                handled = true;
            }
            break;
            
        case 'X':
            if (ctrlPressed) {
                // 剪切功能
                CopySelection();
                ClearSelection();
                handled = true;
            }
            break;
            
        case 'C':
            if (ctrlPressed) {
                CopySelection();
                handled = true;
            }
            break;
            
        case 'V':
            if (ctrlPressed) {
                PasteSelection();
                handled = true;
            }
            break;
            
        case 'A':
            if (ctrlPressed) {
                SelectAll();
                handled = true;
            }
            break;
            
        case 'S':
            if (ctrlPressed) {
                SaveProject();
                handled = true;
            }
            break;
            
        case 'O':
            if (ctrlPressed) {
                OpenProject();
                handled = true;
            }
            break;
            
        case 'N':
            if (ctrlPressed) {
                NewProject();
                handled = true;
            }
            break;
            
        case VK_DELETE:
            if (hasActiveSelection) {
                ClearSelection();
                handled = true;
            }
            break;
            
        case VK_ADD:
        case '+':
            if (ctrlPressed) {
                ZoomIn();
                handled = true;
            }
            break;
            
        case VK_SUBTRACT:
        case '-':
            if (ctrlPressed) {
                ZoomOut();
                handled = true;
            }
            break;
            
        case '0':
            if (ctrlPressed) {
                ResetZoom();
                handled = true;
            }
            break;
            
        // 工具快捷键
        case 'B':
            if (!ctrlPressed) {
                currentTool = TOOL_BRUSH;
                handled = true;
            }
            break;
            
        case 'E':
            if (!ctrlPressed) {
                currentTool = TOOL_ERASER;
                handled = true;
            }
            break;
            
        case 'F':
            if (!ctrlPressed) {
                currentTool = TOOL_FILL;
                handled = true;
            }
            break;
            
        case 'P':
            if (!ctrlPressed) {
                currentTool = TOOL_PICKER;
                handled = true;
            }
            break;
            
        case 'R':
            if (!ctrlPressed) {
                currentTool = TOOL_RECTANGLE;
                handled = true;
            }
            break;
            
        case 'L':
            if (!ctrlPressed) {
                currentTool = TOOL_LINE;
                handled = true;
            }
            break;
            
        case 'C':
            if (!ctrlPressed && !ctrlPressed) {
                currentTool = TOOL_CIRCLE;
                handled = true;
            }
            break;
            
        case 'S':
            if (!ctrlPressed) {
                currentTool = TOOL_SELECT;
                handled = true;
            }
            break;
    }
    
    return handled;
}

// 注册自定义快捷键
void RegisterCustomShortcut(int key, int modifiers, void (*callback)()) {
    Shortcut shortcut;
    shortcut.key = key;
    shortcut.modifiers = modifiers;
    shortcut.callback = callback;
    
    customShortcuts.push_back(shortcut);
}

// 处理自定义快捷键
void ProcessCustomShortcuts(WPARAM wParam, LPARAM lParam) {
    int modifiers = 0;
    
    if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) modifiers |= MOD_CONTROL;
    if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) modifiers |= MOD_SHIFT;
    if ((GetKeyState(VK_MENU) & 0x8000) != 0) modifiers |= MOD_ALT;
    
    for (const Shortcut& shortcut : customShortcuts) {
        if (shortcut.key == wParam && shortcut.modifiers == modifiers) {
            if (shortcut.callback != nullptr) {
                shortcut.callback();
            }
            break;
        }
    }
}

// --- 设置管理器 ---
// 初始化设置
void InitializeSettings() {
    // 设置默认值
    settings.gridVisible = true;
    settings.autoSaveEnabled = false;
    settings.autoSaveInterval = 5; // 5分钟
    settings.showToolTips = true;
    settings.antiAliasEnabled = true;
    settings.defaultBrushSize = 1;
    settings.defaultLayerCount = 1;
    settings.recentColorsCount = 8;
    settings.maxUndoSteps = 50;
    settings.bgPatternType = PATTERN_GRID;
    
    // 加载保存的设置
    LoadSettings();
}

// 保存设置到文件
void SaveSettings() {
    // 在实际应用中，这里应该将设置保存到配置文件
    // 目前仅作为功能占位符
    
    // 显示保存成功提示
    // 在实际应用中，这里应该有一个消息显示机制
}

// 从文件加载设置
void LoadSettings() {
    // 在实际应用中，这里应该从配置文件加载设置
    // 如果文件不存在，保持默认值
    
    // 这里可以添加读取配置文件的代码
}

// 更新设置
void UpdateSetting(const wstring& settingName, const SettingValue& value) {
    // 根据设置名称更新相应的设置值
    if (settingName == L"gridVisible") {
        settings.gridVisible = value.boolValue;
        needCanvasUpdate = true;
    }
    else if (settingName == L"autoSaveEnabled") {
        settings.autoSaveEnabled = value.boolValue;
        // 更新自动保存定时器
    }
    else if (settingName == L"autoSaveInterval") {
        settings.autoSaveInterval = value.intValue;
        // 更新自动保存定时器
    }
    else if (settingName == L"showToolTips") {
        settings.showToolTips = value.boolValue;
    }
    else if (settingName == L"antiAliasEnabled") {
        settings.antiAliasEnabled = value.boolValue;
        needCanvasUpdate = true;
    }
    else if (settingName == L"defaultBrushSize") {
        settings.defaultBrushSize = value.intValue;
    }
    else if (settingName == L"defaultLayerCount") {
        settings.defaultLayerCount = value.intValue;
    }
    else if (settingName == L"recentColorsCount") {
        settings.recentColorsCount = value.intValue;
        // 调整最近颜色数组大小
        recentColors.resize(settings.recentColorsCount);
    }
    else if (settingName == L"maxUndoSteps") {
        settings.maxUndoSteps = value.intValue;
        // 调整历史记录大小
        while (history.size() > settings.maxUndoSteps) {
            history.pop_front();
        }
    }
    else if (settingName == L"bgPatternType") {
        settings.bgPatternType = value.intValue;
        needCanvasUpdate = true;
    }
    
    // 保存更新后的设置
    SaveSettings();
}

// 获取设置值
SettingValue GetSetting(const wstring& settingName) {
    SettingValue value;
    
    if (settingName == L"gridVisible") {
        value.boolValue = settings.gridVisible;
        value.type = SETTING_TYPE_BOOL;
    }
    else if (settingName == L"autoSaveEnabled") {
        value.boolValue = settings.autoSaveEnabled;
        value.type = SETTING_TYPE_BOOL;
    }
    else if (settingName == L"autoSaveInterval") {
        value.intValue = settings.autoSaveInterval;
        value.type = SETTING_TYPE_INT;
    }
    else if (settingName == L"showToolTips") {
        value.boolValue = settings.showToolTips;
        value.type = SETTING_TYPE_BOOL;
    }
    else if (settingName == L"antiAliasEnabled") {
        value.boolValue = settings.antiAliasEnabled;
        value.type = SETTING_TYPE_BOOL;
    }
    else if (settingName == L"defaultBrushSize") {
        value.intValue = settings.defaultBrushSize;
        value.type = SETTING_TYPE_INT;
    }
    else if (settingName == L"defaultLayerCount") {
        value.intValue = settings.defaultLayerCount;
        value.type = SETTING_TYPE_INT;
    }
    else if (settingName == L"recentColorsCount") {
        value.intValue = settings.recentColorsCount;
        value.type = SETTING_TYPE_INT;
    }
    else if (settingName == L"maxUndoSteps") {
        value.intValue = settings.maxUndoSteps;
        value.type = SETTING_TYPE_INT;
    }
    else if (settingName == L"bgPatternType") {
        value.intValue = settings.bgPatternType;
        value.type = SETTING_TYPE_INT;
    }
    
    return value;
}

// --- 图案填充工具 ---
// 创建简单的图案
void CreateSimplePattern(int patternId) {
    // 清空当前图案数据
    memset(patternData, 0, sizeof(patternData));
    
    switch (patternId) {
        case PATTERN_DOTS:
            // 创建点状图案
            for (int x = 0; x < PATTERN_SIZE; x += 3) {
                for (int y = 0; y < PATTERN_SIZE; y += 3) {
                    patternData[x][y] = 1;
                }
            }
            break;
            
        case PATTERN_STRIPES_HORIZONTAL:
            // 创建水平条纹
            for (int y = 0; y < PATTERN_SIZE; y += 4) {
                for (int x = 0; x < PATTERN_SIZE; x++) {
                    patternData[x][y] = 1;
                    if (y + 1 < PATTERN_SIZE) patternData[x][y + 1] = 1;
                }
            }
            break;
            
        case PATTERN_STRIPES_VERTICAL:
            // 创建垂直条纹
            for (int x = 0; x < PATTERN_SIZE; x += 4) {
                for (int y = 0; y < PATTERN_SIZE; y++) {
                    patternData[x][y] = 1;
                    if (x + 1 < PATTERN_SIZE) patternData[x + 1][y] = 1;
                }
            }
            break;
            
        case PATTERN_CHECKERBOARD:
            // 创建棋盘格图案
            for (int x = 0; x < PATTERN_SIZE; x++) {
                for (int y = 0; y < PATTERN_SIZE; y++) {
                    if ((x / 4 + y / 4) % 2 == 0) {
                        patternData[x][y] = 1;
                    }
                }
            }
            break;
            
        case PATTERN_DIAGONAL:
            // 创建对角线图案
            for (int x = 0; x < PATTERN_SIZE; x++) {
                for (int y = 0; y < PATTERN_SIZE; y++) {
                    if ((x + y) % 6 < 2) {
                        patternData[x][y] = 1;
                    }
                }
            }
            break;
    }
}

// 用图案填充区域
void FillWithPattern(int startX, int startY, COLORREF color) {
    SaveHistory();
    
    // 获取起始点颜色
    COLORREF targetColor = gridData[startX][startY];
    if (targetColor == color) return; // 颜色相同，不需要填充
    
    // 使用队列进行广度优先搜索填充
    queue<pair<int, int>> q;
    q.push({startX, startY});
    
    // 标记已访问的点
    bool visited[GRID_NUM][GRID_NUM] = {false};
    visited[startX][startY] = true;
    
    // 填充区域
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        
        // 使用图案填充当前点
        int patternX = x % PATTERN_SIZE;
        int patternY = y % PATTERN_SIZE;
        if (patternData[patternX][patternY]) {
            gridData[x][y] = color;
        } else {
            gridData[x][y] = BLACK; // 保持透明
        }
        
        // 检查四个方向的相邻点
        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};
        
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            // 检查边界和颜色
            if (nx >= 0 && nx < GRID_NUM && ny >= 0 && ny < GRID_NUM &&
                !visited[nx][ny] && gridData[nx][ny] == targetColor) {
                
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }
    
    needCanvasUpdate = true;
}

// --- 批量转换功能 ---
// 将图像转换为像素艺术风格
void ConvertToPixelArt(int pixelSize) {
    SaveHistory();
    
    // 创建临时缓冲区保存原始数据
    COLORREF tempData[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            tempData[x][y] = gridData[x][y];
        }
    }
    
    // 应用像素化
    ApplyPixelateEffect(pixelSize);
    
    // 限制到调色板
    LimitToPalette();
    
    // 锐化边缘
    ApplySharpenFilter();
    
    needCanvasUpdate = true;
}

// --- 统计分析功能 ---
// 分析画布颜色分布
ColorAnalysisResult AnalyzeColorDistribution() {
    ColorAnalysisResult result;
    
    // 初始化统计数据
    result.totalPixels = 0;
    result.transparentPixels = 0;
    
    // 统计每种颜色的出现次数
    std::map<long, int> colorCounts; // 使用long作为键类型，兼容COLORREF
    
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = gridData[x][y];
            result.totalPixels++;
            
            if (color == BLACK) {
                result.transparentPixels++;
            } else {
                // 将COLORREF转换为long类型作为键
                colorCounts[static_cast<long>(color)]++;
            }
        }
    }
    
    // 找出使用最多的颜色
    if (!colorCounts.empty()) {
        int maxCount = 0;
        COLORREF mostUsedColor = 0;
        
        for (const auto& pair : colorCounts) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                mostUsedColor = pair.first;
            }
        }
        
        result.mostUsedColor = mostUsedColor;
        result.mostUsedColorCount = maxCount;
    }
    
    // 计算使用的颜色总数
    result.uniqueColorCount = colorCounts.size();
    
    return result;
}

// --- 初始化 ---
// --- 缩放控制函数 ---
void ZoomIn() {
    if (zoomLevel < 4) {
        zoomLevel *= 2;
    }
}

void ZoomOut() {
    if (zoomLevel > 1) {
        zoomLevel /= 2;
    }
}

void ResetZoom() {
    zoomLevel = 1;
    offsetX = 0;
    offsetY = 0;
}

void InitGame() {
    initgraph(WIN_W, WIN_H);
    setbkcolor(COLOR_BG);
    cleardevice();

    // 初始化画布
    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            gridData[i][j] = -1;

    // UI布局常量
    int bx = CANVAS_W + 15;
    int by = 300;
    int bw = 90; // 按钮宽度
    int bh = 35; // 按钮高度
    int gap = 10;

    // 工具按钮 (一行两个)
    btns.push_back({ bx, 150, bw, bh, L"画笔 (P)", 10 });
    btns.push_back({ bx + bw + gap, 150, bw, bh, L"橡皮 (E)", 11 });
    btns.push_back({ bx, 140 + bh + gap, 190, bh, L"油漆桶 (B)", 12 });

    // 功能按钮
    // 清空按钮（ID为0）
    btns.push_back({ bx, by, bw, bh, L"清空 (C)", 0 });
    btns.push_back({ bx + bw + gap, by, bw, bh, L"撤销 (Z)", 3 });
    btns.push_back({ bx, by + bh + gap, bw, bh, L"重做 (R)", 4 });

    btns.push_back({ bx, by + bh + gap, 90, bh, L"网格开关 (G)", 1 });
    btns.push_back({ bx + 95, by + bh + gap, 95, bh, L"背景切换 (H)", 7 });
    btns.push_back({ bx, by + (bh + gap) * 2, 190, bh, L"保存图片 (S)", 2 });
    btns.push_back({ bx, by + (bh + gap) * 3, 190, bh, L"导入图片 (I)", 5 });
    btns.push_back({ bx, by + (bh + gap) * 4, 190, bh, L"重置视图 (V)", 6 });
    
    // 画笔大小按钮
    int sizeBtnW = 60;
    int sizeBtnX = CANVAS_W + 15;
    for (int i = 0; i < 3; i++) {
        wchar_t sizeText[10];
        swprintf_s(sizeText, 10, L"%dx%d", i + 1, i + 1);
        btns.push_back({ sizeBtnX + i * (sizeBtnW + 5), by + (bh + gap) * 5, sizeBtnW, bh, sizeText, 20 + i });
    }
}

// --- 核心算法：油漆桶 (扫描线) ---
void FloodFill(int x, int y, COLORREF targetColor, COLORREF replaceColor) {
    if (x < 0 || x >= GRID_NUM || y < 0 || y >= GRID_NUM) return;
    if (targetColor == replaceColor) return;
    if (gridData[x][y] != targetColor) return;

    std::vector<std::pair<int,int>> stk;
    stk.reserve(GRID_NUM * GRID_NUM / 4);
    stk.emplace_back(x, y);

    while (!stk.empty()) {
        int sx = stk.back().first;
        int sy = stk.back().second;
        stk.pop_back();

        int lx = sx;
        while (lx >= 0 && gridData[lx][sy] == targetColor) lx--;
        lx++;

        int rx = lx;
        bool upSeed = false;
        bool downSeed = false;
        while (rx < GRID_NUM && gridData[rx][sy] == targetColor) {
            gridData[rx][sy] = replaceColor;

            if (sy - 1 >= 0) {
                if (gridData[rx][sy - 1] == targetColor && !upSeed) {
                    stk.emplace_back(rx, sy - 1);
                    upSeed = true;
                } else if (gridData[rx][sy - 1] != targetColor) {
                    upSeed = false;
                }
            }
            if (sy + 1 < GRID_NUM) {
                if (gridData[rx][sy + 1] == targetColor && !downSeed) {
                    stk.emplace_back(rx, sy + 1);
                    downSeed = true;
                } else if (gridData[rx][sy + 1] != targetColor) {
                    downSeed = false;
                }
            }
            rx++;
        }
    }
}

// --- 历史记录管理 ---
void SaveHistory() {
    HistoryState state;
    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            state.data[i][j] = gridData[i][j];

    undoStack.push_back(state);
    // 新操作则清空重做栈
    redoStack.clear();

    if (undoStack.size() > 50) {
        undoStack.erase(undoStack.begin());
    }
}

void PerformUndo() {
    if (undoStack.empty()) return;
    HistoryState lastState = undoStack.back();
    undoStack.pop_back();

    // 保存当前到重做栈
    HistoryState cur;
    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            cur.data[i][j] = gridData[i][j];
    redoStack.push_back(cur);

    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            gridData[i][j] = lastState.data[i][j];
}

// --- 导入图片功能 ---
void ImportImage() {
    OPENFILENAME ofn;
    wchar_t szFile[260] = L"";
    
    // 初始化OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"图片文件\0*.bmp;*.jpg;*.jpeg;*.png;*.gif\0所有文件\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        SaveHistory(); // 保存当前状态以便撤销
        
        IMAGE img;
        if (loadimage(&img, szFile) == 0) {
            // 调整图像大小以适应画布
            IMAGE scaledImg(CANVAS_W, CANVAS_H);
            SetWorkingImage(&scaledImg);
            putimage(0, 0, CANVAS_W, CANVAS_H, &img, 0, 0, SRCCOPY);
            SetWorkingImage(NULL);
            
            // 将调整后的图像采样到网格中
            for (int i = 0; i < GRID_NUM; i++) {
                for (int j = 0; j < GRID_NUM; j++) {
                    // 计算像素位置
                    int x = i * CELL_SIZE + CELL_SIZE / 2;
                    int y = j * CELL_SIZE + CELL_SIZE / 2;
                    
                    // 获取像素颜色（使用EasyX的getpixel函数）
                    SetWorkingImage(&scaledImg); // 确保工作图像设置正确
                    COLORREF color = getpixel(x, y);
                    
                    // 设置到网格数据中
                    gridData[i][j] = color;
                }
            }
            SetWorkingImage(NULL); // 恢复默认工作图像
            
            // 临时图像资源会在函数结束时自动释放
            
            MessageBox(GetHWnd(), L"图片导入成功！", L"成功", MB_OK);
        } else {
            MessageBox(GetHWnd(), L"图片加载失败！", L"错误", MB_OK | MB_ICONERROR);
        }
    }
}

void PerformRedo() {
    if (redoStack.empty()) return;
    HistoryState last = redoStack.back();
    redoStack.pop_back();

    // 把当前存到撤销栈
    HistoryState cur;
    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            cur.data[i][j] = gridData[i][j];
    undoStack.push_back(cur);

    for (int i = 0; i < GRID_NUM; i++)
        for (int j = 0; j < GRID_NUM; j++)
            gridData[i][j] = last.data[i][j];
}

// --- 绘制界面 ---
// DrawInterface 现在在 UpdateCanvas 之后调用，确保 UI 在画布之上
void DrawInterface() {
    // 1. 绘制右侧背景
    setfillcolor(COLOR_BG);
    solidrectangle(CANVAS_W, 0, WIN_W, WIN_H);

    // 2. 预览与色板
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    outtextxy(CANVAS_W + 15, 10, L"当前颜色:");

    // 预览色块（带边框，边框为高对比色）
    setfillcolor(curColor);
    fillrectangle(CANVAS_W + 15, 30, CANVAS_W + 85, 60);
    // 预览边框：如果是白色则使用黑色边框以保证可见性，否则使用白色或激活色
    COLORREF previewBorder = (curColor == WHITE) ? BLACK : WHITE;
    setlinecolor(previewBorder);
    rectangle(CANVAS_W + 15, 30, CANVAS_W + 85, 60);

    // 3. 色板
    int pSize = 25;
    for (int i = 0; i < 16; i++) {
        int r = i / 4;
        int c = i % 4;
        int x = CANVAS_W + 15 + c * (pSize + 5);
        int y = 70 + r * (pSize + 5);

        setfillcolor(palette[i]);
        fillrectangle(x, y, x + pSize, y + pSize);

        // 选中时绘制高亮边框
        if (palette[i] == curColor) {
            setlinecolor(COLOR_ACT);
            setlinestyle(PS_SOLID, 3);
            rectangle(x, y, x + pSize, y + pSize);
            setlinestyle(PS_SOLID, 1);
        } else {
            // 小边框以区分白色单元格
            setlinecolor(BLACK);
            rectangle(x, y, x + pSize, y + pSize);
        }
    }

    // 4. 按钮绘制
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetHWnd(), &pt);
    int mx = pt.x;
    int my = pt.y;

    // 确保按钮文字颜色
    settextcolor(WHITE);
    for (size_t i = 0; i < btns.size(); i++) {
        bool isActiveTool = false;
        if ((btns[i].id == 10 && currentTool == TOOL_PEN) ||
            (btns[i].id == 11 && currentTool == TOOL_ERASER) ||
            (btns[i].id == 12 && currentTool == TOOL_BUCKET)) {
            isActiveTool = true;
        }

        if (isActiveTool) setfillcolor(COLOR_ACT);
        else if (IsButtonClicked(btns[i], mx, my)) setfillcolor(COLOR_HOVER);
        else setfillcolor(COLOR_BTN);

        setlinecolor(WHITE);
        fillrectangle(btns[i].x, btns[i].y, btns[i].x + btns[i].w, btns[i].y + btns[i].h);

        int tw = textwidth(btns[i].text);
        int th = textheight(btns[i].text);
        outtextxy(btns[i].x + (btns[i].w - tw) / 2, btns[i].y + (btns[i].h - th) / 2, btns[i].text);
    }

    // 5. 状态提示
    wchar_t status[128];
    swprintf_s(status, 128, L"工具: %s | 画笔大小: %dx%d", 
        currentTool == TOOL_PEN ? L"画笔" : currentTool == TOOL_ERASER ? L"橡皮" : L"油漆桶",
        brushSize, brushSize);
    outtextxy(CANVAS_W + 15, WIN_H - 30, status);
}

// --- 画布更新 ---
void UpdateCanvas() {
    int scaledCellSize = CELL_SIZE * zoomLevel;
    
    // 绘制网格
    for (int i = 0; i < GRID_NUM; i++) {
        for (int j = 0; j < GRID_NUM; j++) {
            int x = i * scaledCellSize + offsetX;
            int y = j * scaledCellSize + offsetY;

            if (gridData[i][j] == -1) {
                // 根据背景样式设置填充颜色
                switch (backgroundStyle) {
                case 0: // 棋盘格
                    setfillcolor(((i + j) % 2) ? RGB(220, 220, 220) : WHITE);
                    break;
                case 1: // 浅色背景
                    setfillcolor(RGB(240, 240, 240));
                    break;
                case 2: // 深色背景
                    setfillcolor(RGB(30, 30, 30));
                    break;
                }
            }
            else {
                setfillcolor(gridData[i][j]);
            }
            solidrectangle(x, y, x + scaledCellSize, y + scaledCellSize);
        }
    }

    if (showGrid) {
        setlinecolor(COLOR_GRID);
        for (int i = 0; i <= GRID_NUM; i++) {
            int gridLineX = i * scaledCellSize + offsetX;
            line(gridLineX, 0, gridLineX, CANVAS_H);
        }
        for (int i = 0; i <= GRID_NUM; i++) {
            int gridLineY = i * scaledCellSize + offsetY;
            line(0, gridLineY, CANVAS_W, gridLineY);
        }
    }

    // 画布边框
    setlinecolor(WHITE);
    rectangle(offsetX, offsetY, offsetX + GRID_NUM * scaledCellSize - 1, offsetY + GRID_NUM * scaledCellSize - 1);
    
    // 显示缩放信息
    wchar_t zoomText[32];
    swprintf_s(zoomText, 32, L"缩放: %d%%", zoomLevel * 100);
    settextcolor(WHITE);
    outtextxy(CANVAS_W + 15, WIN_H - 60, zoomText);
}

// --- 设置画笔大小 ---
void SetBrushSize(int size) {
    if (size >= 1 && size <= 3) {
        brushSize = size;
    }
}

// --- 绘制指定大小的区域 ---
void DrawBrushStroke(int centerX, int centerY, COLORREF color) {
    int halfSize = brushSize / 2;
    for (int dx = -halfSize; dx <= halfSize; dx++) {
        for (int dy = -halfSize; dy <= halfSize; dy++) {
            int gx = centerX + dx;
            int gy = centerY + dy;
            if (gx >= 0 && gx < GRID_NUM && gy >= 0 && gy < GRID_NUM) {
                gridData[gx][gy] = color;
            }
        }
    }
}

// --- 键盘快捷键 ---
void HandleKeys() {
    if (_kbhit()) { // 检测是否有键盘输入
        char key = _getch();
        switch (key) {
        case 'p': case 'P': currentTool = TOOL_PEN; break;
        case 'e': case 'E': currentTool = TOOL_ERASER; break;
        case 'b': case 'B': currentTool = TOOL_BUCKET; break;
        case 'g': case 'G': showGrid = !showGrid; break;
        case 'h': case 'H': 
            backgroundStyle = (backgroundStyle + 1) % 3;
            break;
        case 's': case 'S': SaveArtwork(); break;
        case 'c': case 'C': 
            // 清空画布快捷键
            SaveHistory();
            for (int i = 0; i < GRID_NUM; i++)
                for (int j = 0; j < GRID_NUM; j++)
                    gridData[i][j] = -1;
            break;
        case 'z': case 'Z': case 26: // Ctrl+Z
            PerformUndo();
            break;
        case 'r': case 'R':
            PerformRedo();
            break;
        case 'i': case 'I':
            ImportImage();
            break;
        case '+': case '=': // 放大
            ZoomIn();
            break;
        case '-': case '_': // 缩小
            ZoomOut();
            break;
        case 'v': case 'V': // 重置视图
            ResetZoom();
            break;
        case '1': SetBrushSize(1); break;
        case '2': SetBrushSize(2); break;
        case '3': SetBrushSize(3); break;
        }
    }
}

// --- 鼠标处理 ---
void HandleMouse() {
    static bool isDrawing = false;
    static int lastGx = -1, lastGy = -1;
    
    while (MouseHit()) {
        MOUSEMSG m = GetMouseMsg();
        int scaledCellSize = CELL_SIZE * zoomLevel;

        switch (m.uMsg) {

            // --- 情况1：鼠标移动 (处理拖动绘图或画布平移) ---
        case WM_MOUSEMOVE:
            // 处理画布拖动（鼠标中键）
            if (m.mkMButton) {
                int dx = m.x - prevMouseX;
                int dy = m.y - prevMouseY;
                offsetX += dx;
                offsetY += dy;
                prevMouseX = m.x;
                prevMouseY = m.y;
            }
            // 处理绘图（左键且在画布范围内）
            else if (m.mkLButton && m.x >= 0 && m.x < CANVAS_W && m.y >= 0 && m.y < CANVAS_H) {
                // 将屏幕坐标转换为网格坐标
                int gx = (m.x - offsetX) / scaledCellSize;
                int gy = (m.y - offsetY) / scaledCellSize;
                
                // 检查是否在有效网格范围内
                if (gx >= 0 && gx < GRID_NUM && gy >= 0 && gy < GRID_NUM) {
                    // 只在第一次绘制或网格位置改变时保存历史
                    if (!isDrawing || lastGx != gx || lastGy != gy) {
                        SaveHistory();
                        isDrawing = true;
                        lastGx = gx;
                        lastGy = gy;
                    }

                    if (currentTool == TOOL_PEN) {
                        DrawBrushStroke(gx, gy, curColor);
                    }
                    else if (currentTool == TOOL_ERASER) {
                        DrawBrushStroke(gx, gy, -1);
                    }
                }
            }
            break;

            // --- 情况2：左键点击 (处理单击绘图、按钮点击) ---
        case WM_LBUTTONDOWN:
            prevMouseX = m.x;
            prevMouseY = m.y;
            // A. 如果点在画布里
            if (m.x >= 0 && m.x < CANVAS_W && m.y >= 0 && m.y < CANVAS_H) {
                // 将屏幕坐标转换为网格坐标
                int gx = (m.x - offsetX) / scaledCellSize;
                int gy = (m.y - offsetY) / scaledCellSize;
                
                // 检查是否在有效网格范围内
                if (gx >= 0 && gx < GRID_NUM && gy >= 0 && gy < GRID_NUM) {
                    SaveHistory(); // 记录撤销步骤
                    isDrawing = true;
                    lastGx = gx;
                    lastGy = gy;

                    if (currentTool == TOOL_BUCKET) {
                        FloodFill(gx, gy, gridData[gx][gy], curColor);
                    }
                    else if (currentTool == TOOL_PEN) {
                        DrawBrushStroke(gx, gy, curColor);
                    }
                    else if (currentTool == TOOL_ERASER) {
                        DrawBrushStroke(gx, gy, -1);
                    }
                }
            }
            // B. 如果点在右侧 UI 区域
            else {
                // 1. 检测色板
                int pSize = 25;
                for (int i = 0; i < 16; i++) {
                    int r = i / 4; int c = i % 4;
                    int x = CANVAS_W + 15 + c * (pSize + 5);
                    int y = 70 + r * (pSize + 5);
                    if (m.x >= x && m.x <= x + pSize && m.y >= y && m.y <= y + pSize) {
                        curColor = palette[i];
                        if (currentTool == TOOL_ERASER) currentTool = TOOL_PEN;
                    }
                }

                // 2. 检测按钮
                for (auto& btn : btns) {
                    if (IsButtonClicked(btn, m.x, m.y)) {
                        // 不再直接用白色覆盖按钮区域（避免遮挡文字）
                        // 让主循环的 DrawInterface 负责完整重绘按钮状态

                        switch (btn.id) {
                        case 0: // 清空
                            SaveHistory();
                            for (int i = 0; i < GRID_NUM; i++)
                                for (int j = 0; j < GRID_NUM; j++)
                                    gridData[i][j] = -1;
                            break;
                        case 1: showGrid = !showGrid; break;
                        case 2: SaveArtwork(); break;
                        case 3: PerformUndo(); break;
                        case 4: PerformRedo(); break;
                        case 5: ImportImage(); break;
                        case 6: ResetZoom(); break;
                        case 7: 
                            backgroundStyle = (backgroundStyle + 1) % 3;
                            break;
                        case 20: SetBrushSize(1); break;
                        case 21: SetBrushSize(2); break;
                        case 22: SetBrushSize(3); break;
                        case 10: currentTool = TOOL_PEN; break;
                        case 11: currentTool = TOOL_ERASER; break;
                        case 12: currentTool = TOOL_BUCKET; break;
                        }
                    }
                }
            }
            break;
            
            // 鼠标中键按下
            case WM_MBUTTONDOWN:
                prevMouseX = m.x;
                prevMouseY = m.y;
                isDraggingCanvas = true;
                break;
                
            // 鼠标中键释放
            case WM_MBUTTONUP:
                isDraggingCanvas = false;
                break;
                
            // 鼠标左键释放
            case WM_LBUTTONUP:
                isDrawing = false;
                break;
                
            // 滚轮事件：缩放
            case WM_MOUSEWHEEL:
                // 鼠标滚轮向前：放大
                if (m.wheel > 0) {
                    ZoomIn();
                } else { // 向后滚轮：缩小
                    ZoomOut();
                }
                break;

            // --- 情况3：右键点击 (吸管) ---
            case WM_RBUTTONDOWN: 
                prevMouseX = m.x;
                prevMouseY = m.y;
                // 将屏幕坐标转换为网格坐标
                int gx = (m.x - offsetX) / scaledCellSize;
                int gy = (m.y - offsetY) / scaledCellSize;
                
                if (m.x >= 0 && m.x < CANVAS_W && m.y >= 0 && m.y < CANVAS_H && 
                    gx >= 0 && gx < GRID_NUM && gy >= 0 && gy < GRID_NUM) {
                    if (gridData[gx][gy] != -1) {
                        curColor = gridData[gx][gy];
                        currentTool = TOOL_PEN;
                    }
                }
                break;
        }
    }
}
bool IsButtonClicked(Button b, int mx, int my){
    return (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
}

// --- 保存图片 ---
void SaveArtwork(){
    IMAGE img(CANVAS_W, CANVAS_H);
    SetWorkingImage(&img);
    for (int i = 0; i < GRID_NUM; i++) {
        for (int j = 0; j < GRID_NUM; j++) {
            if (gridData[i][j] != -1) setfillcolor(gridData[i][j]);
            else setfillcolor(WHITE);
            solidrectangle(i * CELL_SIZE, j * CELL_SIZE, (i + 1) * CELL_SIZE, (j + 1) * CELL_SIZE);
        }
    }
    SetWorkingImage(NULL);
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "pixel_%Y%m%d_%H%M%S.png", &tstruct);
    wchar_t wFileName[100];
    swprintf_s(wFileName, 100, L"%S", buf);
    saveimage(wFileName, &img);
    MessageBox(GetHWnd(), L"图片已保存！", L"成功", MB_OK);
}

// 更新图层可见性函数实现
void UpdateLayersVisibility() {
    // 遍历所有图层，根据可见性和透明度更新显示
    for (int i = 0; i < layers.size(); i++) {
        // 可以在这里添加图层可见性的额外处理逻辑
        // 实际的图层渲染会在UpdateCanvas函数中进行
    }
}
