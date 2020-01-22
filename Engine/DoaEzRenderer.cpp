// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::ezrender {
	using namespace internal::ezrender;

	void Translate(const glm::vec3& offset) {
		*translation = offset;
	}
	void Translate(const float x, const float y, const float z) {
		translation->x = x;
		translation->y = y;
		translation->z = z;
	}
	void TranslateX(const float x) {
		translation->x = x;
	}
	void TranslateY(const float y) {
		translation->y = y;
	}
	void TranslateZ(const float z) {
		translation->z = z;
	}

	void Rotate(const glm::vec3& amount) {
		*rotation = amount;
	}
	void Rotate(const float x, const float y, const float z) {
		rotation->x = x;
		rotation->y = y;
		rotation->z = z;
	}
	void RotateX(const float x) {
		rotation->x = x;
	}
	void RotateY(const float y) {
		rotation->y = y;
	}
	void RotateZ(const float z) {
		rotation->z = z;
	}

	void Scale(const glm::vec2& factor) {
		*scale = factor;
	}
	void Scale(const float x, const float y) {
		scale->x = x;
		scale->y = y;
	}
	void ScaleX(const float x) {
		scale->x = x;
	}
	void ScaleY(const float y) {
		scale->y = y;
	}

	void Color(const glm::vec3& c) {
		*color = glm::vec4(c, 1);
		isUsingColor = true;
	}
	void Color(const glm::vec4& c) {
		*color = c;
		isUsingColor = true;
	}
	void Color(const float r, const float g, const float b, const float a) {
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = a;
		isUsingColor = true;
	}
	void ColorR(float r) {
		color->r = r;
		isUsingColor = true;
	}
	void ColorG(float g) {
		color->g = g;
		isUsingColor = true;
	}
	void ColorB(float b) {
		color->b = b;
		isUsingColor = true;
	}
	void ColorA(float a) {
		color->a = a;
		isUsingColor = true;
	}

	void Shader(shader::ShaderProgram* const s) {
		sha = s;
	}

	void Texture(texture::Texture* const t) {
		tex = t;
		isUsingTexture = true;
		isUsingAnimation = false;
	}

	void Animation(animation::Animation* const a) {
		anim = a;
		isUsingAnimation = true;
		isUsingTexture = false;
	}

	void Shape(const enum Shape s) {
		shape = s;
	}

	void Mode(const enum Mode m) {
		mode = m;
	}

	void Render(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) {
		primitives::Primitive *p{ 0 };
		switch (shape) {
		case TRIANGLE:
			p = base_triangle;
			break;
		case SQUARE:
			p = base_square;
			break;
		case PENTAGON:
			p = base_pentagon;
			break;
		case HEXAGON:
			p = base_hexagon;
			break;
		case HEPTAGON:
			p = base_heptagon;
			break;
		case OCTAGON:
			p = base_octagon;
			break;
		case NONAGON:
			p = base_nonagon;
			break;
		case DECAGON:
			p = base_decagon;
			break;
		case CIRCLE:
			p = base_circle;
			break;
		}
		p->SetPosition(*translation);
		p->SetRotation(*rotation);
		p->SetScale(*scale);
		if (isUsingColor) {
			p->SetAllVerticesColors(*color);
			if (isUsingTexture) {
				p->SetTexture(tex);
				p->SetDisplayMode(primitives::COLOR_OVER_TEXTURE);
			} else if(isUsingAnimation) {
				p->SetAnimation(anim);
				p->SetDisplayMode(primitives::COLOR_OVER_ANIMATION);
			} else {
				p->SetDisplayMode(primitives::COLOR);
			}
		} else {
			if (isUsingTexture) {
				p->SetTexture(tex);
				p->SetDisplayMode(primitives::TEXTURE);
			} else if (isUsingAnimation) {
				p->SetAnimation(anim);
				p->SetDisplayMode(primitives::ANIMATION);
			} else {
				p->SetAllVerticesColors(glm::vec3(0, 0, 0));
				p->SetDisplayMode(primitives::COLOR);
			}
		}
		if (sha) {
			p->SetShaderProgram(sha);
		}
		p->render(parent, objects, lights);
		reset_primitive(*p);
	}

	void Reset() {
		*translation = glm::vec3(0, 0, 0);
		*rotation = glm::vec3(0, 0, 0);
		*scale = glm::vec3(0, 0, 0);
		*color = glm::vec4(0, 0, 0, 1);
		sha = NULL;
		tex = NULL;
		anim = NULL;
		shape = SQUARE;
		mode = FILL;
		isUsingColor = false;
		isUsingTexture = false;
		isUsingAnimation = false;
	}
}

