#include "history.h"

History::History(int maxSize) 
    : maxHistory(maxSize) {
}

void History::SaveState(const Canvas& canvas) {
    HistoryState state;
    canvas.CopyTo(state.data);
    
    undoStack.push_back(state);
    
    if (undoStack.size() > maxHistory) {
        undoStack.erase(undoStack.begin());
    }
    
    redoStack.clear();
}

void History::Undo(Canvas& canvas) {
    if (!CanUndo()) return;
    
    HistoryState currentState;
    canvas.CopyTo(currentState.data);
    redoStack.push_back(currentState);
    
    HistoryState state = undoStack.back();
    undoStack.pop_back();
    canvas.CopyFrom(state.data);
}

void History::Redo(Canvas& canvas) {
    if (!CanRedo()) return;
    
    HistoryState currentState;
    canvas.CopyTo(currentState.data);
    undoStack.push_back(currentState);
    
    HistoryState state = redoStack.back();
    redoStack.pop_back();
    canvas.CopyFrom(state.data);
}

void History::Clear() {
    undoStack.clear();
    redoStack.clear();
}
