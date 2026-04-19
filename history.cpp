#include "history.h"

History::History(int maxSize) 
    : maxHistory(maxSize) {
}

void History::SaveState(const Canvas& canvas) {
    HistoryState state;
    canvas.SaveStateToHistory(state);
    
    undoStack.push_back(state);
    
    if (undoStack.size() > maxHistory) {
        undoStack.erase(undoStack.begin());
    }
    
    redoStack.clear();
}

void History::Undo(Canvas& canvas) {
    if (!CanUndo()) return;
    
    HistoryState currentState;
    canvas.SaveStateToHistory(currentState);
    redoStack.push_back(currentState);
    
    HistoryState state = undoStack.back();
    undoStack.pop_back();
    canvas.LoadStateFromHistory(state);
}

void History::Redo(Canvas& canvas) {
    if (!CanRedo()) return;
    
    HistoryState currentState;
    canvas.SaveStateToHistory(currentState);
    undoStack.push_back(currentState);
    
    HistoryState state = redoStack.back();
    redoStack.pop_back();
    canvas.LoadStateFromHistory(state);
}

void History::Clear() {
    undoStack.clear();
    redoStack.clear();
}
