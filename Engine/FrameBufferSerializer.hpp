#pragma once

#include <string>
#include <functional>

#include <tinyxml2.h>

#include <Engine/FrameBuffer.hpp>

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeFrameBuffer(const FrameBuffer& frameBuffer) noexcept;

/* Here is the general overview */
namespace FrameBufferSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction = std::function<void(tinyxml2::XMLPrinter& printer, const FrameBuffer& frameBuffer)>;
    using SerializeFunction     = std::function<void(tinyxml2::XMLPrinter& printer, const FrameBuffer& frameBuffer)>;
    namespace Attachments {
        using SerializeColorAttachmentsFunction        = std::function<void(tinyxml2::XMLPrinter& printer, const std::vector<FrameBufferAttachment>& colorAttachments)>;
        using SerializeDepthAttachmentFunction         = std::function<void(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthAttachment)>;
        using SerializeStencilAttachmentsFunction      = std::function<void(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& stencilAttachment)>;
        using SerializeDepthStencilAttachmentsFunction = std::function<void(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthStencilAttachment)>;
    }

    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const FrameBuffer& frameBuffer) noexcept;
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const FrameBuffer& frameBuffer) noexcept;
    namespace Attachments {
        void DefaultSerializeColorAttachments(tinyxml2::XMLPrinter& printer, const std::vector<FrameBufferAttachment>& colorAttachments) noexcept;
        void DefaultSerializeDepthAttachment(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthAttachment) noexcept;
        void DefaultSerializeStencilAttachments(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& stencilAttachment) noexcept;
        void DefaultSerializeDepthStencilAttachments(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthStencilAttachment) noexcept;
    }

    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment };                                                              /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };                                                                          /* Feel free to assign this your own function, if you need custom serialization */
    namespace Attachments {
        inline SerializeColorAttachmentsFunction SerializeColorAttachments{ DefaultSerializeColorAttachments };                      /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeDepthAttachmentFunction SerializeDepthAttachment{ DefaultSerializeDepthAttachment };                         /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeStencilAttachmentsFunction SerializeStencilAttachments{ DefaultSerializeStencilAttachments };                /* Feel free to assign this your own function, if you need custom serialization */
        inline SerializeDepthStencilAttachmentsFunction SerializeDepthStencilAttachments{ DefaultSerializeDepthStencilAttachments }; /* Feel free to assign this your own function, if you need custom serialization */
    }
}

namespace FrameBufferSerializer::Helpers {
    void SerializeAttachment(tinyxml2::XMLPrinter& printer, const FrameBufferAttachment& attachment) noexcept;
}