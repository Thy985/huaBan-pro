#include "huaBan.h"
#include "canvas.h"
#include "tools.h"
#include "history.h"
#include "ui.h"
#include "fileio.h"
#include "filter.h"
#include "animation.h"

class PixelEditor {
private:
    Canvas canvas;
    Tools tools;
    History history;
    UI ui;
    Animation animation;
    
    bool isMouseDown;
    bool isDraggingCanvas;
    int prevMouseX;
    int prevMouseY;
    
    // 几何工具状态
    bool isDrawingShape;
    int startGridX;
    int startGridY;
    
    // 动画播放状态
    bool isPlayingAnimation;
    DWORD lastFrameTime;
    
    void HandleMouse();
    void HandleKeys();
    void ProcessButtonClick(Button* btn);
    void HandleAnimation();
    
public:
    PixelEditor();
    void Run();
};

PixelEditor::PixelEditor() 
    : isMouseDown(false)
    , isDraggingCanvas(false)
    , prevMouseX(0)
    , prevMouseY(0)
    , isDrawingShape(false)
    , startGridX(0)
    , startGridY(0)
    , isPlayingAnimation(false)
    , lastFrameTime(0) {
}

void PixelEditor::ProcessButtonClick(Button* btn) {
    if (!btn) return;
    
    switch (btn->id) {
        case BTN_CLEAR:
            history.SaveState(canvas);
            canvas.Clear();
            break;
        case BTN_GRID:
            btn->isToggled = !btn->isToggled;
            canvas.SetShowGrid(btn->isToggled);
            break;
        case BTN_SAVE:
            FileIO::SaveImage(canvas);
            break;
        case BTN_UNDO:
            history.Undo(canvas);
            break;
        case BTN_REDO:
            history.Redo(canvas);
            break;
        case BTN_IMPORT:
            history.SaveState(canvas);
            FileIO::LoadImage(canvas);
            break;
        case BTN_PEN:
            tools.SetTool(TOOL_PEN);
            break;
        case BTN_ERASER:
            tools.SetTool(TOOL_ERASER);
            break;
        case BTN_BUCKET:
            tools.SetTool(TOOL_BUCKET);
            break;
        case BTN_LINE:
            tools.SetTool(TOOL_LINE);
            break;
        case BTN_RECTANGLE:
            tools.SetTool(TOOL_RECTANGLE);
            break;
        case BTN_CIRCLE:
            tools.SetTool(TOOL_CIRCLE);
            break;
        case BTN_SIZE_1:
            tools.SetBrushSize(1);
            break;
        case BTN_SIZE_2:
            tools.SetBrushSize(2);
            break;
        case BTN_SIZE_3:
            tools.SetBrushSize(3);
            break;
        // 滤镜按钮
        case BTN_BRIGHTNESS:
            history.SaveState(canvas);
            Filter::AdjustBrightness(canvas, 30); // 增加30亮度
            break;
        case BTN_CONTRAST:
            history.SaveState(canvas);
            Filter::AdjustContrast(canvas, 30); // 增加30对比度
            break;
        case BTN_BLUR:
            history.SaveState(canvas);
            Filter::ApplyBlur(canvas, 2); // 模糊半径2
            break;
        case BTN_SHARPEN:
            history.SaveState(canvas);
            Filter::ApplySharpen(canvas);
            break;
        case BTN_GRAYSCALE:
            history.SaveState(canvas);
            Filter::ApplyGrayscale(canvas);
            break;
        case BTN_INVERT:
            history.SaveState(canvas);
            Filter::ApplyInvert(canvas);
            break;
        // 动画按钮
        case BTN_ADD_FRAME:
            animation.AddFrame();
            // 从当前画布复制到新帧
            AnimationFrame* newFrame = animation.GetCurrentFrame();
            if (newFrame) {
                newFrame->CopyFrom(canvas);
            }
            break;
        case BTN_REMOVE_FRAME:
            animation.RemoveFrame(animation.GetCurrentFrameIndex());
            // 显示当前帧
            AnimationFrame* currentFrame = animation.GetCurrentFrame();
            if (currentFrame) {
                currentFrame->CopyTo(canvas);
            }
            break;
        case BTN_DUPLICATE_FRAME:
            animation.DuplicateFrame(animation.GetCurrentFrameIndex());
            break;
        case BTN_PREV_FRAME:
            animation.PreviousFrame();
            // 显示当前帧
            currentFrame = animation.GetCurrentFrame();
            if (currentFrame) {
                currentFrame->CopyTo(canvas);
            }
            break;
        case BTN_NEXT_FRAME:
            animation.NextFrame();
            // 显示当前帧
            currentFrame = animation.GetCurrentFrame();
            if (currentFrame) {
                currentFrame->CopyTo(canvas);
            }
            break;
        case BTN_PLAY_ANIMATION:
            isPlayingAnimation = !isPlayingAnimation;
            if (isPlayingAnimation) {
                lastFrameTime = GetTickCount();
            }
            break;
        case BTN_EXPORT_GIF:
            animation.ExportToGIF("animation.gif");
            break;
    }
}

