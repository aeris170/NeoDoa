#include <Engine/FrameBufferSerializer.hpp>

#include <algorithm>

#include <tinyxml2.h>

std::string SerializeFrameBuffer(const FrameBuffer& frameBuffer) noexcept {
    tinyxml2::XMLPrinter printer;
    FrameBufferSerializer::HeaderComment(printer, frameBuffer);
    FrameBufferSerializer::Serialize(printer, frameBuffer);

    return printer.CStr();
}

void FrameBufferSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const FrameBuffer& frameBuffer) noexcept {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
}
void FrameBufferSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const FrameBuffer& frameBuffer) noexcept {
    printer.OpenElement("framebuffer");
    {
        Attachments::SerializeColorAttachments(printer, frameBuffer.ColorAttachments);
        Attachments::SerializeDepthAttachment(printer, frameBuffer.DepthAttachment);
        Attachments::SerializeStencilAttachments(printer, frameBuffer.StencilAttachment);
        Attachments::SerializeDepthStencilAttachments(printer, frameBuffer.DepthStencilAttachment);
    }
    printer.CloseElement();
}

void FrameBufferSerializer::Attachments::DefaultSerializeColorAttachments(tinyxml2::XMLPrinter& printer, const std::vector<FrameBufferAttachment>& colorAttachments) noexcept {
    printer.OpenElement("color-attachments");
    std::ranges::for_each(colorAttachments, [&printer](const FrameBufferAttachment& attachment) {
        printer.OpenElement("attachment");
        Helpers::SerializeAttachment(printer, attachment);
        printer.CloseElement();
    });
    printer.CloseElement();
}
void FrameBufferSerializer::Attachments::DefaultSerializeDepthAttachment(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthAttachment) noexcept {
    printer.OpenElement("depth-attachment");
    bool hasValue = depthAttachment.has_value();
    printer.PushAttribute("has-value", hasValue);
    if (hasValue) {
        Helpers::SerializeAttachment(printer, depthAttachment.value());
    }
    printer.CloseElement();
}
void FrameBufferSerializer::Attachments::DefaultSerializeStencilAttachments(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& stencilAttachment) noexcept {
    printer.OpenElement("stencil-attachment");
    bool hasValue = stencilAttachment.has_value();
    printer.PushAttribute("has-value", hasValue);
    if (hasValue) {
        Helpers::SerializeAttachment(printer, stencilAttachment.value());
    }
    printer.CloseElement();
}
void FrameBufferSerializer::Attachments::DefaultSerializeDepthStencilAttachments(tinyxml2::XMLPrinter& printer, const std::optional<FrameBufferAttachment>& depthStencilAttachment) noexcept {
    printer.OpenElement("depth-stencil-attachment");
    bool hasValue = depthStencilAttachment.has_value();
    printer.PushAttribute("has-value", hasValue);
    if (hasValue) {
        Helpers::SerializeAttachment(printer, depthStencilAttachment.value());
    }
    printer.CloseElement();
}

void FrameBufferSerializer::Helpers::SerializeAttachment(tinyxml2::XMLPrinter& printer, const FrameBufferAttachment& attachment) noexcept {
    printer.PushAttribute("width", attachment.Resolution.Width);
    printer.PushAttribute("height", attachment.Resolution.Height);
    printer.PushAttribute("format", ToString(attachment.Format).data());
    printer.PushAttribute("samples", ToString(attachment.Samples).data());
    printer.PushAttribute("is-readable-from-CPU", attachment.HasReadbackSupport);
}
