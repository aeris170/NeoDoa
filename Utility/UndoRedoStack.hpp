#pragma once

#include <vector>
#include <memory>

struct ICommand {
    virtual ~ICommand() noexcept = 0;
    virtual void Execute() noexcept = 0;
    virtual void UnExecute() noexcept = 0;
};

struct UndoRedoStack {
    void Do(std::unique_ptr<ICommand>&& command) noexcept;
    void Undo() noexcept;
    void Redo() noexcept;

    bool CanUndo() const noexcept;
    bool CanRedo() const noexcept;

    const ICommand& PeekUndoStack() const noexcept;
    const ICommand& PeekRedoStack() const noexcept;

private:
    std::vector<std::unique_ptr<ICommand>> undoStack{};
    std::vector<std::unique_ptr<ICommand>> redoStack{};
};