void PixelEditor::HandleMouse() {
    if (MouseHit()) {
        MOUSEMSG msg = GetMouseMsg();
        
        if (msg.uMsg == WM_LBUTTONDOWN) {
            isMouseDown = true;
            prevMouseX = msg.x;
            prevMouseY = msg.y;
            
            if (msg.x < UI_WIDTH) {
                Button* btn = ui.FindButton(msg.x, msg.y);
                if (btn) {
                    ProcessButtonClick(btn);
                } else {
                    int colorIdx = ui.FindPaletteColor(msg.x, msg.y);
                    if (colorIdx >= 0 && colorIdx < 16) {
                        COLORREF paletteColors[16] = {
                            BLACK, WHITE, RGB(200,200,200), RGB(100,100,100),
                            RED, GREEN, BLUE, RGB(255,255,0),
                            RGB(255,165,0), RGB(128,0,128), RGB(0,255,255), RGB(255,192,203),
                            RGB(139,69,19), RGB(34,139,34), RGB(75,0,130), RGB(255,215,0)
                        };
                        tools.SetColor(paletteColors[colorIdx]);
                        ui.AddRecentColor(paletteColors[colorIdx]);
                    }
                }
            } else {
                int gridX, gridY;
                if (canvas.ScreenToGrid(msg.x, msg.y, gridX, gridY)) {
                    ToolType currentTool = tools.GetTool();
                    if (currentTool == TOOL_LINE || currentTool == TOOL_RECTANGLE || currentTool == TOOL_CIRCLE) {
                        // 开始绘制形状
                        isDrawingShape = true;
                        startGridX = gridX;
                        startGridY = gridY;
                    } else {
                        // 普通工具
                        history.SaveState(canvas);
                        tools.Apply(canvas, gridX, gridY);
                    }
                }
            }
        } else if (msg.uMsg == WM_LBUTTONUP) {
            if (isDrawingShape) {
                // 完成形状绘制
                int gridX, gridY;
                if (canvas.ScreenToGrid(msg.x, msg.y, gridX, gridY)) {
                    history.SaveState(canvas);
                    ToolType currentTool = tools.GetTool();
                    switch (currentTool) {
                        case TOOL_LINE:
                            tools.DrawLine(canvas, startGridX, startGridY, gridX, gridY);
                            break;
                        case TOOL_RECTANGLE:
                            tools.DrawRectangle(canvas, startGridX, startGridY, gridX, gridY);
                            break;
                        case TOOL_CIRCLE:
                            // 计算半径
                            int radius = (int)sqrt((gridX - startGridX) * (gridX - startGridX) + (gridY - startGridY) * (gridY - startGridY));
                            tools.DrawCircle(canvas, startGridX, startGridY, radius);
                            break;
                        default:
                            break;
                    }
                }
                isDrawingShape = false;
            }
            isMouseDown = false;
            isDraggingCanvas = false;
        } else if (msg.uMsg == WM_MOUSEMOVE) {
            if (isMouseDown) {
                if (msg.mkCtrl) {
                    if (!isDraggingCanvas) {
                        isDraggingCanvas = true;
                    } else {
                        canvas.Pan(msg.x - prevMouseX, msg.y - prevMouseY);
                        prevMouseX = msg.x;
                        prevMouseY = msg.y;
                    }
                } else if (msg.x >= UI_WIDTH) {
                    if (isDrawingShape) {
                        // 这里可以添加实时预览，但需要额外的逻辑
                    } else {
                        int gridX, gridY;
                        if (canvas.ScreenToGrid(msg.x, msg.y, gridX, gridY)) {
                            tools.Apply(canvas, gridX, gridY);
                        }
                    }
                }
            }
        } else if (msg.uMsg == WM_MOUSEWHEEL) {
            int delta = msg.wheel;
            if (delta > 0 && canvas.GetZoom() < 4) {
                canvas.SetZoom(canvas.GetZoom() + 1);
            } else if (delta < 0 && canvas.GetZoom() > 1) {
                canvas.SetZoom(canvas.GetZoom() - 1);
            }
        }
    }
}

void PixelEditor::HandleKeys() {
    if (_kbhit()) {
        int key = _getch();
        if (key == 0 || key == 0xE0) {
            key = _getch();
        }
        
        if (key == 26 && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                history.Redo(canvas);
            } else {
                history.Undo(canvas);
            }
        } else if (key == 'z' || key == 'Z') {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                history.Undo(canvas);
            }
        } else if (key == 'y' || key == 'Y') {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                history.Redo(canvas);
            }
        } else if (key == 's' || key == 'S') {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                FileIO::SaveImage(canvas);
            }
        }
    }
}

void PixelEditor::HandleAnimation() {
    if (isPlayingAnimation) {
        DWORD currentTime = GetTickCount();
        AnimationFrame* currentFrame = animation.GetCurrentFrame();
        if (currentFrame && currentTime - lastFrameTime >= currentFrame->GetDuration()) {
            animation.NextFrame();
            currentFrame = animation.GetCurrentFrame();
            if (currentFrame) {
                currentFrame->CopyTo(canvas);
            }
            lastFrameTime = currentTime;
        }
    }
}

void PixelEditor::Run() {
    initgraph(WIN_W, WIN_H);
    BeginBatchDraw();
    
    while (true) {
        cleardevice();
        
        canvas.Draw();
        ui.Draw(canvas, tools);
        canvas.DrawBorder();
        
        FlushBatchDraw();
        
        HandleMouse();
        HandleKeys();
        HandleAnimation();
        
        Sleep(10);
    }
    
    closegraph();
}

int main() {
    PixelEditor editor;
    editor.Run();
    return 0;
}
