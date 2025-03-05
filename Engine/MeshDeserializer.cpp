#include <Engine/MeshDeserializer.hpp>

#include <Engine/Log.hpp>
#include <Engine/FileNode.hpp>

MeshDeserializationResult DeserializeMesh(const FNode& file) noexcept {
    file.ReadContent();
    auto rv = DeserializeMesh(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedMesh.Name = file.Name();
    }
    return rv;
}
MeshDeserializationResult DeserializeMesh(const std::string_view data) noexcept {
    MeshDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't deserialize mesh!");
        rv.errors.emplace_back("This should normally never happen unless mesh data is ill-formed.");
        DOA_LOG_ERROR("Couldn't deserialize mesh!\n\n%s", data);
    } else {
        MeshDeserializer::Deserialize(*doc.RootElement(), rv);
    }

    return rv;
}

void MeshDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootElem, MeshDeserializationResult& mdr) noexcept {
    DeserializeName(rootElem, mdr);

    tinyxml2::XMLElement* verticesElem = rootElem.FirstChildElement("vertices");
    if (!verticesElem) {
        mdr.erred = true;
        mdr.errors.emplace_back("Couldn't deserialize vertices! No \"vertices\" element.");
    } else {
        Vertices::Deserialize(*verticesElem, mdr);
    }

    tinyxml2::XMLElement* indicesElem = rootElem.FirstChildElement("indices");
    if (!indicesElem) {
        mdr.warnings.emplace_back("Couldn't deserialize indices! No \"indices\" element.");
        mdr.warnings.emplace_back("Using indices may help reducing the vertex count for better performance.");
    } else {
        Indices::Deserialize(*indicesElem, mdr);
    }
}
void MeshDeserializer::DefaultDeserializeName(tinyxml2::XMLElement& meshElem, MeshDeserializationResult& mdr) noexcept {
    const char* name;
    meshElem.QueryAttribute("name", &name);
    if (!name || name[0] == '\0') {
        mdr.erred = true;
        mdr.errors.emplace_back("Couldn't read mesh name.");
        return;
    }
    mdr.deserializedMesh.Name = name;
}

