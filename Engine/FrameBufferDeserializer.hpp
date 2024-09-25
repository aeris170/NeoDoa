#pragma once

#include <string>
#include <vector>
#include <functional>

#include <tinyxml2.h>

#include <Engine/FrameBuffer.hpp>

struct FNode;

struct FrameBufferDeserializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    FrameBuffer deserializedFrameBuffer{ "" };
};

struct FrameBufferAttachmentSerializationResult {
    bool erred{ false };
    std::vector<std::string> errors{};
    FrameBufferAttachment deserializedAttachment{};
};

/* This is the big bad bada-boom of deserializers, this function will call appropriate deserializers. See below for an overview! */
FrameBufferDeserializationResult DeserializeFrameBuffer(const FNode& file) noexcept;
FrameBufferDeserializationResult DeserializeFrameBuffer(const std::string_view data) noexcept;

/* Here is the general overview */
namespace FrameBufferDeserializer {

    /* ------- Type Definitons ------- */
    using DeserializeFunction = std::function<void(const tinyxml2::XMLElement& rootElem, FrameBufferDeserializationResult& fbdr)>;
    namespace Attachments {
        using DeserializeColorAttachmentsFunction       = std::function<void(const tinyxml2::XMLElement& attachmentsElem, FrameBufferDeserializationResult& fbdr)>;
        using DeserializeDepthAttachmentFunction        = std::function<void(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr)>;
        using DeserializeStencilAttachmentFunction      = std::function<void(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr)>;
        using DeserializeDepthStencilAttachmentFunction = std::function<void(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will deserialize by default. */
    void DefaultDeserialize(const tinyxml2::XMLElement& rootElem, FrameBufferDeserializationResult& fbdr) noexcept;
    namespace Attachments {
        void DefaultDeserializeColorAttachments(const tinyxml2::XMLElement& attachmentsElem, FrameBufferDeserializationResult& fbdr) noexcept;
        void DefaultDeserializeDepthAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept;
        void DefaultDeserializeStencilAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept;
        void DefaultDeserializeDepthStencilAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept;
    }

    /* ----- Deserializer Functions ----- */
    inline DeserializeFunction Deserialize{ DefaultDeserialize };                                                                       /* Feel free to assign this your own function, if you need custom deserialization */
    namespace Attachments {
        inline DeserializeColorAttachmentsFunction       DeserializeColorAttachments      { DefaultDeserializeColorAttachments       }; /* Feel free to assign this your own function, if you need custom deserialization */
        inline DeserializeDepthAttachmentFunction        DeserializeDepthAttachment       { DefaultDeserializeDepthAttachment        }; /* Feel free to assign this your own function, if you need custom deserialization */
        inline DeserializeStencilAttachmentFunction      DeserializeStencilAttachment     { DefaultDeserializeStencilAttachment      }; /* Feel free to assign this your own function, if you need custom deserialization */
        inline DeserializeDepthStencilAttachmentFunction DeserializeDepthStencilAttachment{ DefaultDeserializeDepthStencilAttachment }; /* Feel free to assign this your own function, if you need custom deserialization */
    }
};

namespace FrameBufferDeserializer::Helpers {
    FrameBufferAttachmentSerializationResult DeserializeAttachment(const tinyxml2::XMLElement& attachmentElem) noexcept;
}