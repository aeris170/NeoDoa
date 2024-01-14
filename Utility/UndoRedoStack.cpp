#include <Utility/UndoRedoStack.hpp>

#include <cassert>

ICommand::~ICommand() noexcept {};

void UndoRedoStack::Do(std::unique_ptr<ICommand>&& command) noexcept {
    command->Execute();
    undoStack.push_back(std::move(command));
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