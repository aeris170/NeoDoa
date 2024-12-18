#pragma once

#include <functional>
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include <Engine/Entity.hpp>

struct Color;
struct Scene;
struct IDComponent;
struct TransformComponent;
struct ParentComponent;
struct ChildComponent;
struct OrthoCameraComponent;
struct PerspectiveCameraComponent;

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeScene(const Scene& scene);

/* Here is the general overview */
namespace SceneSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction                          = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
    using SerializeFunction                              = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
    namespace SceneConfig {
        using SerializeFunction                          = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
        using SerializeNameFunction                      = std::function<void(tinyxml2::XMLPrinter& printer, std::string_view name)>;
        using SerializeClearColorFunction                = std::function<void(tinyxml2::XMLPrinter& printer, const Color& color)>;
    }
    namespace Entities {
        using SerializeFunction                           = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
        using SerializeEntityFunction                     = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity)>;
        using SerializeIDComponentFunction                = std::function<void(tinyxml2::XMLPrinter& printer, const IDComponent& component)>;
        using SerializeTransformComponentFunction         = std::function<void(tinyxml2::XMLPrinter& printer, const TransformComponent& component)>;
        using SerializeParentComponentFunction            = std::function<void(tinyxml2::XMLPrinter& printer, const ParentComponent& component)>;
        using SerializeChildComponentFunction             = std::function<void(tinyxml2::XMLPrinter& printer, const ChildComponent& component)>;
        using SerializeOrthoCameraComponentFunction       = std::function<void(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& component)>;
        using SerializePerspectiveCameraComponentFunction = std::function<void(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& component)>;
        using SerializeUserDefinedComponentsFunction      = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Scene& scene);
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
    namespace SceneConfig {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
        void DefaultSerializeName(tinyxml2::XMLPrinter& printer, ::std::string_view name);
        void DefaultSerializeClearColor(tinyxml2::XMLPrinter& printer, const Color& color);
    }
    namespace Entities {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
        void DefaultSerializeEntity(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);
        void DefaultSerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& component);
        void DefaultSerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& component);
        void DefaultSerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& component);
        void DefaultSerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& component);
        void DefaultSerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& component);
        void DefaultSerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& component);
        void DefaultSerializeUserDefinedComponents(tinyxml2::XMLPrinter&, const Scene&, Entity);
    }

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                                                       /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                                                                   /* Feel free to assign this your own function, if you need custom serialization */
    namespace SceneConfig {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                               /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeNameFunction SerializeName{ DefaultSerializeName };                                                                   /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeClearColorFunction SerializeClearColor{ DefaultSerializeClearColor };                                                 /* Feel free to assign this your own function, if you need custom serialization */
    }
    namespace Entities {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                               /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeEntityFunction SerializeEntity{ DefaultSerializeEntity };                                                             /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeIDComponentFunction SerializeIDComponent{ DefaultSerializeIDComponent };                                              /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTransformComponentFunction SerializeTransformComponent{ DefaultSerializeTransformComponent };                         /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeParentComponentFunction SerializeParentComponent{ DefaultSerializeParentComponent };                                  /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeChildComponentFunction SerializeChildComponent{ DefaultSerializeChildComponent };                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeOrthoCameraComponentFunction SerializeOrthoCameraComponent{ DefaultSerializeOrthoCameraComponent };                   /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializePerspectiveCameraComponentFunction SerializePerspectiveCameraComponent{ DefaultSerializePerspectiveCameraComponent }; /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeUserDefinedComponentsFunction SerializeUserDefinedComponents{ DefaultSerializeUserDefinedComponents };                /* Feel free to assign this your own function, if you need custom serialization */
    }
}

/* Some Helper functions to trivially serialize trivial data types */
namespace SceneSerializer::Helpers {
    void SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, const int value);
    void SerializeEntityID(tinyxml2::XMLPrinter& printer, const Entity entity);
    void SerializeInt8(tinyxml2::XMLPrinter& printer, const int8_t value);
    void SerializeInt16(tinyxml2::XMLPrinter& printer, const int16_t value);
    void SerializeInt32(tinyxml2::XMLPrinter& printer, const int32_t value);
    void SerializeInt64(tinyxml2::XMLPrinter& printer, const int64_t value);
    void SerializeUInt8(tinyxml2::XMLPrinter& printer, const uint8_t value);
    void SerializeUInt16(tinyxml2::XMLPrinter& printer, const uint16_t value);
    void SerializeUInt32(tinyxml2::XMLPrinter& printer, const uint32_t value);
    void SerializeUInt64(tinyxml2::XMLPrinter& printer, const uint64_t value);
    void SerializeInt(tinyxml2::XMLPrinter& printer, const int value);
    void SerializeLong(tinyxml2::XMLPrinter& printer, const long value);
    void SerializeFloat(tinyxml2::XMLPrinter& printer, const float value);
    void SerializeDouble(tinyxml2::XMLPrinter& printer, const double value);
    void SerializeBool(tinyxml2::XMLPrinter& printer, const bool value);
    void SerializeString(tinyxml2::XMLPrinter& printer, const std::string_view value);
    void SerializeVec2(tinyxml2::XMLPrinter& printer, const glm::vec2& vec);
    void SerializeVec3(tinyxml2::XMLPrinter& printer, const glm::vec3& vec);
    void SerializeVec4(tinyxml2::XMLPrinter& printer, const glm::vec4& vec);
    void SerializeQuat(tinyxml2::XMLPrinter& printer, const glm::quat& quat);
    void SerializeMat2(tinyxml2::XMLPrinter& printer, const glm::mat2& mat);
    void SerializeMat3(tinyxml2::XMLPrinter& printer, const glm::mat3& mat);
    void SerializeMat4(tinyxml2::XMLPrinter& printer, const glm::mat4& mat);
}