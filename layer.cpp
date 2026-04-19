#include "layer.h"

// 定义透明色
#define TRANSPARENT RGB(255, 0, 255) // 洋红色作为透明色

Layer::Layer(const std::string& layerName) 
    : visible(true)
    , opacity(255)
    , name(layerName)
    , width(CANVAS_SIZE)
    , height(CANVAS_SIZE) {
    Clear();
}

void Layer::SetPixel(int x, int y, COLORREF color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y][x] = color;
    }
}

COLORREF Layer::GetPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y][x];
    }
    return TRANSPARENT;
}

void Layer::Clear(COLORREF clearColor) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pixels[y][x] = clearColor;
        }
    }
}

void Layer::SetVisible(bool isVisible) {
    visible = isVisible;
}

bool Layer::IsVisible() const {
    return visible;
}

void Layer::SetOpacity(int value) {
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    opacity = value;
}

int Layer::GetOpacity() const {
    return opacity;
}

void Layer::SetName(const std::string& newName) {
    name = newName;
}

std::string Layer::GetName() const {
    return name;
}

int Layer::GetWidth() const {
    return width;
}

int Layer::GetHeight() const {
    return height;
}

void Layer::Draw(HDC hdc, int offsetX, int offsetY, int zoom) const {
    if (!visible) return;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF color = pixels[y][x];
            if (color != TRANSPARENT) {
                // 应用透明度
                if (opacity < 255) {
                    COLORREF bgColor = GetPixelColor(hdc, offsetX + x * zoom, offsetY + y * zoom);
                    color = RGB(
                        (GetRValue(color) * opacity + GetRValue(bgColor) * (255 - opacity)) / 255,
                        (GetGValue(color) * opacity + GetGValue(bgColor) * (255 - opacity)) / 255,
                        (GetBValue(color) * opacity + GetBValue(bgColor) * (255 - opacity)) / 255
                    );
                }
                
                // 绘制像素
                for (int dy = 0; dy < zoom; dy++) {
                    for (int dx = 0; dx < zoom; dx++) {
                        SetPixel(hdc, offsetX + x * zoom + dx, offsetY + y * zoom + dy, color);
                    }
                }
            }
        }
    }
}

LayerManager::LayerManager() : activeLayerIndex(0) {
    // 添加默认图层
    AddLayer("Background");
}

LayerManager::~LayerManager() {
    for (Layer* layer : layers) {
        delete layer;
    }
    layers.clear();
}

int LayerManager::AddLayer(const std::string& name) {
    Layer* newLayer = new Layer(name);
    layers.push_back(newLayer);
    activeLayerIndex = layers.size() - 1;
    return activeLayerIndex;
}

void LayerManager::RemoveLayer(int index) {
    if (index >= 0 && index < layers.size() && layers.size() > 1) {
        delete layers[index];
        layers.erase(layers.begin() + index);
        if (activeLayerIndex >= layers.size()) {
            activeLayerIndex = layers.size() - 1;
        }
    }
}

void LayerManager::MoveLayerUp(int index) {
    if (index > 0 && index < layers.size()) {
        std::swap(layers[index], layers[index - 1]);
        if (activeLayerIndex == index) {
            activeLayerIndex--;
        } else if (activeLayerIndex == index - 1) {
            activeLayerIndex++;
        }
    }
}

void LayerManager::MoveLayerDown(int index) {
    if (index >= 0 && index < layers.size() - 1) {
        std::swap(layers[index], layers[index + 1]);
        if (activeLayerIndex == index) {
            activeLayerIndex++;
        } else if (activeLayerIndex == index + 1) {
            activeLayerIndex--;
        }
    }
}

void LayerManager::SetActiveLayer(int index) {
    if (index >= 0 && index < layers.size()) {
        activeLayerIndex = index;
    }
}

int LayerManager::GetActiveLayerIndex() const {
    return activeLayerIndex;
}

Layer* LayerManager::GetActiveLayer() {
    if (activeLayerIndex >= 0 && activeLayerIndex < layers.size()) {
        return layers[activeLayerIndex];
    }
    return nullptr;
}

const Layer* LayerManager::GetActiveLayer() const {
    if (activeLayerIndex >= 0 && activeLayerIndex < layers.size()) {
        return layers[activeLayerIndex];
    }
    return nullptr;
}

Layer* LayerManager::GetLayer(int index) {
    if (index >= 0 && index < layers.size()) {
        return layers[index];
    }
    return nullptr;
}

const Layer* LayerManager::GetLayer(int index) const {
    if (index >= 0 && index < layers.size()) {
        return layers[index];
    }
    return nullptr;
}

int LayerManager::GetLayerCount() const {
    return layers.size();
}

void LayerManager::DrawAll(HDC hdc, int offsetX, int offsetY, int zoom) const {
    for (const Layer* layer : layers) {
        layer->Draw(hdc, offsetX, offsetY, zoom);
    }
}