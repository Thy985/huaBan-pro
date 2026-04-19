#pragma once
#include "huaBan.h"
#include "canvas.h"
#include <cmath>

class Tools {
private:
    ToolType currentTool;
    COLORREF currentColor;
    int brushSize;
    
    void FloodFillInternal(Canvas& canvas, int x, int y, COLORREF targetColor, COLORREF replaceColor);

public:
    Tools();
    
    void SetTool(ToolType tool) { currentTool = tool; }
    ToolType GetTool() const { return currentTool; }
    
    void SetColor(COLORREF color) { currentColor = color; }
    COLORREF GetColor() const { return currentColor; }
    
    void SetBrushSize(int size) { brushSize = size; }
    int GetBrushSize() const { return brushSize; }
    
    void Draw(Canvas& canvas, int gridX, int gridY);
    void Erase(Canvas& canvas, int gridX, int gridY);
    void Bucket(Canvas& canvas, int gridX, int gridY);
    
    void Apply(Canvas& canvas, int gridX, int gridY);
};
