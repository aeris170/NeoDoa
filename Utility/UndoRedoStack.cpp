#include <Utility/UndoRedoStack.hpp>

#include <cassert>

ICommand::~ICommand() noexcept {};

bool ICommand::TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept { return false; }

void UndoRedoStack::Do(std::unique_ptr<ICommand>&& command) noexcept {
    if (!undoStack.empty() && undoStack.back()->TryMergeWith(*this, command.get())) {
        // Can Merge, don't push command but merge it with peek and re-execute peek (instead of command)
        undoStack.back()->Execute();
    } else {
        // Can't Merge, push command and execute it
        command->Execute();
        undoStack.push_back(std::move(command));
    }

    redoStack.clear();
}
void UndoRedoStack::Undo() noexcept {
    assert(CanUndo());
    auto command{ std::move(undoStack.back()) };
    undoStack.pop_back();

    command->UnExecute();
    redoStack.push_back(std::move(command));
}
void UndoRedoStack::Redo() noexcept {
    assert(CanRedo());
    auto command{ std::move(redoStack.back()) };
    redoStack.pop_back();

    command->Execute();
    undoStack.push_back(std::move(command));
}
void UndoRedoStack::Clear() noexcept {
    undoStack.clear();
    redoStack.clear();
}

bool UndoRedoStack::CanUndo() const noexcept { return !undoStack.empty(); }
bool UndoRedoStack::CanRedo() const noexcept { return !redoStack.empty(); }

const ICommand& UndoRedoStack::PeekUndoStack() const noexcept {
    assert(CanUndo());
    return *undoStack.back().get();
}
const ICommand& UndoRedoStack::PeekRedoStack() const noexcept {
    assert(CanRedo());
    return *redoStack.back().get();
}
void UndoRedoStack::PopUndoStack() noexcept { undoStack.pop_back(); }
void UndoRedoStack::PopRedoStack() noexcept { redoStack.pop_back(); }