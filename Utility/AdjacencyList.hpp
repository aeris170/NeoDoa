#pragma once

#include <vector>
#include <utility>
#include <algorithm>

#include <Utility/TemplateUtilities.hpp>

template<typename Vertex, size_t InitialVertexCount = 512, size_t InitialEdgeCountPerVertex = 8>
    requires std::equality_comparable<Vertex>
struct AdjacencyList {
    using EdgeList = std::vector<size_t>;
    using DataStructure = std::vector<std::pair<Vertex, EdgeList>>;

    /// <summary>
    /// Iterates over incoming edges and retrieves origin vertices.
    /// Use HasNext() to check if there are more edges and iteration can continue,
    /// Use Next() to retrieve the next vertex
    /// </summary>
    struct IncomingEdgeIterator {

        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using pointer = value_type*;
        using reference = value_type&;

        IncomingEdgeIterator(DataStructure::const_iterator begin, DataStructure::const_iterator end, const Vertex& vertex);

        bool HasNext() const noexcept;
        const Vertex& Next() noexcept;

    private:
        DataStructure::const_iterator begin, current, end;
        EdgeList::value_type vertexIndex;
    };

    /// <summary>
    /// Iterates over outgoing edges and retrieves destination vertices.
    /// Use HasNext() to check if there are more edges and iteration can continue,
    /// Use Next() to retrieve the next vertex
    /// </summary>
    struct OutgoingEdgeIterator {

        OutgoingEdgeIterator(DataStructure::const_iterator begin, DataStructure::const_iterator end, const Vertex& vertex);

        bool HasNext() const noexcept;
        const Vertex& Next() noexcept;

    private:
        DataStructure::const_iterator begin, end;
        EdgeList::size_type currentIndex{};
        const EdgeList& edgeList;
    };

    AdjacencyList() noexcept;

    /// <summary>
    /// Precondition: vertex is absent.
    /// Postcondition: vertex is present.
    /// </summary>
    void AddVertex(const Vertex& vertex) noexcept;

    /// <summary>
    /// Precondition: origin and destination are present
    ///               origin and destionation are different
    ///               an edge from origin to destination is absent.
    /// Postcondition: An edge from origin to destination is present.
    /// </summary>
    void AddEdge(const Vertex& origin, const Vertex& destination) noexcept;

    /// <summary>
    /// Returns true if vertex exists.
    /// Precondition: None.
    /// Postcondition: None.
    /// </summary>
    bool HasVertex(const Vertex& vertex) const noexcept;

    /// <summary>
    /// Precondition: origin is present.
    /// Postcondition: None.
    /// </summary>
    bool HasEdge(const Vertex& origin, const Vertex& destination) const noexcept;

    /// <summary>
    /// Precondition: vertex is present
    /// Postcondition: vertex is absent
    ///                incoming and outgoing edges to/from vertex are absent
    /// </summary>
    void RemoveVertex(const Vertex& vertex) noexcept;

    /// <summary>
    /// Precondition: origin and destination are present
    ///               origin and destination are different
    ///               an edge from origin to destination is present.
    /// Postcondition: An edge from origin to destination is absent.
    /// </summary>
    void RemoveEdge(const Vertex& origin, const Vertex& destination) noexcept;

    /// <summary>
    /// Precondition: None.
    /// Postcondition: All vertices and edges are absent.
    /// </summary>
    void Clear() noexcept;

    /// <summary>
    /// Precondition: origin is present.
    /// Postcondition: None.
    /// </summary>
    IncomingEdgeIterator GetIncomingEdgesOf(const Vertex& origin) const noexcept;

