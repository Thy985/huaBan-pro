#ifndef FILTER_H
#define FILTER_H

#include "huaBan.h"
#include "canvas.h"

class Filter {
public:
    // 亮度调整
    static void AdjustBrightness(Canvas& canvas, int value); // -100 到 100
    
    // 对比度调整
    static void AdjustContrast(Canvas& canvas, int value); // -100 到 100
    
    // 模糊效果
    static void ApplyBlur(Canvas& canvas, int radius); // 1 到 5
    
    // 锐化效果
    static void ApplySharpen(Canvas& canvas);
    
    // 灰度效果
    static void ApplyGrayscale(Canvas& canvas);
    
    // 反色效果
    static void ApplyInvert(Canvas& canvas);
    
private:
    // 辅助方法
    static COLORREF AdjustColorBrightness(COLORREF color, int value);
    static COLORREF AdjustColorContrast(COLORREF color, int value);
    static COLORREF GetBlurredPixel(Canvas& canvas, int x, int y, int radius);
};

#endif // FILTER_H