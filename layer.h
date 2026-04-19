#ifndef LAYER_H
#define LAYER_H

#include "huaBan.h"

class Layer {
private:
    COLORREF pixels[CANVAS_SIZE][CANVAS_SIZE];
    bool visible;
    int opacity; // 0-255
    std::string name;
    int width;
    int height;

public:
    Layer(const std::string& layerName = "New Layer");
    
    // 基本操作
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y) const;
    void Clear(COLORREF clearColor = TRANSPARENT);
    
    // 图层属性
    void SetVisible(bool isVisible);
    bool IsVisible() const;
    
    void SetOpacity(int value);
    int GetOpacity() const;
    
    void SetName(const std::string& newName);
    std::string GetName() const;
    
    // 尺寸
    int GetWidth() const;
    int GetHeight() const;
    
    // 绘制
    void Draw(HDC hdc, int offsetX, int offsetY, int zoom) const;
};

class LayerManager {
private:
    std::vector<Layer*> layers;
    int activeLayerIndex;
    
public:
    LayerManager();
    ~LayerManager();
    
    // 图层管理
    int AddLayer(const std::string& name = "New Layer");
    void RemoveLayer(int index);
    void DuplicateLayer(int index);
    void MoveLayerUp(int index);
    void MoveLayerDown(int index);
    
    // 活动图层
    void SetActiveLayer(int index);
    int GetActiveLayerIndex() const;
    Layer* GetActiveLayer();
    const Layer* GetActiveLayer() const;
    
    // 图层访问
    Layer* GetLayer(int index);
    const Layer* GetLayer(int index) const;
    int GetLayerCount() const;
    
    // 绘制所有图层
    void DrawAll(HDC hdc, int offsetX, int offsetY, int zoom) const;
};

#endif // LAYER_H