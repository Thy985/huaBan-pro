#pragma once
#include "huaBan.h"
#include "canvas.h"
#include <vector>

class History {
private:
    std::vector<HistoryState> undoStack;
    std::vector<HistoryState> redoStack;
    int maxHistory;

public:
    History(int maxSize = MAX_HISTORY);
    
    void SaveState(const Canvas& canvas);
    bool CanUndo() const { return !undoStack.empty(); }
    bool CanRedo() const { return !redoStack.empty(); }
    
    void Undo(Canvas& canvas);
    void Redo(Canvas& canvas);
    
    void Clear();
};
