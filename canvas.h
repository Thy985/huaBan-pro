#pragma once
#include "huaBan.h"
#include "layer.h"

class Canvas {
private:
    LayerManager layerManager;
    bool needUpdate;
    int zoomLevel;
    int offsetX, offsetY;
    bool showGrid;
    int backgroundStyle;

public:
    Canvas();
    
    // 图层管理
    LayerManager& GetLayerManager();
    const LayerManager& GetLayerManager() const;
    
    void Clear();
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y) const;
    
    void Draw();
    void DrawBorder();
    
    void SetZoom(int level);
    int GetZoom() const { return zoomLevel; }
    
    void Pan(int dx, int dy);
    void GetOffset(int& x, int& y) const { x = offsetX; y = offsetY; }
    
    void SetShowGrid(bool show) { showGrid = show; }
    bool GetShowGrid() const { return showGrid; }
    
    void SetBackgroundStyle(int style) { backgroundStyle = style; }
    int GetBackgroundStyle() const { return backgroundStyle; }
    
    bool ScreenToGrid(int screenX, int screenY, int& gridX, int& gridY) const;
    
    void CopyFrom(const Canvas& other);
    void CopyTo(COLORREF dest[GRID_NUM][GRID_NUM]) const;
    void CopyFrom(const COLORREF src[GRID_NUM][GRID_NUM]);
    
    bool NeedUpdate() const { return needUpdate; }
    void SetNeedUpdate(bool need) { needUpdate = need; }
};