namespace internal::ezrender {

	glm::vec3* translation{ new glm::vec3() };
	glm::vec3* rotation{ new glm::vec3() };
	glm::vec2* scale{ new glm::vec2(1, 1) };
	glm::vec4* color{ new glm::vec4(-1, -1, -1, 1) };
	doa::shader::ShaderProgram* sha{ NULL };
	doa::texture::Texture* tex{ NULL };
	doa::animation::Animation* anim{ NULL };
	enum doa::ezrender::Shape shape { doa::ezrender::SQUARE };
	enum doa::ezrender::Mode mode {	doa::ezrender::FILL	};

	bool isUsingColor = false;
	bool isUsingTexture = false;
	bool isUsingAnimation = false;

	doa::primitives::Primitive* base_triangle { NULL };
	doa::primitives::Primitive* base_square   { NULL };
	doa::primitives::Primitive* base_pentagon { NULL };
	doa::primitives::Primitive* base_hexagon  { NULL };
	doa::primitives::Primitive* base_heptagon { NULL };
	doa::primitives::Primitive* base_octagon  { NULL };
	doa::primitives::Primitive* base_nonagon  { NULL };
	doa::primitives::Primitive* base_decagon  { NULL };
	doa::primitives::Primitive* base_circle   { NULL };

	void instantiate_primitives() {
		base_triangle = { new doa::primitives::Primitive(doa::primitives::TRIANGLE) };
		base_square   = { new doa::primitives::Primitive(doa::primitives::SQUARE)   };
		base_pentagon = { new doa::primitives::Primitive(doa::primitives::PENTAGON) };
		base_hexagon  = { new doa::primitives::Primitive(doa::primitives::HEXAGON)  };
		base_heptagon = { new doa::primitives::Primitive(doa::primitives::HEPTAGON) };
		base_octagon  = { new doa::primitives::Primitive(doa::primitives::OCTAGON)  };
		base_nonagon  = { new doa::primitives::Primitive(doa::primitives::NONAGON)  };
		base_decagon  = { new doa::primitives::Primitive(doa::primitives::DECAGON)  };
		base_circle   = { new doa::primitives::Primitive(doa::primitives::CIRCLE)   };
	}

	void reset_primitive(doa::primitives::Primitive& p) {
		p.SetPosition(0.f, 0.f);
		p.SetRotation(0.f, 0.f, 0.f);
		p.SetScale(1.f, 1.f);
		p.SetAllVerticesColors(glm::vec4(0, 0, 0, 1));
		p.SetShaderProgram(doa::shader::SHADERS["primitives-shader"]);
		p.SetTexture(0);
		p.SetAnimation(0);
		p.SetDisplayMode(doa::primitives::COLOR);
		p.SetRenderMode(doa::primitives::FILL);
	}

	void purge() {
		delete translation;
		delete rotation;
		delete scale;
		delete color;

		delete base_triangle;
		delete base_square;
		delete base_pentagon;
		delete base_hexagon;
		delete base_heptagon;
		delete base_octagon;
		delete base_nonagon;
		delete base_decagon;
		delete base_circle;
	}
}