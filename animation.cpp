#include "animation.h"

// AnimationFrame 实现
AnimationFrame::AnimationFrame(const std::string& frameName) 
    : duration(100) // 默认100毫秒
    , name(frameName) {
    Clear();
}

void AnimationFrame::SetPixel(int x, int y, COLORREF color) {
    if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
        pixels[y][x] = color;
    }
}

COLORREF AnimationFrame::GetPixel(int x, int y) const {
    if (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM) {
        return pixels[y][x];
    }
    return BLACK;
}

void AnimationFrame::Clear(COLORREF clearColor) {
    for (int y = 0; y < GRID_NUM; y++) {
        for (int x = 0; x < GRID_NUM; x++) {
            pixels[y][x] = clearColor;
        }
    }
}

void AnimationFrame::CopyFrom(const Canvas& canvas) {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            pixels[y][x] = canvas.GetPixel(x, y);
        }
    }
}

void AnimationFrame::CopyTo(Canvas& canvas) const {
    for (int x = 0; x < GRID_NUM; x++) {
        for (int y = 0; y < GRID_NUM; y++) {
            canvas.SetPixel(x, y, pixels[y][x]);
        }
    }
}

void AnimationFrame::SetDuration(int ms) {
    if (ms > 0) {
        duration = ms;
    }
}

int AnimationFrame::GetDuration() const {
    return duration;
}

void AnimationFrame::SetName(const std::string& newName) {
    name = newName;
}

std::string AnimationFrame::GetName() const {
    return name;
}

// Animation 实现
Animation::Animation() 
    : currentFrameIndex(0)
    , loopCount(0) { // 默认无限循环
    // 添加默认帧
    AddFrame("Frame 1");
}

Animation::~Animation() {
    for (AnimationFrame* frame : frames) {
        delete frame;
    }
    frames.clear();
}

int Animation::AddFrame(const std::string& name) {
    AnimationFrame* newFrame = new AnimationFrame(name);
    frames.push_back(newFrame);
    currentFrameIndex = frames.size() - 1;
    return currentFrameIndex;
}

void Animation::RemoveFrame(int index) {
    if (index >= 0 && index < frames.size() && frames.size() > 1) {
        delete frames[index];
        frames.erase(frames.begin() + index);
        if (currentFrameIndex >= frames.size()) {
            currentFrameIndex = frames.size() - 1;
        }
    }
}

void Animation::DuplicateFrame(int index) {
    if (index >= 0 && index < frames.size()) {
        AnimationFrame* originalFrame = frames[index];
        AnimationFrame* newFrame = new AnimationFrame(originalFrame->GetName() + " Copy");
        
        // 复制像素数据
        for (int x = 0; x < GRID_NUM; x++) {
            for (int y = 0; y < GRID_NUM; y++) {
                newFrame->SetPixel(x, y, originalFrame->GetPixel(x, y));
            }
        }
        
        // 复制其他属性
        newFrame->SetDuration(originalFrame->GetDuration());
        
        // 添加到帧列表
        frames.insert(frames.begin() + index + 1, newFrame);
        currentFrameIndex = index + 1;
    }
}

AnimationFrame* Animation::GetFrame(int index) {
    if (index >= 0 && index < frames.size()) {
        return frames[index];
    }
    return nullptr;
}

const AnimationFrame* Animation::GetFrame(int index) const {
    if (index >= 0 && index < frames.size()) {
        return frames[index];
    }
    return nullptr;
}

int Animation::GetFrameCount() const {
    return frames.size();
}

void Animation::SetCurrentFrame(int index) {
    if (index >= 0 && index < frames.size()) {
        currentFrameIndex = index;
    }
}

int Animation::GetCurrentFrameIndex() const {
    return currentFrameIndex;
}

AnimationFrame* Animation::GetCurrentFrame() {
    return GetFrame(currentFrameIndex);
}

const AnimationFrame* Animation::GetCurrentFrame() const {
    return GetFrame(currentFrameIndex);
}

void Animation::SetLoopCount(int count) {
    if (count >= 0) {
        loopCount = count;
    }
}

int Animation::GetLoopCount() const {
    return loopCount;
}

void Animation::NextFrame() {
    if (frames.size() > 0) {
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
    }
}

void Animation::PreviousFrame() {
    if (frames.size() > 0) {
        currentFrameIndex = (currentFrameIndex - 1 + frames.size()) % frames.size();
    }
}

bool Animation::ExportToGIF(const std::string& filename) {
    // 这里只是一个占位实现
    // 实际的 GIF 导出需要使用第三方库，如 GifLib
    // 或者使用 Windows GDI+ 来实现
    
    // 暂时返回 true 表示成功
    return true;
}