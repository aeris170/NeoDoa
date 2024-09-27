#include <Engine/FrameBufferDeserializer.hpp>

#include <algorithm>

#include <Engine/Log.hpp>
#include <Engine/Graphics.hpp>
#include <Engine/FileNode.hpp>

FrameBufferDeserializationResult DeserializeFrameBuffer(const FNode& file) noexcept {
    file.ReadContent();
    auto rv = DeserializeFrameBuffer(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedFrameBuffer.Name = file.Name();
    }
    return rv;
}

FrameBufferDeserializationResult DeserializeFrameBuffer(const std::string_view data) noexcept {
    FrameBufferDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't deserialize framebuffer!");
        rv.errors.emplace_back("This should normally never happen. If you didn't try to edit the file manually, please submit an issue.");
        DOA_LOG_ERROR("Couldn't deserialize framebuffer!\n\n%s", data);
    } else {
        FrameBufferDeserializer::Deserialize(*doc.RootElement(), rv);
    }

    return rv;
}

void FrameBufferDeserializer::DefaultDeserialize(const tinyxml2::XMLElement& rootElem, FrameBufferDeserializationResult& fbdr) noexcept {
    Attachments::DefaultDeserializeColorAttachments(*rootElem.FirstChildElement("color-attachments"), fbdr);
    Attachments::DefaultDeserializeDepthAttachment(*rootElem.FirstChildElement("depth-attachment"), fbdr);
    Attachments::DefaultDeserializeStencilAttachment(*rootElem.FirstChildElement("stencil-attachment"), fbdr);
    Attachments::DefaultDeserializeDepthStencilAttachment(*rootElem.FirstChildElement("depth-stencil-attachment"), fbdr);
}

void FrameBufferDeserializer::Attachments::DefaultDeserializeColorAttachments(const tinyxml2::XMLElement& attachmentsElem, FrameBufferDeserializationResult& fbdr) noexcept {
    for (const tinyxml2::XMLElement* attachmentElem = attachmentsElem.FirstChildElement(); attachmentElem != nullptr; attachmentElem = attachmentElem->NextSiblingElement()) {
        auto adr = Helpers::DeserializeAttachment(*attachmentElem);
        if (adr.erred) {
            fbdr.errors.reserve(fbdr.errors.size() + adr.errors.size());
            std::ranges::move(adr.errors, std::back_inserter(fbdr.errors));
            return;
        }
        fbdr.deserializedFrameBuffer.ColorAttachments.emplace_back(std::move(adr.deserializedAttachment));
    }
}
void FrameBufferDeserializer::Attachments::DefaultDeserializeDepthAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept {
    bool hasValue{ false };
    tinyxml2::XMLError err = attachmentElem.QueryAttribute("has-value", &hasValue);
    if (err != tinyxml2::XML_SUCCESS) {
        fbdr.erred = true;
        fbdr.errors.emplace_back("Cannot deserialize depth attachment.");
        fbdr.errors.emplace_back("Couldn't extract has-value. Data is corrupted.");
        DOA_LOG_ERROR("Cannot deserialize depth attachment.");
        DOA_LOG_ERROR("Couldn't extract has-value. Data is corrupted.");
        return;
    }

    if (hasValue) {
        auto adr = Helpers::DeserializeAttachment(attachmentElem);
        if (adr.erred) {
            fbdr.errors.reserve(fbdr.errors.size() + adr.errors.size());
            std::ranges::move(adr.errors, std::back_inserter(fbdr.errors));
            return;
        }
        fbdr.deserializedFrameBuffer.DepthAttachment.emplace(std::move(adr.deserializedAttachment));
    }
}
void FrameBufferDeserializer::Attachments::DefaultDeserializeStencilAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept {
    bool hasValue{ false };
    tinyxml2::XMLError err = attachmentElem.QueryAttribute("has-value", &hasValue);
    if (err != tinyxml2::XML_SUCCESS) {
        fbdr.erred = true;
        fbdr.errors.emplace_back("Cannot deserialize stencil attachment.");
        fbdr.errors.emplace_back("Couldn't extract has-value. Data is corrupted.");
        DOA_LOG_ERROR("Cannot deserialize stencil attachment.");
        DOA_LOG_ERROR("Couldn't extract has-value. Data is corrupted.");
        return;
    }

    if (hasValue) {
        auto adr = Helpers::DeserializeAttachment(attachmentElem);
        if (adr.erred) {
            fbdr.errors.reserve(fbdr.errors.size() + adr.errors.size());
            std::ranges::move(adr.errors, std::back_inserter(fbdr.errors));
            return;
        }
        fbdr.deserializedFrameBuffer.StencilAttachment.emplace(std::move(adr.deserializedAttachment));
    }
}
void FrameBufferDeserializer::Attachments::DefaultDeserializeDepthStencilAttachment(const tinyxml2::XMLElement& attachmentElem, FrameBufferDeserializationResult& fbdr) noexcept {
    bool hasValue{ false };
    tinyxml2::XMLError err = attachmentElem.QueryAttribute("has-value", &hasValue);
    if (err != tinyxml2::XML_SUCCESS) {
        fbdr.erred = true;
        fbdr.errors.emplace_back("Cannot deserialize depth-stencil attachment.");
        fbdr.errors.emplace_back("Couldn't extract has-value. Data is corrupted.");
        DOA_LOG_ERROR("Cannot deserialize depth-stencil attachment.");
        DOA_LOG_ERROR("Couldn't extract has-value. Data is corrupted.");
        return;
    }

    if (hasValue) {
        auto adr = Helpers::DeserializeAttachment(attachmentElem);
        if (adr.erred) {
            fbdr.errors.reserve(fbdr.errors.size() + adr.errors.size());
            std::ranges::move(adr.errors, std::back_inserter(fbdr.errors));
            return;
        }
        fbdr.deserializedFrameBuffer.DepthStencilAttachment.emplace(std::move(adr.deserializedAttachment));
    }
}

