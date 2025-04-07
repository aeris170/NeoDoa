#pragma once

#include <string>
#include <optional>
#include <functional>

#include <tinyxml2.h>

#include <Engine/Mesh.hpp>

struct FNode;

struct MeshDeserializationResult {
    bool erred{ false };
    std::vector<std::string> warnings{};
    std::vector<std::string> errors{};
    Mesh deserializedMesh{};
};

/* This is the big bad bada-boom of deserializers, this function will call appropriate deserializers. See below for an overview! */
MeshDeserializationResult DeserializeMesh(const FNode& file) noexcept;
MeshDeserializationResult DeserializeMesh(const std::string_view data) noexcept;

/* Here is the general overview */
namespace MeshDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction = std::function<void(tinyxml2::XMLElement& rootElem, MeshDeserializationResult& mdr)>;
    using DeserializeNameFunction = std::function<void(tinyxml2::XMLElement& meshElem, MeshDeserializationResult& mdr)>;
    namespace Vertices {
        using DeserializeFunction = std::function<void(tinyxml2::XMLElement& verticesElem, MeshDeserializationResult& mdr)>;
        using DeserializeVertexFunction = std::function<void(tinyxml2::XMLElement& vertexElem, MeshDeserializationResult& mdr)>;
    }
    namespace Indices {
        using DeserializeFunction = std::function<void(tinyxml2::XMLElement& indicesElem, MeshDeserializationResult& mdr)>;
        using DeserializeIndexFunction = std::function<void(tinyxml2::XMLElement& indexElem, MeshDeserializationResult& mdr)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will desrialize by default. */
    void DefaultDeserialize(tinyxml2::XMLElement& rootElem, MeshDeserializationResult& mdr) noexcept;
    void DefaultDeserializeName(tinyxml2::XMLElement& meshElem, MeshDeserializationResult& mdr) noexcept;
    namespace Vertices {
        void DefaultDeserialize(tinyxml2::XMLElement& verticesElem, MeshDeserializationResult& mdr);
        void DefaultDeserializeVertex(tinyxml2::XMLElement& vertexElem, MeshDeserializationResult& mdr);
    }
    namespace Indices {
        void DefaultDeserialize(tinyxml2::XMLElement& indicesElem, MeshDeserializationResult& mdr);
        void DefaultDeserializeIndex(tinyxml2::XMLElement& indexElem, MeshDeserializationResult& mdr);
    }

    /* ----- Serializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                     /* Feel free to assign this your own function, if you need custom deserialization */
    inline DeserializeNameFunction DeserializeName{ DefaultDeserializeName };         /* Feel free to assign this your own function, if you need custom deserialization */
    namespace Vertices {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                   /* Feel free to assign this your own function, if you need custom deserialization */
        inline DeserializeVertexFunction DeserializeVertex{ DefaultDeserializeVertex }; /* Feel free to assign this your own function, if you need custom deserialization */
    }
    namespace Indices {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                   /* Feel free to assign this your own function, if you need custom deserialization */
        inline DeserializeIndexFunction DeserializeIndex{ DefaultDeserializeIndex };    /* Feel free to assign this your own function, if you need custom deserialization */
    }
}
