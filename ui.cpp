#include "ui.h"
#include <algorithm>

UI::UI() 
    : recentColorCount(0) {
    InitButtons();
    InitPalette();
}

void UI::InitButtons() {
    buttons = {
        { 10, 10, 90, 30, L"清空", BTN_CLEAR, true, true, false, false },
        { 110, 10, 90, 30, L"网格", BTN_GRID, true, true, true, true },
        { 10, 50, 90, 30, L"保存", BTN_SAVE, true, true, false, false },
        { 110, 50, 40, 30, L"撤销", BTN_UNDO, true, true, false, false },
        { 160, 50, 40, 30, L"重做", BTN_REDO, true, true, false, false },
        { 10, 90, 190, 30, L"导入图片", BTN_IMPORT, true, true, false, false },
        { 10, 130, 60, 30, L"画笔", BTN_PEN, true, true, false, false },
        { 80, 130, 60, 30, L"橡皮", BTN_ERASER, true, true, false, false },
        { 150, 130, 50, 30, L"填充", BTN_BUCKET, true, true, false, false },
        { 10, 170, 60, 30, L"直线", BTN_LINE, true, true, false, false },
        { 80, 170, 60, 30, L"矩形", BTN_RECTANGLE, true, true, false, false },
        { 150, 170, 50, 30, L"圆形", BTN_CIRCLE, true, true, false, false },
        { 10, 210, 60, 30, L"1x", BTN_SIZE_1, true, true, false, false },
        { 80, 210, 60, 30, L"2x", BTN_SIZE_2, true, true, false, false },
        { 150, 210, 50, 30, L"3x", BTN_SIZE_3, true, true, false, false },
        { 10, 250, 60, 30, L"亮度", BTN_BRIGHTNESS, true, true, false, false },
        { 80, 250, 60, 30, L"对比度", BTN_CONTRAST, true, true, false, false },
        { 150, 250, 50, 30, L"模糊", BTN_BLUR, true, true, false, false },
        { 10, 290, 60, 30, L"锐化", BTN_SHARPEN, true, true, false, false },
        { 80, 290, 60, 30, L"灰度", BTN_GRAYSCALE, true, true, false, false },
        { 150, 290, 50, 30, L"反色", BTN_INVERT, true, true, false, false },
        // 动画按钮
        { 10, 330, 60, 30, L"添加帧", BTN_ADD_FRAME, true, true, false, false },
        { 80, 330, 60, 30, L"删除帧", BTN_REMOVE_FRAME, true, true, false, false },
        { 150, 330, 50, 30, L"复制帧", BTN_DUPLICATE_FRAME, true, true, false, false },
        { 10, 370, 60, 30, L"上一帧", BTN_PREV_FRAME, true, true, false, false },
        { 80, 370, 60, 30, L"下一帧", BTN_NEXT_FRAME, true, true, false, false },
        { 150, 370, 50, 30, L"播放", BTN_PLAY_ANIMATION, true, true, false, false },
        { 10, 410, 190, 30, L"导出GIF", BTN_EXPORT_GIF, true, true, false, false },
    };
}

void UI::InitPalette() {
    palette[0] = BLACK; palette[1] = WHITE; palette[2] = RGB(200,200,200); palette[3] = RGB(100,100,100);
    palette[4] = RED; palette[5] = GREEN; palette[6] = BLUE; palette[7] = RGB(255,255,0);
    palette[8] = RGB(255,165,0); palette[9] = RGB(128,0,128); palette[10] = RGB(0,255,255); palette[11] = RGB(255,192,203);
    palette[12] = RGB(139,69,19); palette[13] = RGB(34,139,34); palette[14] = RGB(75,0,130); palette[15] = RGB(255,215,0);
    
    paletteNames[0] = L"黑色"; paletteNames[1] = L"白色"; paletteNames[2] = L"浅灰"; paletteNames[3] = L"深灰";
    paletteNames[4] = L"红色"; paletteNames[5] = L"绿色"; paletteNames[6] = L"蓝色"; paletteNames[7] = L"黄色";
    paletteNames[8] = L"橙色"; paletteNames[9] = L"紫色"; paletteNames[10] = L"青色"; paletteNames[11] = L"粉色";
    paletteNames[12] = L"棕色"; paletteNames[13] = L"森林绿"; paletteNames[14] = L"靛蓝"; paletteNames[15] = L"金色";
}

void UI::Draw(const Canvas& canvas, const Tools& tools) {
    DrawButtons(tools);
    DrawPalette();
    DrawInfo(canvas, tools);
}

