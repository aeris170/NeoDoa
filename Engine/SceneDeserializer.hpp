#pragma once

#include <string>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include <Engine/Entity.hpp>

struct Scene;
struct FNode;
struct Color;

Scene DeserializeScene(const FNode& file);
Scene DeserializeScene(const std::string& data);

/* Here is the general overview */
namespace SceneDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction                           = std::function<void(tinyxml2::XMLElement& rootNode, Scene& scene)>;
    namespace SceneConfig {
        using DeserializeFunction                       = std::function<void(tinyxml2::XMLElement& configNode, Scene& scene)>;
        using DeserializeNameFunction                   = std::function<void(tinyxml2::XMLElement& configNode, Scene& scene)>;
        using DeserializeClearColorFunction             = std::function<void(tinyxml2::XMLElement& configNode, Scene& scene)>;
        using DeserializeSelectionOutlineColorFunction  = std::function<void(tinyxml2::XMLElement& configNode, Scene& scene)>;
        namespace Cameras {
            using DeserializeFunction                   = std::function<void(tinyxml2::XMLElement& camerasNode, Scene& scene)>;
            using DeserializeActiveCameraFunction       = std::function<void(tinyxml2::XMLElement& activeCameraNode, Scene& scene)>;
            using DeserializeOrthoCameraFunction        = std::function<void(tinyxml2::XMLElement& orthoCameraNode, Scene& scene)>;
            using DeserializePerspectiveCameraFunction  = std::function<void(tinyxml2::XMLElement& perspectiveCameraNode, Scene& scene)>;
        }
    }
    namespace Entities {
        using DeserializeFunction                       = std::function<void(tinyxml2::XMLElement& entitiesNode, Scene& scene)>;
        using DeserializeEntityFunction                 = std::function<void(tinyxml2::XMLElement& entityNode, Scene& scene)>;
        using DeserializeIDComponentFunction            = std::function<void(tinyxml2::XMLElement& componentNode, Scene& scene, Entity& readEntity)>;
        using DeserializeTransformComponentFunction     = std::function<void(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity)>;
        using DeserializeParentComponentFunction        = std::function<void(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity)>;
        using DeserializeChildComponentFunction         = std::function<void(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity)>;
        using DeserializeUserDefinedComponentsFunction  = std::function<void(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity, const std::string& name)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will deserialize by default. */
    void DefaultDeserialize(tinyxml2::XMLElement& rootNode, Scene& scene);
    namespace SceneConfig {
        void DefaultDeserialize(tinyxml2::XMLElement& configNode, Scene& scene);
        void DefaultDeserializeName(tinyxml2::XMLElement& configNode, Scene& scene);
        void DefaultDeserializeClearColor(tinyxml2::XMLElement& configNode, Scene& scene);
        void DefaultDeserializeSelectionOutlineColor(tinyxml2::XMLElement& configNode, Scene& scene);
        namespace Cameras {
            void DefaultDeserialize(tinyxml2::XMLElement& camerasNode, Scene& scene);
            void DefaultDeserializeActiveCamera(tinyxml2::XMLElement& activeCameraNode, Scene& scene);
            void DefaultDeserializeOrthoCamera(tinyxml2::XMLElement& orthoCameraNode, Scene& scene);
            void DefaultDeserializePerspectiveCamera(tinyxml2::XMLElement& perspectiveCameraNode, Scene& scene);
        }
    }
    namespace Entities {
        void DefaultDeserialize(tinyxml2::XMLElement& entitiesNode, Scene& scene);
        void DefaultDeserializeEntity(tinyxml2::XMLElement& entityNode, Scene& scene);
        void DefaultDeserializeIDComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity& readEntity);
        void DefaultDeserializeTransformComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity);
        void DefaultDeserializeParentComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity);
        void DefaultDeserializeChildComponent(tinyxml2::XMLElement& componentNode, Scene& scene, Entity entity);
        void DefaultDeserializeUserDefinedComponents(tinyxml2::XMLElement&, Scene&, Entity, const std::string&);
    }

    /* ----- Deserializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                     /* Feel free to assign this your own function, if you need custom serialization */
    namespace SceneConfig {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeNameFunction DeserializeName{ DefaultDeserializeName };                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeClearColorFunction DeserializeClearColor{ DefaultDeserializeClearColor };                                   /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeSelectionOutlineColorFunction DeserializeSelectionOutlineColor{ DefaultDeserializeSelectionOutlineColor };  /* Feel free to assign this your own function, if you need custom serialization */
        namespace Cameras {
            inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                             /* Feel free to assign this your own function, if you need custom serialization */
            inline DeserializeActiveCameraFunction DeserializeActiveCamera{ DefaultDeserializeActiveCamera };                         /* Feel free to assign this your own function, if you need custom serialization */
            inline DeserializeOrthoCameraFunction DeserializeOrthoCamera{ DefaultDeserializeOrthoCamera };                            /* Feel free to assign this your own function, if you need custom serialization */
            inline DeserializePerspectiveCameraFunction DeserializePerspectiveCamera{ DefaultDeserializePerspectiveCamera };          /* Feel free to assign this your own function, if you need custom serialization */
        }
    }
    namespace Entities {
        inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeEntityFunction DeserializeEntity{ DefaultDeserializeEntity };                                               /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeIDComponentFunction DeserializeIDComponent{ DefaultDeserializeIDComponent };                                /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeTransformComponentFunction DeserializeTransformComponent{ DefaultDeserializeTransformComponent };           /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeParentComponentFunction DeserializeParentComponent{ DefaultDeserializeParentComponent };                    /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeChildComponentFunction DeserializeChildComponent{ DefaultDeserializeChildComponent };                       /* Feel free to assign this your own function, if you need custom serialization */
        inline DeserializeUserDefinedComponentsFunction DeserializeUserDefinedComponents{ DefaultDeserializeUserDefinedComponents };  /* Feel free to assign this your own function, if you need custom serialization */
    }
}

