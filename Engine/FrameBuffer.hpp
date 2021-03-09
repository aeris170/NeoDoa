#pragma once

#include "TypedefsAndConstants.hpp"

struct FrameBuffer {
	int _width;
	int _height;

	FBO _fbo{ 0 };
	TEX _tex{ 0 };
	RBO _rbo{ 0 };

	FrameBuffer(int width, int height);
	~FrameBuffer();
private:
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	// move semantics are default
};