void UI::DrawButtons(const Tools& tools) {
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(GetHWnd(), &mousePos);
    
    for (auto& btn : buttons) {
        if (!btn.visible) continue;
        
        COLORREF btnColor = btn.enabled ? COLOR_BTN : RGB(50, 55, 65);
        COLORREF textColor = WHITE;
        
        if (IsButtonClicked(btn, mousePos.x, mousePos.y) && btn.enabled) {
            btnColor = COLOR_HOVER;
        }
        
        if (btn.id >= BTN_PEN && btn.id <= BTN_CIRCLE) {
            if ((btn.id == BTN_PEN && tools.GetTool() == TOOL_PEN) ||
                (btn.id == BTN_ERASER && tools.GetTool() == TOOL_ERASER) ||
                (btn.id == BTN_BUCKET && tools.GetTool() == TOOL_BUCKET) ||
                (btn.id == BTN_LINE && tools.GetTool() == TOOL_LINE) ||
                (btn.id == BTN_RECTANGLE && tools.GetTool() == TOOL_RECTANGLE) ||
                (btn.id == BTN_CIRCLE && tools.GetTool() == TOOL_CIRCLE)) {
                btnColor = COLOR_ACT;
            }
        }
        
        if (btn.id >= BTN_SIZE_1 && btn.id <= BTN_SIZE_3) {
            int size = btn.id - BTN_SIZE_1 + 1;
            if (size == tools.GetBrushSize()) {
                btnColor = COLOR_ACT;
            }
        }
        
        if (btn.isToggle && btn.isToggled) {
            btnColor = COLOR_ACT;
        }
        
        setfillcolor(btnColor);
        setlinecolor(RGB(100, 100, 100));
        fillrectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);
        
        settextcolor(textColor);
        setbkmode(TRANSPARENT);
        RECT textRect = { btn.x, btn.y, btn.x + btn.w, btn.y + btn.h };
        drawtext(btn.text, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void UI::DrawPalette() {
    int paletteX = 20;
    int paletteY = 450;
    int colorSize = 20;
    int colorSpacing = 25;
    
    settextcolor(WHITE);
    outtextxy(paletteX, paletteY - 20, L"调色板");
    
    for (int i = 0; i < 16; i++) {
        int x = paletteX + (i % 8) * colorSpacing;
        int y = paletteY + (i / 8) * colorSpacing;
        
        setfillcolor(palette[i]);
        setlinecolor(RGB(100, 100, 100));
        setlinestyle(PS_SOLID, 1);
        fillrectangle(x, y, x + colorSize, y + colorSize);
        rectangle(x, y, x + colorSize, y + colorSize);
    }
    
    paletteY += 60;
    outtextxy(paletteX, paletteY - 20, L"最近使用");
    
    for (int i = 0; i < recentColorCount && i < 8; i++) {
        int x = paletteX + (i % 8) * colorSpacing;
        int y = paletteY + (i / 8) * colorSpacing;
        
        setfillcolor(recentColors[i]);
        setlinecolor(RGB(100, 100, 100));
        fillrectangle(x, y, x + colorSize, y + colorSize);
        rectangle(x, y, x + colorSize, y + colorSize);
    }
}

void UI::DrawInfo(const Canvas& canvas, const Tools& tools) {
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    
    const wchar_t* toolName = L"";
    switch (tools.GetTool()) {
        case TOOL_PEN: toolName = L"工具: 画笔"; break;
        case TOOL_ERASER: toolName = L"工具: 橡皮"; break;
        case TOOL_BUCKET: toolName = L"工具: 填充"; break;
        case TOOL_LINE: toolName = L"工具: 直线"; break;
        case TOOL_RECTANGLE: toolName = L"工具: 矩形"; break;
        case TOOL_CIRCLE: toolName = L"工具: 圆形"; break;
    }
    outtextxy(UI_WIDTH + 10, 10, toolName);
    
    outtextxy(UI_WIDTH + 10, 30, L"当前颜色:");
    setfillcolor(tools.GetColor());
    setlinecolor(WHITE);
    fillrectangle(UI_WIDTH + 80, 30, UI_WIDTH + 100, 50);
    
    wchar_t sizeText[50];
    swprintf(sizeText, 50, L"画笔大小: %d", tools.GetBrushSize());
    outtextxy(UI_WIDTH + 120, 30, sizeText);
    
    wchar_t zoomText[50];
    swprintf(zoomText, 50, L"缩放: %d%%", canvas.GetZoom() * 100);
    outtextxy(WIN_W - 120, 10, zoomText);
    
    // 添加快捷键提示
    outtextxy(UI_WIDTH + 10, 50, L"快捷键:");
    outtextxy(UI_WIDTH + 10, 70, L"Ctrl+Z: 撤销");
    outtextxy(UI_WIDTH + 10, 90, L"Ctrl+Y: 重做");
    outtextxy(UI_WIDTH + 10, 110, L"Ctrl+S: 保存");
    outtextxy(UI_WIDTH + 10, 130, L"Ctrl+拖动: 平移");
    outtextxy(UI_WIDTH + 10, 150, L"鼠标滚轮: 缩放");
}

bool UI::IsButtonClicked(const Button& btn, int x, int y) const {
    return x >= btn.x && x < btn.x + btn.w && y >= btn.y && y < btn.y + btn.h;
}

Button* UI::FindButton(int x, int y) {
    for (auto& btn : buttons) {
        if (IsButtonClicked(btn, x, y) && btn.enabled && btn.visible) {
            return &btn;
        }
    }
    return nullptr;
}

int UI::FindPaletteColor(int x, int y) {
    int paletteX = 20;
    int paletteY = 450;
    int colorSize = 20;
    int colorSpacing = 25;
    
    for (int i = 0; i < 16; i++) {
        int cx = paletteX + (i % 8) * colorSpacing;
        int cy = paletteY + (i / 8) * colorSpacing;
        if (x >= cx && x <= cx + colorSize && y >= cy && y <= cy + colorSize) {
            return i;
        }
    }
    
    paletteY += 60;
    for (int i = 0; i < recentColorCount && i < 8; i++) {
        int cx = paletteX + (i % 8) * colorSpacing;
        int cy = paletteY + (i / 8) * colorSpacing;
        if (x >= cx && x <= cx + colorSize && y >= cy && y <= cy + colorSize) {
            return 16 + i;
        }
    }
    
    return -1;
}

void UI::AddRecentColor(COLORREF color) {
    for (int i = 0; i < recentColorCount; i++) {
        if (recentColors[i] == color) {
            if (i > 0) {
                for (int j = i; j > 0; j--) {
                    recentColors[j] = recentColors[j - 1];
                }
                recentColors[0] = color;
            }
            return;
        }
    }
    
    if (recentColorCount >= 8) {
        recentColorCount = 7;
    }
    
    for (int i = recentColorCount; i > 0; i--) {
        recentColors[i] = recentColors[i - 1];
    }
    
    recentColors[0] = color;
    recentColorCount++;
}
