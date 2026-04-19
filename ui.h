#pragma once
#include "huaBan.h"
#include "canvas.h"
#include "tools.h"
#include <vector>

class UI {
private:
    std::vector<Button> buttons;
    COLORREF palette[16];
    const wchar_t* paletteNames[16];
    COLORREF recentColors[8];
    int recentColorCount;
    
    void InitButtons();
    void InitPalette();

public:
    UI();
    
    void Draw(const Canvas& canvas, const Tools& tools);
    void DrawButtons(const Tools& tools);
    void DrawPalette();
    void DrawInfo(const Canvas& canvas, const Tools& tools);
    
    bool IsButtonClicked(const Button& btn, int x, int y) const;
    Button* FindButton(int x, int y);
    int FindPaletteColor(int x, int y);
    
    void AddRecentColor(COLORREF color);
    
    const std::vector<Button>& GetButtons() const { return buttons; }
};
