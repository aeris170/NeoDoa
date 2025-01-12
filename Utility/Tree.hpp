#pragma once

#include <queue>
#include <stack>
#include <ranges>
#include <vector>
#include <cassert>
#include <concepts>
#include <type_traits>

template<typename NodeData, size_t InitialNodeCapacity = 512, size_t InitialChildCapacity = 8>
struct Tree {

    using NodeIndex = size_t;
    static constexpr NodeIndex Root = 0;
    static constexpr NodeIndex Invalid = static_cast<NodeIndex>(-1);


    struct Node {
        NodeData Data;
        std::vector<NodeIndex> Children;
    };

    using DataStructure = std::vector<Node>;

    struct ChildrenList {
        struct NodeIterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = NodeData;
            using pointer = value_type*;
            using reference = value_type&;

            NodeIterator(Tree& tree, const std::vector<NodeIndex>& indices, size_t pos) :
                tree(tree), indices(indices), pos(pos) {}

            reference operator*() const { return tree.data[indices[pos]].Data; }
            pointer operator->() const { return &tree.data[indices[pos]].Data; }
            NodeIterator& operator++() { ++pos; return *this; }
            NodeIterator operator++(int) { NodeIterator tmp = *this; ++(*this); return tmp; }
            friend bool operator==(const NodeIterator& a, const NodeIterator& b) { return a.pos == b.pos; }

        private:
            Tree& tree;
            const std::vector<NodeIndex>& indices;
            size_t pos;
        };

        ChildrenList(Tree& tree, const std::vector<NodeIndex>& indices) noexcept :
            tree(tree), indices(indices) {};

        NodeData& operator[](std::size_t idx) { return tree.data[indices[idx]].Data; }
        const NodeData& operator[](std::size_t idx) const { return tree.data[indices[idx]].Data; }

        NodeIterator begin() { return NodeIterator(tree, indices, 0); }
        NodeIterator end()   { return NodeIterator(tree, indices, indices.size()); }

        size_t size() const { return indices.size(); }

    private:
        Tree& tree;
        const std::vector<NodeIndex>& indices;
    };

    Tree() noexcept requires(std::is_nothrow_default_constructible_v<NodeData>);
    explicit Tree(const NodeData& rootData) noexcept;

    /// <summary>
    /// Precondition: A node at parentIndex is present.
    /// Postcondition: A node under specified parent is present with data created using args.
    /// </summary>
    template<typename... Args>
        requires std::constructible_from<NodeData, Args...>
    NodeIndex EmplaceNode(const NodeIndex parentIndex, Args&&... args) noexcept;

    /// <summary>
    /// Precondition: A node at parentIndex is present.
    /// Postcondition: A node under specified parent is present with data nodeData (copy).
    /// </summary>
    NodeIndex InsertNode(const NodeIndex parentIndex, const NodeData& nodeData) noexcept;

    /// <summary>
    /// Precondition: A node at parentIndex is present.
    /// Postcondition: A node under specified parent is present with data nodeData (move).
    /// </summary>
    NodeIndex InsertNode(const NodeIndex parentIndex, NodeData&& nodeData) noexcept;

    /// <summary>
    /// Precondition: None.
    /// Postcondition: None.
    /// </summary>
    bool HasNodeAt(const NodeIndex index) const noexcept;

    /// <summary>
    /// Precondition: A node at index is present.
    /// Postcondition: None.
    /// </summary>
    NodeData& NodeAt(const NodeIndex index) noexcept;

    /// <summary>
    /// Precondition: A node at index is present.
    /// Postcondition: None.
    /// </summary>
    const NodeData& NodeAt(const NodeIndex index) const noexcept;

    /// <summary>
    /// Precondition: A node at index is present, index is not Root.
    /// Postcondition: None.
    /// </summary>
    NodeIndex FindParentOfNodeAt(const NodeIndex index) const noexcept;

    /// <summary>
    /// Precondition: A node at index is present.
    /// Postcondition: None.
    /// </summary>
    ChildrenList ChildrenOfNodeAt(const NodeIndex index) noexcept;

    /// <summary>
    /// Precondition: A node at index is present.
    /// Postcondition: None.
    /// </summary>
    ChildrenList ChildrenOfNodeAt(const NodeIndex index) const noexcept;

    /// <summary>
    /// Breadth First Searches for the first occurence of a node with supplied data on subtree with root with index parent.
    /// Returns the index of the first occurence or Tree<NodeData>::Invalid
    /// Precondition: NodeData is equality comparable.
    ///               A node at parent is present.
    /// Postcondition: None.
    /// </summary>
    NodeIndex FindNodeIndexBFS(const NodeData& data, const NodeIndex parent = Root) const noexcept requires(std::equality_comparable<NodeData>);

    /// <summary>
    /// Depth First Searches for the first occurence of a node with supplied data on subtree with root with index parent.
    /// Returns the index of the first occurence or Tree<NodeData>::Invalid
    /// Precondition: NodeData is equality comparable.
    ///               A node at parent is present.
    /// Postcondition: None.
    /// </summary>
    NodeIndex FindNodeIndexDFS(const NodeData& data, const NodeIndex parent = Root) const noexcept requires(std::equality_comparable<NodeData>);

    /// <summary>
    /// Removes the subtree with root with supplied index.
    /// Special case for index == Root, only Root node remains.
    /// Precondition: A node at index is present.
    /// Postcondition: Node at index and it's children aren't present.
    /// </summary>
    void DeleteNode(const NodeIndex index) noexcept;