void MeshDeserializer::Vertices::DefaultDeserialize(tinyxml2::XMLElement& verticesElem, MeshDeserializationResult& mdr) {
    tinyxml2::XMLElement* vertexElem = verticesElem.FirstChildElement("vertex");
    while (vertexElem) {
        DeserializeVertex(*vertexElem, mdr);
        vertexElem = vertexElem->NextSiblingElement("vertex");
    }
    mdr.deserializedMesh.CalculateAABBProperties();
}
void MeshDeserializer::Vertices::DefaultDeserializeVertex(tinyxml2::XMLElement& vertexElem, MeshDeserializationResult& mdr) {
    const tinyxml2::XMLElement* positionElem  = vertexElem.FirstChildElement("position");
    const tinyxml2::XMLElement* normalElem    = vertexElem.FirstChildElement("normal");
    const tinyxml2::XMLElement* colorElem     = vertexElem.FirstChildElement("color");
    const tinyxml2::XMLElement* texCoordsElem = vertexElem.FirstChildElement("texCoords");
    if (!positionElem || !normalElem || !colorElem || !texCoordsElem) {
        mdr.erred = true;
        mdr.errors.emplace_back(std::format("Couldn't deserialize vertex! Line: {}", vertexElem.GetLineNum()));
        mdr.errors.emplace_back("Missing: ");
        if (!positionElem)  { mdr.errors.emplace_back("\tPosition");   }
        if (!normalElem)    { mdr.errors.emplace_back("\tNormal");     }
        if (!colorElem)     { mdr.errors.emplace_back("\tColor");      }
        if (!texCoordsElem) { mdr.errors.emplace_back("\tTex Coords"); }
        return;
    }

    //
    static_assert(std::numeric_limits<float>::has_quiet_NaN);

    Mesh::Vertex vertex;
    { // Deserialize position
        float x{ std::numeric_limits<float>::quiet_NaN() };
        float y{ std::numeric_limits<float>::quiet_NaN() };
        float z{ std::numeric_limits<float>::quiet_NaN() };
        positionElem->QueryFloatAttribute("x", &x);
        positionElem->QueryFloatAttribute("y", &y);
        positionElem->QueryFloatAttribute("z", &z);
        if (std::isnan(x) || std::isnan(y) || std::isnan(z)) {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Couldn't deserialize vertex position! Line: {}", positionElem->GetLineNum()));
            mdr.errors.emplace_back("Missing or ill-formed: ");
            if (std::isnan(x)) { mdr.errors.emplace_back("\tx"); }
            if (std::isnan(y)) { mdr.errors.emplace_back("\ty"); }
            if (std::isnan(z)) { mdr.errors.emplace_back("\tz"); }
            return;
        }
        vertex.Position = { x, y, z };
    }

    { // Deserialize normal
        float x{ std::numeric_limits<float>::quiet_NaN() };
        float y{ std::numeric_limits<float>::quiet_NaN() };
        float z{ std::numeric_limits<float>::quiet_NaN() };
        normalElem->QueryFloatAttribute("x", &x);
        normalElem->QueryFloatAttribute("y", &y);
        normalElem->QueryFloatAttribute("z", &z);
        if (std::isnan(x) || std::isnan(y) || std::isnan(z)) {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Couldn't deserialize vertex normal! Line: {}", normalElem->GetLineNum()));
            mdr.errors.emplace_back("Missing or ill-formed: ");
            if (std::isnan(x)) { mdr.errors.emplace_back("\tx"); }
            if (std::isnan(y)) { mdr.errors.emplace_back("\ty"); }
            if (std::isnan(z)) { mdr.errors.emplace_back("\tz"); }
            return;
        }
        vertex.Normal = { x, y, z };
    }

    { // Deserialize color
        float r{ std::numeric_limits<float>::quiet_NaN() };
        float g{ std::numeric_limits<float>::quiet_NaN() };
        float b{ std::numeric_limits<float>::quiet_NaN() };
        float a{ std::numeric_limits<float>::quiet_NaN() };
        colorElem->QueryFloatAttribute("r", &r);
        colorElem->QueryFloatAttribute("g", &g);
        colorElem->QueryFloatAttribute("b", &b);
        colorElem->QueryFloatAttribute("a", &a);
        if (std::isnan(r) || std::isnan(g) || std::isnan(b) || std::isnan(a)) {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Couldn't deserialize vertex color! Line: {}", colorElem->GetLineNum()));
            mdr.errors.emplace_back("Missing or ill-formed: ");
            if (std::isnan(r)) { mdr.errors.emplace_back("\tr"); }
            if (std::isnan(g)) { mdr.errors.emplace_back("\tg"); }
            if (std::isnan(b)) { mdr.errors.emplace_back("\tb"); }
            if (std::isnan(a)) { mdr.errors.emplace_back("\ta"); }
            return;
        }
        vertex.Color = { r, g, b, a };
    }

    { // Deserialize texCoords
        float u{ std::numeric_limits<float>::quiet_NaN() };
        float v{ std::numeric_limits<float>::quiet_NaN() };
        texCoordsElem->QueryFloatAttribute("u", &u);
        texCoordsElem->QueryFloatAttribute("v", &v);
        if (std::isnan(u) || std::isnan(v)) {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Couldn't deserialize vertex texCoords! Line: {}", texCoordsElem->GetLineNum()));
            mdr.errors.emplace_back("Missing or ill-formed: ");
            if (std::isnan(u)) { mdr.errors.emplace_back("\tu"); }
            if (std::isnan(v)) { mdr.errors.emplace_back("\tv"); }
            return;
        }
        vertex.TexCoords = { u, v };
    }

    mdr.deserializedMesh.Vertices.push_back(vertex);
}


void MeshDeserializer::Indices::DefaultDeserialize(tinyxml2::XMLElement& indicesElem, MeshDeserializationResult& mdr) {
    tinyxml2::XMLElement* indexElem = indicesElem.FirstChildElement("index");
    while (indexElem) {
        DeserializeIndex(*indexElem, mdr);
        indexElem = indexElem->NextSiblingElement("index");
    }
}
void MeshDeserializer::Indices::DefaultDeserializeIndex(tinyxml2::XMLElement& indexElem, MeshDeserializationResult& mdr) {
    Mesh::IndexList::value_type index{ std::numeric_limits<Mesh::IndexList::value_type>::max() };
    indexElem.QueryUnsignedText(&index);
    if (index == std::numeric_limits<Mesh::IndexList::value_type>::max()) {
        mdr.erred = true;
        mdr.errors.emplace_back(std::format("Couldn't deserialize index! Line: {}", indexElem.GetLineNum()));
        return;
    }

    mdr.deserializedMesh.Indices.push_back(index);
}
