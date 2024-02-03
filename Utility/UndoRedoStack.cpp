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

// Iterators API:
UndoRedoStack::Iterator::~Iterator() noexcept {};

// UndoStackIterator
UndoRedoStack::UndoStackIterator::UndoStackIterator(const UndoRedoStack& stack) noexcept :
    stack(stack) {}

bool UndoRedoStack::UndoStackIterator::HasNext() const noexcept {
    return currentIndex < stack.undoStack.size() && !stack.undoStack.empty();
}
UndoRedoStack::Iterator::Element UndoRedoStack::UndoStackIterator::Next() noexcept {
    assert(HasNext());

    const UndoRedoStack::Command* uptr = &stack.undoStack[currentIndex];
    currentIndex++;
    return { *uptr->get(), ElementSource };
}
// UndoStackIterator
// UndoStackReverseIterator
UndoRedoStack::UndoStackReverseIterator::UndoStackReverseIterator(const UndoRedoStack& stack) noexcept :
    currentIndex(stack.undoStack.size() - 1),
    stack(stack) {}

bool UndoRedoStack::UndoStackReverseIterator::HasNext() const noexcept {
    return currentIndex >= 0 && !stack.undoStack.empty();
}
UndoRedoStack::Iterator::Element UndoRedoStack::UndoStackReverseIterator::Next() noexcept {
    assert(HasNext());

    const UndoRedoStack::Command* uptr = &stack.undoStack[currentIndex];
    currentIndex--;
    return { *uptr->get(), ElementSource };
}
// UndoStackReverseIterator
// RedoStackIterator
UndoRedoStack::RedoStackIterator::RedoStackIterator(const UndoRedoStack& stack) noexcept :
    stack(stack) {}

bool UndoRedoStack::RedoStackIterator::HasNext() const noexcept {
    return currentIndex < stack.redoStack.size() && !stack.redoStack.empty();
}
UndoRedoStack::Iterator::Element UndoRedoStack::RedoStackIterator::Next() noexcept {
    assert(HasNext());

    const UndoRedoStack::Command* uptr = &stack.redoStack[currentIndex];
    currentIndex++;
    return { *uptr->get(), ElementSource };
}
// RedoStackIterator
// RedoStackReverseIterator
UndoRedoStack::RedoStackReverseIterator::RedoStackReverseIterator(const UndoRedoStack& stack) noexcept :
    currentIndex(stack.redoStack.size() - 1),
    stack(stack) {}

bool UndoRedoStack::RedoStackReverseIterator::HasNext() const noexcept {
    return currentIndex >= 0 && !stack.redoStack.empty();
}
UndoRedoStack::Iterator::Element UndoRedoStack::RedoStackReverseIterator::Next() noexcept {
    assert(HasNext());

    const UndoRedoStack::Command* uptr = &stack.redoStack[currentIndex];
    currentIndex--;
    return { *uptr->get(), ElementSource };
}
// RedoStackReverseIterator
// UndoRedoStackIterator
UndoRedoStack::UndoRedoStackIterator::UndoRedoStackIterator(const UndoRedoStack& stack) noexcept {
    iterators[0] = std::make_unique<UndoStackIterator>(stack);
    iterators[1] = std::make_unique<RedoStackReverseIterator>(stack);
    while (!iterators[currentIndex]->HasNext()) { currentIndex++; }
}

bool UndoRedoStack::UndoRedoStackIterator::HasNext() const noexcept {
    return currentIndex < iterators.size();
}
UndoRedoStack::Iterator::Element UndoRedoStack::UndoRedoStackIterator::Next() noexcept {
    assert(HasNext()); // Don't call Next() before checking HasNext()

    const auto& it = iterators[currentIndex];
    Element element = it->Next();
    while (!iterators[currentIndex]->HasNext()) { currentIndex++; }
    return element;
}
// UndoRedoStackIterator
// UndoRedoStackReverseIterator
UndoRedoStack::UndoRedoStackReverseIterator::UndoRedoStackReverseIterator(const UndoRedoStack& stack) noexcept {
    iterators[0] = std::make_unique<RedoStackIterator>(stack);
    iterators[1] = std::make_unique<UndoStackReverseIterator>(stack);
    while (!iterators[currentIndex]->HasNext()) { currentIndex++; }
}

bool UndoRedoStack::UndoRedoStackReverseIterator::HasNext() const noexcept {
    return currentIndex < iterators.size();
}
UndoRedoStack::Iterator::Element UndoRedoStack::UndoRedoStackReverseIterator::Next() noexcept {
    assert(HasNext()); // Don't call Next() before checking HasNext()

    const auto& it = iterators[currentIndex];
    Element element = it->Next();
    while (!iterators[currentIndex]->HasNext()) { currentIndex++; }
    return element;
}
// UndoRedoStackReverseIterator