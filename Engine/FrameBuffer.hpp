#pragma once

#include "TypedefsAndConstants.hpp"

#include "Resolution.hpp"
#include "Color.hpp"

struct FrameBuffer {
	Color ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };

	Resolution _resolution;

	FBO _fbo{ 0 };
	TEX _tex{ 0 };
	RBO _rbo{ 0 };

	FrameBuffer(Resolution resolution);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&);
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer& operator=(FrameBuffer&&);

	void DeAlloc();

	void Bind() const;
	void UnBind() const;
	void UnBind(Resolution oldViewport) const;

	void ClearBuffers() const;
	void ClearColorBuffer() const;
	void ClearDepthBuffer() const;
	void ClearStencilBuffer() const;
};