    /// <summary>
    /// Precondition: origin is present.
    /// Postcondition: None.
    /// </summary>
    OutgoingEdgeIterator GetOutgoingEdgesOf(const Vertex& origin) const noexcept;

private:
    DataStructure data{};
};

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::IncomingEdgeIterator::IncomingEdgeIterator(DataStructure::const_iterator begin, DataStructure::const_iterator end, const Vertex& vertex) :
    begin(begin),
    current(begin),
    end(end),
    vertexIndex(std::distance(begin, std::find_if(begin, end, [&vertex](auto& pair) { return pair.first == vertex; }))) {
    while(current < end) {
        auto search = std::ranges::find(current->second, vertexIndex);
        if (search == current->second.end()) {
            current = std::next(current);
        } else {
            break;
        }
    }
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline bool AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::IncomingEdgeIterator::HasNext() const noexcept { return current < end; }
template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline const Vertex& AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::IncomingEdgeIterator::Next() noexcept {
    auto& rv = current->first;
    current = std::next(current);
    while (current < end) {
        auto search = std::ranges::find(current->second, vertexIndex);
        if (search == current->second.end()) {
            current = std::next(current);
        } else {
            break;
        }
    }
    return rv;
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::OutgoingEdgeIterator::OutgoingEdgeIterator(DataStructure::const_iterator begin, DataStructure::const_iterator end, const Vertex& vertex) :
    begin(begin),
    end(end),
    edgeList(std::find_if(begin, end, [&vertex](auto& pair) { return pair.first == vertex; })->second) {}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline bool AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::OutgoingEdgeIterator::HasNext() const noexcept { return currentIndex < edgeList.size(); }
template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline const Vertex& AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::OutgoingEdgeIterator::Next() noexcept {
    size_t edgeVertexIndex = edgeList[currentIndex++];
    return (begin + edgeVertexIndex)->first;
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::AdjacencyList() noexcept {
    data.reserve(InitialVertexCount);
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline void AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::AddVertex(const Vertex& vertex) noexcept {
    data.emplace_back(vertex, EdgeList{}).second.reserve(InitialEdgeCountPerVertex);
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline void AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::AddEdge(const Vertex& origin, const Vertex& destination) noexcept {
    EdgeList& originEdgeList = std::ranges::find_if(data, [&origin](const auto& pair) { return pair.first == origin; })->second;

    auto destinationIndex = std::distance(data.begin(), std::ranges::find_if(data, [&destination](const auto& pair) { return pair.first == destination; }));

    originEdgeList.push_back(destinationIndex);
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline bool AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::HasVertex(const Vertex& origin) const noexcept {
    return std::ranges::find_if(data, [&origin](const auto& pair) { return pair.first == origin; }) != data.end();
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline bool AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::HasEdge(const Vertex& origin, const Vertex& destination) const noexcept {
    const EdgeList& originEdgeList = std::ranges::find_if(data, [&origin](const auto& pair) { return pair.first == origin; })->second;

    auto destinationIndex = std::distance(data.begin(), std::ranges::find_if(data, [&destination](const auto& pair) { return pair.first == destination; }));

    return std::ranges::find(originEdgeList, destinationIndex) != originEdgeList.end();
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline void AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::RemoveVertex(const Vertex& vertex) noexcept {
    // Step 1. Remove all incoming edges - traverse graph, check every edge list and remove own index
    // Step 2. Remove all outgoing edges - trivial, remove vertex from graph (graphnode contains vertex and it's outgoing edges), this will break indices
    // Step 3. Fix indices - traverse graph, traverse every edge list and decrement entries greater than own index

    // S1
    auto vertexIterator = std::ranges::find_if(data, [&vertex](const auto& pair) { return pair.first == vertex; });
    auto vertexIndex = static_cast<EdgeList::value_type>(std::distance(data.begin(), vertexIterator));
    for (auto& [_, edgeList] : data) {
        auto vertexIndexInEdges = std::ranges::find(edgeList, vertexIndex);
        if (vertexIndexInEdges != edgeList.end()) {
            edgeList.erase(vertexIndexInEdges);
        }
    }

    // S2
    data.erase(vertexIterator);

    // S3
    for (auto& [_, edgeList] : data) {
        for (auto& edgeIndex : edgeList) {
            // Branchless code below
            int modifier = static_cast<int>(edgeIndex > vertexIndex);
            edgeIndex = edgeIndex - modifier;
            /// if (edgeIndex > vertexIndex) edgeIndex--;
        }
    }
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline void AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::RemoveEdge(const Vertex& origin, const Vertex& destination) noexcept {
    EdgeList& originEdgeList = std::ranges::find_if(data, [&origin](const auto& pair) { return pair.first == origin; })->second;

    auto destinationIndex = static_cast<EdgeList::value_type>(std::distance(data.begin(), std::ranges::find_if(data, [&destination](auto& pair) { return pair.first == destination; })));

    auto destinationIndexInEdges = std::ranges::find(originEdgeList, destinationIndex);
    originEdgeList.erase(destinationIndexInEdges);
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline void AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::Clear() noexcept {
    data.clear();
}

template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::IncomingEdgeIterator AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::GetIncomingEdgesOf(const Vertex& vertex) const noexcept {
    return { data.cbegin(), data.cend(), vertex };
}
template<typename Vertex, size_t InitialVertexCount, size_t InitialEdgeCountPerVertex>
    requires std::equality_comparable<Vertex>
inline AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::OutgoingEdgeIterator AdjacencyList<Vertex, InitialVertexCount, InitialEdgeCountPerVertex>::GetOutgoingEdgesOf(const Vertex& vertex) const noexcept {
    return { data.cbegin(), data.cend(), vertex };
}
