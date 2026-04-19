#include "canvas.h"
#include <algorithm>

Canvas::Canvas() 
    : needUpdate(true)
    , zoomLevel(1)
    , offsetX(0)
    , offsetY(0)
    , showGrid(true)
    , backgroundStyle(0) {
}

LayerManager& Canvas::GetLayerManager() {
    return layerManager;
}

const LayerManager& Canvas::GetLayerManager() const {
    return layerManager;
}

void Canvas::Clear() {
    for (int i = 0; i < layerManager.GetLayerCount(); i++) {
        layerManager.GetLayer(i)->Clear();
    }
    needUpdate = true;
}

void Canvas::SetPixel(int x, int y, COLORREF color) {
    if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
        Layer* activeLayer = layerManager.GetActiveLayer();
        if (activeLayer) {
            activeLayer->SetPixel(x, y, color);
            needUpdate = true;
        }
    }
}

COLORREF Canvas::GetPixel(int x, int y) const {
    if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
        // 从顶层图层开始检查，返回第一个非透明像素
        for (int i = layerManager.GetLayerCount() - 1; i >= 0; i--) {
            const Layer* layer = layerManager.GetLayer(i);
            if (layer && layer->IsVisible()) {
                COLORREF color = layer->GetPixel(x, y);
                if (color != RGB(255, 0, 255)) { // 检查是否不是透明色
                    return color;
                }
            }
        }
    }
    return BLACK;
}

void Canvas::Draw() {
    // 绘制背景
    switch (backgroundStyle) {
        case 0:
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
        case 1:
            setfillcolor(RGB(240, 240, 240));
            solidrectangle(0, 0, WIN_W, WIN_H);
            break;
        case 2:
            setfillcolor(COLOR_BG);
            solidrectangle(0, 0, WIN_W, WIN_H);
            break;
    }

    // 计算绘制区域
    int canvasStartX = UI_WIDTH + offsetX;
    int canvasStartY = offsetY;
    int cellDrawSize = CELL_SIZE * zoomLevel;

    // 绘制所有图层
    HDC hdc = GetImageHDC();
    layerManager.DrawAll(hdc, canvasStartX, canvasStartY, cellDrawSize);

    // 绘制网格
    if (showGrid) {
        setlinecolor(COLOR_GRID);
        setlinestyle(PS_SOLID, 1);
        for (int i = 0; i <= GRID_NUM; i++) {
            int x = canvasStartX + i * cellDrawSize;
            int y = canvasStartY + i * cellDrawSize;
            line(x, canvasStartY, x, canvasStartY + GRID_NUM * cellDrawSize);
            line(canvasStartX, y, canvasStartX + GRID_NUM * cellDrawSize, y);
        }
    }

    needUpdate = false;
}

void Canvas::DrawBorder() {
    // 绘制画布边框
    setlinecolor(RGB(150, 150, 150));
    setlinestyle(PS_SOLID, 2);
    rectangle(UI_WIDTH - 2, -2, WIN_W + 1, WIN_H + 1);
    setlinestyle(PS_SOLID, 1);
}

void Canvas::SetZoom(int level) {
    if (level >= 1 && level <= 4) {
        int centerX = WIN_W / 2 - UI_WIDTH;
        int centerY = WIN_H / 2;
        float scaleRatio = (float)level / zoomLevel;
        offsetX = (int)(centerX - (centerX - offsetX) * scaleRatio);
        offsetY = (int)(centerY - (centerY - offsetY) * scaleRatio);
        zoomLevel = level;
        needUpdate = true;
    }
}

void Canvas::Pan(int dx, int dy) {
    offsetX += dx;
    offsetY += dy;
    
    int maxOffsetX = (GRID_NUM * CELL_SIZE * zoomLevel - CANVAS_W) / 2 + 50;
    int maxOffsetY = (GRID_NUM * CELL_SIZE * zoomLevel - CANVAS_H) / 2 + 50;
    if (maxOffsetX < 0) maxOffsetX = 0;
    if (maxOffsetY < 0) maxOffsetY = 0;
    
    offsetX = std::max(-maxOffsetX, std::min(maxOffsetX, offsetX));
    offsetY = std::max(-maxOffsetY, std::min(maxOffsetY, offsetY));
    
    needUpdate = true;
}

bool Canvas::ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY) const {
    int canvasStartX = UI_WIDTH + offsetX;
    int canvasStartY = offsetY;
    int cellDrawSize = CELL_SIZE * zoomLevel;
    
    gridX = (screenX - canvasStartX) / cellDrawSize;
    gridY = (screenY - canvasStartY) / cellDrawSize;
    
    return gridX >= 0 && gridX < GRID_NUM && gridY >= 0 && gridY < GRID_NUM;
}

void Canvas::CopyFrom(const Canvas& other) {
    // 这里简化处理，只复制活动图层
    Layer* activeLayer = layerManager.GetActiveLayer();
    const Layer* otherActiveLayer = other.layerManager.GetActiveLayer();
    if (activeLayer && otherActiveLayer) {
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                activeLayer->SetPixel(x, y, otherActiveLayer->GetPixel(x, y));
            }
        }
    }
    needUpdate = true;
}

void Canvas::CopyTo(COLORREF dest[GRID_NUM][GRID_NUM]) const {
    // 这里简化处理，只复制活动图层
    const Layer* activeLayer = layerManager.GetActiveLayer();
    if (activeLayer) {
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                dest[x][y] = activeLayer->GetPixel(x, y);
            }
        }
    }
}

void Canvas::CopyFrom(const COLORREF src[GRID_NUM][GRID_NUM]) {
    // 这里简化处理，只复制到活动图层
    Layer* activeLayer = layerManager.GetActiveLayer();
    if (activeLayer) {
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                activeLayer->SetPixel(x, y, src[x][y]);
            }
        }
    }
    needUpdate = true;
}
