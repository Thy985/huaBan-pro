#ifndef PLUGIN_H
#define PLUGIN_H

#include "huaBan.h"
#include "canvas.h"
#include "tools.h"

class Plugin {
public:
    virtual ~Plugin() = default;
    
    // 插件信息
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::string GetVersion() const = 0;
    
    // 初始化和清理
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // 工具相关
    virtual int GetToolCount() const { return 0; }
    virtual std::string GetToolName(int index) const { return ""; }
    virtual void OnToolSelected(int index) {};
    virtual void OnToolUsed(Canvas& canvas, int x, int y) {};
    
    // 滤镜相关
    virtual int GetFilterCount() const { return 0; }
    virtual std::string GetFilterName(int index) const { return ""; }
    virtual void ApplyFilter(Canvas& canvas, int index) {};
    
    // 菜单相关
    virtual int GetMenuCount() const { return 0; }
    virtual std::string GetMenuName(int index) const { return ""; }
    virtual void OnMenuSelected(int index) {};
};

// 插件工厂函数类型
typedef Plugin* (*PluginCreateFunc)();
typedef void (*PluginDestroyFunc)(Plugin*);

#endif // PLUGIN_H