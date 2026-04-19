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

void Tools::DrawLine(Canvas& canvas, int startX, int startY, int endX, int endY) {
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);
    int sx = startX < endX ? 1 : -1;
    int sy = startY < endY ? 1 : -1;
    int err = dx - dy;
    
    int x = startX;
    int y = startY;
    
    while (true) {
        canvas.SetPixel(x, y, currentColor);
        
        if (x == endX && y == endY) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void Tools::DrawRectangle(Canvas& canvas, int startX, int startY, int endX, int endY) {
    int left = std::min(startX, endX);
    int right = std::max(startX, endX);
    int top = std::min(startY, endY);
    int bottom = std::max(startY, endY);
    
    // 绘制四条边
    for (int x = left; x <= right; x++) {
        canvas.SetPixel(x, top, currentColor);
        canvas.SetPixel(x, bottom, currentColor);
    }
    for (int y = top; y <= bottom; y++) {
        canvas.SetPixel(left, y, currentColor);
        canvas.SetPixel(right, y, currentColor);
    }
}

void Tools::DrawCircle(Canvas& canvas, int centerX, int centerY, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        canvas.SetPixel(centerX + x, centerY + y, currentColor);
        canvas.SetPixel(centerX + y, centerY + x, currentColor);
        canvas.SetPixel(centerX - y, centerY + x, currentColor);
        canvas.SetPixel(centerX - x, centerY + y, currentColor);
        canvas.SetPixel(centerX - x, centerY - y, currentColor);
        canvas.SetPixel(centerX - y, centerY - x, currentColor);
        canvas.SetPixel(centerX + y, centerY - x, currentColor);
        canvas.SetPixel(centerX + x, centerY - y, currentColor);
        
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
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