private:
    DataStructure data;
};

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::Tree() noexcept requires(std::is_nothrow_default_constructible_v<NodeData>) {
    data.reserve(InitialNodeCapacity);
    data.emplace_back(NodeData{}, decltype(Node::Children){}).Children.reserve(InitialChildCapacity);
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::Tree(const NodeData& rootData) noexcept {
    data.reserve(InitialNodeCapacity);
    data.emplace_back(rootData, decltype(Node::Children){}).Children.reserve(InitialChildCapacity);
}

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
template<typename... Args>
    requires std::constructible_from<NodeData, Args...>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::EmplaceNode(const NodeIndex parentIndex, Args&&... args) noexcept -> NodeIndex {
    assert(parentIndex < data.size());
    data.emplace_back(std::forward<Args>(args)..., decltype(Node::Children){}).Children.reserve(InitialChildCapacity);
    data[parentIndex].Children.emplace_back(data.size() - 1);
    return data.size() - 1;
}

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::InsertNode(const NodeIndex parentIndex, const NodeData& nodeData) noexcept -> NodeIndex {
    assert(parentIndex < data.size());
    data.emplace_back(nodeData, decltype(Node::Children){}).Children.reserve(InitialChildCapacity);
    data[parentIndex].Children.emplace_back(data.size() - 1);
    return data.size() - 1;
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::InsertNode(const NodeIndex parentIndex, NodeData&& nodeData) noexcept -> NodeIndex {
    assert(parentIndex < data.size());
    data.emplace_back(std::move(nodeData), decltype(Node::Children){}).Children.reserve(InitialChildCapacity);
    data[parentIndex].Children.emplace_back(data.size() - 1);
    return data.size() - 1;
}

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::HasNodeAt(const NodeIndex index) const noexcept -> bool {
    return index < data.size();
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::NodeAt(const NodeIndex index) noexcept -> NodeData& {
    assert(index < data.size());
    return data[index].Data;
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::NodeAt(const NodeIndex index) const noexcept -> const NodeData& {
    assert(index < data.size());
    return data[index].Data;
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::FindParentOfNodeAt(const NodeIndex index) const noexcept -> NodeIndex {
    assert(index > Root); // Parent of root? Are you crazy?
    assert(index < data.size());

    for (int i = 0; i < data.size(); i++) {
        const Node& node = data[i];
        if (std::ranges::find(node.Children, index) != node.Children.end()) {
            return i;
        }
    }
    std::unreachable();
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::ChildrenOfNodeAt(const NodeIndex index) noexcept -> ChildrenList {
    assert(index < data.size());
    return ChildrenList(*this, data[index].Children);
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::ChildrenOfNodeAt(const NodeIndex index) const noexcept -> ChildrenList {
    assert(index < data.size());
    return ChildrenList(const_cast<Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>&>(*this), data[index].Children);
}

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::FindNodeIndexBFS(const NodeData& data, const NodeIndex parent) const noexcept -> NodeIndex requires(std::equality_comparable<NodeData>) {
    assert(parent < this->data.size());

    std::queue<NodeIndex> q;
    q.push(parent);

    while (!q.empty()) {
        NodeIndex nodeIndex = q.front();
        if (this->data[nodeIndex].Data == data) {
            return nodeIndex;
        }
        q.pop();

        for (const auto& childIndex : this->data[nodeIndex].Children) {
            q.push(childIndex);
        }
    }

    return Invalid;
}
template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline auto Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::FindNodeIndexDFS(const NodeData& data, const NodeIndex parent) const noexcept -> NodeIndex requires(std::equality_comparable<NodeData>) {
    assert(parent < this->data.size());

    std::stack<NodeIndex> s;
    s.push(parent);

    while (!s.empty()) {
        NodeIndex nodeIndex = s.top();
        if (this->data[nodeIndex].Data == data) {
            return nodeIndex;
        }
        s.pop();

        for (const auto& childIndex : this->data[nodeIndex].Children) {
            s.push(childIndex);
        }
    }

    return Invalid;
}

template<typename NodeData, size_t InitialNodeCapacity, size_t InitialChildCapacity>
inline void Tree<NodeData, InitialNodeCapacity, InitialChildCapacity>::DeleteNode(const NodeIndex index) noexcept {
    assert(index < data.size());

    if (index == Root) {
        auto root = std::move(data[0]);
        data.clear();
        data.emplace_back(std::move(root));
        return;
    }

    // Deletion is complicated. Because the underlying data structure
    // is a vector, any deletion will slide the indices of elements
    // after it. Here's the problems illustration:
    //
    //      ROOT -> [0], VN -> [N]
    //
    //              (ROOT)[0]
    //             /      \
    //            /        \
    //          (V1)[1]   (V2)[2]
    //          /  \         \
    //         /    \         \
    //        /      \         \
    //      (V3)[3] (V4)[4]   (V5)[5]
    //
    // Here, after deleting V1, V2 will receive index 1, V5 will receive
    // index 2. A deletion operation on a sibling will cause V2 to update
    // it's ChildrenList, as V5 is no longer at 5, it's at 2!
    //
    //              (ROOT)[0]
    //                 |
    //                 |
    //               (V2)[1]
    //                 |
    //                 |
    //               (V5)[2]
    //
    // Therefore, here's the algorithm:
    //  1. Recurse. We now have some form of invariancy.
    //  2. Erase yourself from tree.
    //  3. Fix indices. Traverse entire tree, for each node, traverse children
    //    3.1. if childIndex == N, erase (delete yourself from parent)
    //    3.2. if childIndex > N, decrement (fix yourself, someone got deleted)
    //

    // Step 1
    Node& node = data[index];
    while (!node.Children.empty()) {
        const NodeIndex childIndex = node.Children[0];
        DeleteNode(childIndex);
    }

    // Step 2
    data.erase(data.begin() + index);

    // Step 3
    for (Node& node : data) {
        // Step 3.1
        std::erase(node.Children, index);

        // Step 3.2
        std::ranges::for_each(node.Children, [index](NodeIndex& childIndex) {
            if (childIndex > index) {
                childIndex--;
            }
        });
    }
}