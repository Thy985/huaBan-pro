#include "filter.h"
#include <cmath>

// 亮度调整
void Filter::AdjustBrightness(Canvas& canvas, int value) {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = canvas.GetPixel(x, y);
            COLORREF newColor = AdjustColorBrightness(color, value);
            canvas.SetPixel(x, y, newColor);
        }
    }
}

// 对比度调整
void Filter::AdjustContrast(Canvas& canvas, int value) {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = canvas.GetPixel(x, y);
            COLORREF newColor = AdjustColorContrast(color, value);
            canvas.SetPixel(x, y, newColor);
        }
    }
}

// 模糊效果
void Filter::ApplyBlur(Canvas& canvas, int radius) {
    // 创建临时数组存储原始像素值
    COLORREF temp[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            temp[x][y] = canvas.GetPixel(x, y);
        }
    }
    
    // 应用模糊效果
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF blurredColor = GetBlurredPixel(canvas, x, y, radius);
            canvas.SetPixel(x, y, blurredColor);
        }
    }
}

// 锐化效果
void Filter::ApplySharpen(Canvas& canvas) {
    // 创建临时数组存储原始像素值
    COLORREF temp[GRID_NUM][GRID_NUM];
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            temp[x][y] = canvas.GetPixel(x, y);
        }
    }
    
    // 锐化卷积核
    int kernel[3][3] = {
        { 0, -1, 0 },
        { -1, 5, -1 },
        { 0, -1, 0 }
    };
    
    // 应用锐化效果
    for (int x = 1; x < GRID_NUM - 1; x++) {
        for (int y = 1; y < GRID_NUM - 1; y++) {
            int r = 0, g = 0, b = 0;
            
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    COLORREF color = temp[x + i][y + j];
                    r += GetRValue(color) * kernel[i + 1][j + 1];
                    g += GetGValue(color) * kernel[i + 1][j + 1];
                    b += GetBValue(color) * kernel[i + 1][j + 1];
                }
            }
            
            // 确保颜色值在有效范围内
            r = std::min(255, std::max(0, r));
            g = std::min(255, std::max(0, g));
            b = std::min(255, std::max(0, b));
            
            canvas.SetPixel(x, y, RGB(r, g, b));
        }
    }
}

// 灰度效果
void Filter::ApplyGrayscale(Canvas& canvas) {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = canvas.GetPixel(x, y);
            int r = GetRValue(color);
            int g = GetGValue(color);
            int b = GetBValue(color);
            
            // 计算灰度值
            int gray = (r * 0.299 + g * 0.587 + b * 0.114);
            
            canvas.SetPixel(x, y, RGB(gray, gray, gray));
        }
    }
}

// 反色效果
void Filter::ApplyInvert(Canvas& canvas) {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            COLORREF color = canvas.GetPixel(x, y);
            int r = 255 - GetRValue(color);
            int g = 255 - GetGValue(color);
            int b = 255 - GetBValue(color);
            
            canvas.SetPixel(x, y, RGB(r, g, b));
        }
    }
}

// 辅助方法：调整颜色亮度
COLORREF Filter::AdjustColorBrightness(COLORREF color, int value) {
    float factor = 1.0f + value / 100.0f;
    int r = std::min(255, std::max(0, (int)(GetRValue(color) * factor)));
    int g = std::min(255, std::max(0, (int)(GetGValue(color) * factor)));
    int b = std::min(255, std::max(0, (int)(GetBValue(color) * factor)));
    return RGB(r, g, b);
}

// 辅助方法：调整颜色对比度
COLORREF Filter::AdjustColorContrast(COLORREF color, int value) {
    float factor = 1.0f + value / 100.0f;
    int r = std::min(255, std::max(0, (int)((GetRValue(color) - 128) * factor + 128)));
    int g = std::min(255, std::max(0, (int)((GetGValue(color) - 128) * factor + 128)));
    int b = std::min(255, std::max(0, (int)((GetBValue(color) - 128) * factor + 128)));
    return RGB(r, g, b);
}

// 辅助方法：获取模糊后的像素值
COLORREF Filter::GetBlurredPixel(Canvas& canvas, int x, int y, int radius) {
    int r = 0, g = 0, b = 0;
    int count = 0;
    
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < GRID_NUM && ny >= 0 && ny < GRID_NUM) {
                COLORREF color = canvas.GetPixel(nx, ny);
                r += GetRValue(color);
                g += GetGValue(color);
                b += GetBValue(color);
                count++;
            }
        }
    }
    
    if (count > 0) {
        r /= count;
        g /= count;
        b /= count;
    }
    
    return RGB(r, g, b);
}