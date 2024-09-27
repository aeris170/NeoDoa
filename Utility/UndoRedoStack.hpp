#pragma once

#include <array>
#include <vector>
#include <memory>
#include <utility>

struct UndoRedoStack;
struct UndoRedoStackIterator;

struct ICommand {
    virtual ~ICommand() noexcept = 0;
    virtual void Execute() noexcept = 0;
    virtual void UnExecute() noexcept = 0;

    virtual bool TryMergeWith([[maybe_unused]] UndoRedoStack& history, [[maybe_unused]] const ICommand* command) noexcept;
};

struct UndoRedoStack {
    using Command = std::unique_ptr<ICommand>;

    void Do(Command&& command) noexcept;
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
    using DataStructure = std::vector<Command>;
    DataStructure undoStack{};
    DataStructure redoStack{};

    // Iterators API:
    // Provides support for forward-reverse iteration
    // of either undo-redo or both stacks.
public:
    struct Iterator {
        enum class ElementSource { UndoStack, RedoStack };
        using Element = std::pair<ICommand&, ElementSource>;

        virtual ~Iterator() noexcept = 0;
        virtual bool HasNext() const noexcept = 0;
        virtual Element Next() noexcept = 0;
    };
    // UndoStackIterator
    struct UndoStackIterator : Iterator {
        const ElementSource ElementSource{ ElementSource::UndoStack };

        explicit UndoStackIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        const UndoRedoStack& stack;
    };
    // UndoStackIterator
    // UndoStackReverseIterator
    struct UndoStackReverseIterator : Iterator {
        const ElementSource ElementSource{ ElementSource::UndoStack };

        explicit UndoStackReverseIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        const UndoRedoStack& stack;
    };
    // UndoStackReverseIterator
    // RedoStackIterator
    struct RedoStackIterator : Iterator {
        const ElementSource ElementSource{ ElementSource::RedoStack };

        explicit RedoStackIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        const UndoRedoStack& stack;
    };
    // RedoStackIterator
    // RedoStackReverseIterator
    struct RedoStackReverseIterator : Iterator {
        const ElementSource ElementSource{ ElementSource::RedoStack };

        explicit RedoStackReverseIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        const UndoRedoStack& stack;
    };
    // RedoStackReverseIterator
    // UndoRedoStackIterator
    struct UndoRedoStackIterator : Iterator {
        explicit UndoRedoStackIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        std::array<std::unique_ptr<Iterator>, 2> iterators;
    };
    // UndoRedoStackIterator
    // UndoRedoStackReverseIterator
    struct UndoRedoStackReverseIterator : Iterator {
        explicit UndoRedoStackReverseIterator(const UndoRedoStack& stack) noexcept;

        bool HasNext() const noexcept override;
        Element Next() noexcept override;
    private:
        size_t currentIndex{};
        std::array<std::unique_ptr<Iterator>, 2> iterators;
    };
    // UndoRedoStackReverseIterator
private:
    friend struct UndoStackIterator;
    friend struct UndoStackReverseIterator;
    friend struct RedoStackIterator;
    friend struct RedoStackReverseIterator;
    friend struct UndoRedoStackIterator;
    friend struct UndoRedoStackReverseIterator;
};