// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::ezrender {

	enum Shape { TRIANGLE, SQUARE, PENTAGON, HEXAGON, HEPTAGON, OCTAGON, NONAGON, DECAGON, CIRCLE };
	enum Mode { POINTS, DRAW, FILL };

	DOA_API void Translate(const glm::vec3& offset);
	DOA_API void TranslateX(const float x);
	DOA_API void TranslateY(const float y);
	DOA_API void TranslateZ(const float z);

	DOA_API void Rotate(const glm::vec3& amount);
	DOA_API void RotateX(const float x);
	DOA_API void RotateY(const float y);
	DOA_API void RotateZ(const float z);

	DOA_API void Scale(const glm::vec2& factor);
	DOA_API void ScaleX(const float x);
	DOA_API void ScaleY(const float y);

	DOA_API void Color(const glm::vec3& color);
	DOA_API void Color(const glm::vec4& color);
	DOA_API void ColorR(const float r);
	DOA_API void ColorG(const float g);
	DOA_API void ColorB(const float b);
	DOA_API void ColorA(const float a);

	DOA_API void Texture(const texture::Texture* const t);

	DOA_API void Animation(const animation::Animation* const a);

	DOA_API void Shape(const enum Shape s);

	DOA_API void Mode(const enum Mode m);

	DOA_API void Render(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights);

	DOA_API void Reset();
}

namespace internal::ezrender {

	extern glm::vec3 *translation;
	extern glm::vec3 *rotation;
	extern glm::vec2 *scale;
	extern glm::vec4 *color;
	extern doa::texture::Texture *tex;
	extern doa::animation::Animation *anim;
	extern enum doa::ezrender::Shape shape;
	extern enum doa::ezrender::Mode mode;

	extern bool isUsingColor;
	extern bool isUsingTexture;
	extern bool isUsingAnimation;

	extern doa::primitives::Primitive *base_triangle;
	extern doa::primitives::Primitive *base_square;
	extern doa::primitives::Primitive *base_pentagon;
	extern doa::primitives::Primitive *base_hexagon;
	extern doa::primitives::Primitive *base_heptagon;
	extern doa::primitives::Primitive *base_octagon;
	extern doa::primitives::Primitive *base_nonagon;
	extern doa::primitives::Primitive *base_decagon;
	extern doa::primitives::Primitive *base_circle;

	void instantiate_primitives();
	void reset_primitive(doa::primitives::Primitive& p);

	void purge();
}