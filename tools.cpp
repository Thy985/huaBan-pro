#include "tools.h"
#include <queue>

Tools::Tools() 
    : currentTool(TOOL_PEN)
    , currentColor(RED)
    , brushSize(1) {
}

void Tools::Draw(Canvas& canvas, int gridX, int gridY) {
    for (int dx = -brushSize + 1; dx < brushSize; dx++) {
        for (int dy = -brushSize + 1; dy < brushSize; dy++) {
            int x = gridX + dx;
            int y = gridY + dy;
            float distance = sqrtf((float)(dx * dx + dy * dy));
            if (distance < brushSize - 0.5f) {
                canvas.SetPixel(x, y, currentColor);
            }
        }
    }
}

void Tools::Erase(Canvas& canvas, int gridX, int gridY) {
    for (int dx = -brushSize + 1; dx < brushSize; dx++) {
        for (int dy = -brushSize + 1; dy < brushSize; dy++) {
            int x = gridX + dx;
            int y = gridY + dy;
            float distance = sqrtf((float)(dx * dx + dy * dy));
            if (distance < brushSize - 0.5f) {
                canvas.SetPixel(x, y, BLACK);
            }
        }
    }
}

void Tools::FloodFillInternal(Canvas& canvas, int x, int y, COLORREF targetColor, COLORREF replaceColor) {
    if (targetColor == replaceColor) return;
    
    std::queue<std::pair<int, int>> q;
    q.push({x, y});
    
    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();
        
        if (cx < 0 || cx >= GRID_NUM || cy < 0 || cy >= GRID_NUM) continue;
        if (canvas.GetPixel(cx, cy) != targetColor) continue;
        
        canvas.SetPixel(cx, cy, replaceColor);
        
        q.push({cx + 1, cy});
        q.push({cx - 1, cy});
        q.push({cx, cy + 1});
        q.push({cx, cy - 1});
    }
}

void Tools::Bucket(Canvas& canvas, int gridX, int gridY) {
    COLORREF targetColor = canvas.GetPixel(gridX, gridY);
    FloodFillInternal(canvas, gridX, gridY, targetColor, currentColor);
}

void Tools::Apply(Canvas& canvas, int gridX, int gridY) {
    switch (currentTool) {
        case TOOL_PEN:
            Draw(canvas, gridX, gridY);
            break;
        case TOOL_ERASER:
            Erase(canvas, gridX, gridY);
            break;
        case TOOL_BUCKET:
            Bucket(canvas, gridX, gridY);
            break;
    }
}
