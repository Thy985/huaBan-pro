#ifndef ANIMATION_H
#define ANIMATION_H

#include "huaBan.h"
#include "canvas.h"

class AnimationFrame {
private:
    COLORREF pixels[GRID_NUM][GRID_NUM];
    int duration; // 帧持续时间（毫秒）
    std::string name;

public:
    AnimationFrame(const std::string& frameName = "Frame");
    
    // 基本操作
    void SetPixel(int x, int y, COLORREF color);
    COLORREF GetPixel(int x, int y) const;
    void Clear(COLORREF clearColor = BLACK);
    
    // 从画布复制
    void CopyFrom(const Canvas& canvas);
    void CopyTo(Canvas& canvas) const;
    
    // 属性
    void SetDuration(int ms);
    int GetDuration() const;
    
    void SetName(const std::string& newName);
    std::string GetName() const;
};

class Animation {
private:
    std::vector<AnimationFrame*> frames;
    int currentFrameIndex;
    int loopCount; // 0 表示无限循环
    
public:
    Animation();
    ~Animation();
    
    // 帧管理
    int AddFrame(const std::string& name = "Frame");
    void RemoveFrame(int index);
    void DuplicateFrame(int index);
    
    // 帧访问
    AnimationFrame* GetFrame(int index);
    const AnimationFrame* GetFrame(int index) const;
    int GetFrameCount() const;
    
    // 当前帧
    void SetCurrentFrame(int index);
    int GetCurrentFrameIndex() const;
    AnimationFrame* GetCurrentFrame();
    const AnimationFrame* GetCurrentFrame() const;
    
    // 动画属性
    void SetLoopCount(int count);
    int GetLoopCount() const;
    
    // 播放控制
    void NextFrame();
    void PreviousFrame();
    
    // 导出
    bool ExportToGIF(const std::string& filename);
};

#endif // ANIMATION_H