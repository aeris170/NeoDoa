#pragma once

#include <vector>
#include <memory>

struct UndoRedoStack;

struct ICommand {
    virtual ~ICommand() noexcept = 0;
    virtual void Execute() noexcept = 0;
    virtual void UnExecute() noexcept = 0;

    virtual bool TryMergeWith(UndoRedoStack& history, const ICommand* command) noexcept;
};

struct UndoRedoStack {
    void Do(std::unique_ptr<ICommand>&& command) noexcept;
    void Undo() noexcept;
    void Redo() noexcept;
    void Clear() noexcept;

    bool CanUndo() const noexcept;
    bool CanRedo() const noexcept;

    const ICommand& PeekUndoStack() const noexcept;
    const ICommand& PeekRedoStack() const noexcept;
    void PopUndoStack() noexcept;
    void PopRedoStack() noexcept;

private:
    std::vector<std::unique_ptr<ICommand>> undoStack{};
    std::vector<std::unique_ptr<ICommand>> redoStack{};
};