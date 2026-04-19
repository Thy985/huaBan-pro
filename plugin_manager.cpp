#include "plugin_manager.h"

PluginManager::~PluginManager() {
    UnloadAllPlugins();
}

bool PluginManager::LoadPlugin(const std::string& pluginPath) {
    // 这里只是一个占位实现
    // 实际的插件加载需要使用动态库加载API，如 LoadLibrary on Windows, dlopen on Linux
    
    // 暂时返回 false 表示加载失败
    return false;
}

void PluginManager::UnloadAllPlugins() {
    for (size_t i = 0; i < plugins.size(); i++) {
        plugins[i]->Shutdown();
        delete plugins[i];
    }
    plugins.clear();
    
    // 释放动态库句柄
    for (void* handle : pluginHandles) {
        // 实际的释放需要使用 FreeLibrary on Windows, dlclose on Linux
    }
    pluginHandles.clear();
}

int PluginManager::GetPluginCount() const {
    return plugins.size();
}

Plugin* PluginManager::GetPlugin(int index) {
    if (index >= 0 && index < plugins.size()) {
        return plugins[index];
    }
    return nullptr;
}

const Plugin* PluginManager::GetPlugin(int index) const {
    if (index >= 0 && index < plugins.size()) {
        return plugins[index];
    }
    return nullptr;
}

int PluginManager::GetTotalToolCount() const {
    int count = 0;
    for (const Plugin* plugin : plugins) {
        count += plugin->GetToolCount();
    }
    return count;
}

std::string PluginManager::GetToolName(int pluginIndex, int toolIndex) const {
    const Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        return plugin->GetToolName(toolIndex);
    }
    return "";
}

void PluginManager::OnToolSelected(int pluginIndex, int toolIndex) {
    Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        plugin->OnToolSelected(toolIndex);
    }
}

void PluginManager::OnToolUsed(int pluginIndex, int toolIndex, Canvas& canvas, int x, int y) {
    Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        plugin->OnToolUsed(canvas, x, y);
    }
}

int PluginManager::GetTotalFilterCount() const {
    int count = 0;
    for (const Plugin* plugin : plugins) {
        count += plugin->GetFilterCount();
    }
    return count;
}

std::string PluginManager::GetFilterName(int pluginIndex, int filterIndex) const {
    const Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        return plugin->GetFilterName(filterIndex);
    }
    return "";
}

void PluginManager::ApplyFilter(int pluginIndex, int filterIndex, Canvas& canvas) {
    Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        plugin->ApplyFilter(canvas, filterIndex);
    }
}

int PluginManager::GetTotalMenuCount() const {
    int count = 0;
    for (const Plugin* plugin : plugins) {
        count += plugin->GetMenuCount();
    }
    return count;
}

std::string PluginManager::GetMenuName(int pluginIndex, int menuIndex) const {
    const Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        return plugin->GetMenuName(menuIndex);
    }
    return "";
}

void PluginManager::OnMenuSelected(int pluginIndex, int menuIndex) {
    Plugin* plugin = GetPlugin(pluginIndex);
    if (plugin) {
        plugin->OnMenuSelected(menuIndex);
    }
}
