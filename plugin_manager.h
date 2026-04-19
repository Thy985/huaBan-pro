#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "plugin.h"
#include <vector>
#include <string>

class PluginManager {
private:
    std::vector<Plugin*> plugins;
    std::vector<void*> pluginHandles; // 用于存储动态库句柄
    
public:
    ~PluginManager();
    
    // 插件管理
    bool LoadPlugin(const std::string& pluginPath);
    void UnloadAllPlugins();
    
    // 插件访问
    int GetPluginCount() const;
    Plugin* GetPlugin(int index);
    const Plugin* GetPlugin(int index) const;
    
    // 工具相关
    int GetTotalToolCount() const;
    std::string GetToolName(int pluginIndex, int toolIndex) const;
    void OnToolSelected(int pluginIndex, int toolIndex);
    void OnToolUsed(int pluginIndex, int toolIndex, Canvas& canvas, int x, int y);
    
    // 滤镜相关
    int GetTotalFilterCount() const;
    std::string GetFilterName(int pluginIndex, int filterIndex) const;
    void ApplyFilter(int pluginIndex, int filterIndex, Canvas& canvas);
    
    // 菜单相关
    int GetTotalMenuCount() const;
    std::string GetMenuName(int pluginIndex, int menuIndex) const;
    void OnMenuSelected(int pluginIndex, int menuIndex);
};

#endif // PLUGIN_MANAGER_H