namespace SceneDeserializer::Helpers {
    int DeserializeEnum(const tinyxml2::XMLElement& property);
    Entity DeserializeEntityID(const tinyxml2::XMLElement& property, std::string_view identifierOverride = "id");
    int8_t DeserializeInt8  (const tinyxml2::XMLElement& property);
    int16_t DeserializeInt16 (const tinyxml2::XMLElement& property);
    int32_t DeserializeInt32 (const tinyxml2::XMLElement& property);
    int64_t DeserializeInt64 (const tinyxml2::XMLElement& property);
    uint8_t DeserializeUInt8 (const tinyxml2::XMLElement& property);
    uint16_t DeserializeUInt16(const tinyxml2::XMLElement& property);
    uint32_t DeserializeUInt32(const tinyxml2::XMLElement& property);
    uint64_t DeserializeUInt64(const tinyxml2::XMLElement& property);
    int DeserializeInt(const tinyxml2::XMLElement& property);
    long DeserializeLong(const tinyxml2::XMLElement& property);
    float DeserializeFloat(const tinyxml2::XMLElement& property);
    double DeserializeDouble(const tinyxml2::XMLElement& property);
    bool DeserializeBool(const tinyxml2::XMLElement& property);
    std::string DeserializeString(const tinyxml2::XMLElement& property);
    glm::vec2 DeserializeVec2(const tinyxml2::XMLElement& property);
    glm::vec3 DeserializeVec3(const tinyxml2::XMLElement& property);
    glm::vec4 DeserializeVec4(const tinyxml2::XMLElement& property);
    glm::quat DeserializeQuat(const tinyxml2::XMLElement& property);
    glm::mat2 DeserializeMat2(const tinyxml2::XMLElement& property);
    glm::mat3 DeserializeMat3(const tinyxml2::XMLElement& property);
    glm::mat4 DeserializeMat4(const tinyxml2::XMLElement& property);
}