FrameBufferAttachmentSerializationResult FrameBufferDeserializer::Helpers::DeserializeAttachment(const tinyxml2::XMLElement& attachmentElem) noexcept {
    FrameBufferAttachmentSerializationResult rv;

    tinyxml2::XMLError err;
    err = attachmentElem.QueryAttribute("width", &rv.deserializedAttachment.Resolution.Width);
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't extract width. Data is corrupted.");
        DOA_LOG_ERROR("Couldn't extract width. Data is corrupted.");
        return rv;
    }

    err = attachmentElem.QueryAttribute("height", &rv.deserializedAttachment.Resolution.Height);
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't extract height. Data is corrupted.");
        DOA_LOG_ERROR("Couldn't extract height. Data is corrupted.");
        return rv;
    }

    const char* dataFormatString;
    err = attachmentElem.QueryAttribute("format", &dataFormatString);
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't extract format. Data is corrupted.");
        DOA_LOG_ERROR("Couldn't extract format. Data is corrupted.");
        return rv;
    }
    rv.deserializedAttachment.Format = DataFormatFromString(dataFormatString);

    const char* samplesString;
    err = attachmentElem.QueryAttribute("samples", &samplesString);
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't extract samples. Data is corrupted.");
        DOA_LOG_ERROR("Couldn't extract samples. Data is corrupted.");
        return rv;
    }
    rv.deserializedAttachment.Samples = MultisampleFromString(samplesString);

    err = attachmentElem.QueryAttribute("is-readable-from-CPU", &rv.deserializedAttachment.HasReadbackSupport);
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't extract is-readable-from-CPU. Data is corrupted.");
        DOA_LOG_ERROR("Couldn't extract is-readable-from-CPU. Data is corrupted.");
        return rv;
    }

    return rv;
}
