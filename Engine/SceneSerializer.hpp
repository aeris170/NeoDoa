#pragma once

#include <functional>
#include <string_view>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tinyxml2.h>

#include "Entity.hpp"

struct Scene;
struct Color;
struct ACamera;
struct OrthoCamera;
struct PerspectiveCamera;
struct IDComponent;
struct TransformComponent;
struct ParentComponent;
struct ChildComponent;
struct OrthoCameraComponent;
struct PerspectiveCameraComponent;
/* This big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
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
        using SerializeSelectionOutlineColorFunction     = std::function<void(tinyxml2::XMLPrinter& printer, const Color& color)>;
        namespace Cameras {
            using SerializeFunction                      = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
            using SerializeActiveCameraFunction          = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
            using SerializeOrthoCameraFunction           = std::function<void(tinyxml2::XMLPrinter& printer, const OrthoCamera& camera)>;
            using SerializePerspectiveCameraFunction     = std::function<void(tinyxml2::XMLPrinter& printer, const PerspectiveCamera& camera)>;
        }
    }
    namespace Entities {
        using SerializeFunction                          = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene)>;
        using SerializeEntityFunction                    = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity)>;
        using SerializeIDComponentFunction               = std::function<void(tinyxml2::XMLPrinter& printer, const IDComponent& component)>;
        using SerializeTransformComponentFunction        = std::function<void(tinyxml2::XMLPrinter& printer, const TransformComponent& component)>;
        using SerializeParentComponentFunction           = std::function<void(tinyxml2::XMLPrinter& printer, const ParentComponent& component)>;
        using SerializeChildComponentFunction            = std::function<void(tinyxml2::XMLPrinter& printer, const ChildComponent& component)>;
        using SerializeUserDefinedComponentsFunction     = std::function<void(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Scene& scene);
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
    namespace SceneConfig {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
        void DefaultSerializeName(tinyxml2::XMLPrinter& printer, ::std::string_view name);
        void DefaultSerializeClearColor(tinyxml2::XMLPrinter& printer, const Color& color);
        void DefaultSerializeSelectionOutlineColor(tinyxml2::XMLPrinter& printer, const Color& color);
        namespace Cameras {
            void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
            void DefaultSerializeActiveCamera(tinyxml2::XMLPrinter& printer, const Scene& scene);
            void DefaultSerializeOrthoCamera(tinyxml2::XMLPrinter& printer, const OrthoCamera& camera);
            void DefaultSerializePerspectiveCamera(tinyxml2::XMLPrinter& printer, const PerspectiveCamera& camera);
        }
    }
    namespace Entities {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Scene& scene);
        void DefaultSerializeEntity(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);
        void DefaultSerializeIDComponent(tinyxml2::XMLPrinter& printer, const IDComponent& component);
        void DefaultSerializeTransformComponent(tinyxml2::XMLPrinter& printer, const TransformComponent& component);
        void DefaultSerializeParentComponent(tinyxml2::XMLPrinter& printer, const ParentComponent& component);
        void DefaultSerializeChildComponent(tinyxml2::XMLPrinter& printer, const ChildComponent& component);
        void DefaultSerializeUserDefinedComponents(tinyxml2::XMLPrinter& printer, const Scene& scene, Entity entity);
    }

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                                         /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                                                     /* Feel free to assign this your own function, if you need custom serialization */
    namespace SceneConfig {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeNameFunction SerializeName{ DefaultSerializeName };                                                     /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeClearColorFunction SerializeClearColor{ DefaultSerializeClearColor };                                   /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeSelectionOutlineColorFunction SerializeSelectionOutlineColor{ DefaultSerializeSelectionOutlineColor };  /* Feel free to assign this your own function, if you need custom serialization */
        namespace Cameras {
            inline SerializeFunction Serialize{ DefaultSerialize };                                                             /* Feel free to assign this your own function, if you need custom serialization */
            inline SerializeActiveCameraFunction SerializeActiveCamera{ DefaultSerializeActiveCamera };                         /* Feel free to assign this your own function, if you need custom serialization */
            inline SerializeOrthoCameraFunction SerializeOrthoCamera{ DefaultSerializeOrthoCamera };                            /* Feel free to assign this your own function, if you need custom serialization */
            inline SerializePerspectiveCameraFunction SerializePerspectiveCamera{ DefaultSerializePerspectiveCamera };          /* Feel free to assign this your own function, if you need custom serialization */
        }
    }
    namespace Entities {
        inline SerializeFunction Serialize{ DefaultSerialize };                                                                 /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeEntityFunction SerializeEntity{ DefaultSerializeEntity };                                               /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeIDComponentFunction SerializeIDComponent{ DefaultSerializeIDComponent };                                /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeTransformComponentFunction SerializeTransformComponent{ DefaultSerializeTransformComponent };           /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeParentComponentFunction SerializeParentComponent{ DefaultSerializeParentComponent };                    /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeChildComponentFunction SerializeChildComponent{ DefaultSerializeChildComponent };                       /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeUserDefinedComponentsFunction SerializeUserDefinedComponents{ DefaultSerializeUserDefinedComponents };  /* Feel free to assign this your own function, if you need custom serialization */
    }
}

/* Some Helper functions to trivially serialize trivial data types */
namespace SceneSerializer::Helpers {
    void SerializeEntityID(tinyxml2::XMLPrinter& printer, Entity entity);
    void SerializeEnum(tinyxml2::XMLPrinter& printer, std::string_view name, int value);
    void SerializeInt(tinyxml2::XMLPrinter& printer, int value);
    void SerializeFloat(tinyxml2::XMLPrinter& printer, float value);
    void SerializeDouble(tinyxml2::XMLPrinter& printer, double value);
    void SerializeString(tinyxml2::XMLPrinter& printer, std::string_view value);
    void SerializeVec2(tinyxml2::XMLPrinter& printer, const glm::vec2& vec);
    void SerializeVec3(tinyxml2::XMLPrinter& printer, const glm::vec3& vec);
    void SerializeVec4(tinyxml2::XMLPrinter& printer, const glm::vec4& vec);
    void SerializeQuat(tinyxml2::XMLPrinter& printer, const glm::quat& quat);
    void SerializeMat2(tinyxml2::XMLPrinter& printer, const glm::mat2& mat);
    void SerializeMat3(tinyxml2::XMLPrinter& printer, const glm::mat3& mat);
    void SerializeMat4(tinyxml2::XMLPrinter& printer, const glm::mat4& mat);
    void SerializeOrthoCameraComponent(tinyxml2::XMLPrinter& printer, const OrthoCameraComponent& camera);
    void SerializePerspectiveCameraComponent(tinyxml2::XMLPrinter& printer, const PerspectiveCameraComponent& camera);
}