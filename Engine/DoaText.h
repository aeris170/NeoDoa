// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace internal::text { void purge(); }

namespace doa::text {

	class Font;

	extern std::map<std::string, Font*> FONTS;

	DOA_API Font* CreateANSIFont(const char* name, const char* path, int atlasSize = 256, int charCount = 255);
	DOA_API Font* CreateUnicodeFont(const char* name, const char* path, int atlasSize = 2048, int charCount = 65535);

	DOA_API Font* Get(const std::string& name);

	class DOA_API Font {
		int m_sdf_size{ 0 };
		texture::Texture* m_sdf_tex{ NULL };

		std::map<char, SDF::sdf_glyph*>* m_chars{ new std::map<char, SDF::sdf_glyph*> };

		Font();
		~Font();

		friend class Text;
		friend DOA_API Font* CreateANSIFont(const char* name, const char* path, int atlasSize, int charCount);
		friend DOA_API Font* CreateUnicodeFont(const char* name, const char* path, int atlasSize, int charCount);
		friend void internal::text::purge();
	};

	class DOA_API Text : public scene::GameObject {
	public:
		enum ReferencePoint { TOP_RIGHT, TOP_LEFT, BOTTOM_RIGHT, BOTTOM_LEFT, CENTER };

	private:
		Font* m_font{ NULL };
		ReferencePoint m_ref_point{ BOTTOM_LEFT };

		const char* m_data{ NULL };
		int m_data_length{ 0 };

		float m_data_max_x{ 0 };
		float m_data_min_y{ 0 };
		float m_data_max_y{ 0 };

		GLuint m_VAO{ 0 };
		GLuint m_vertices_VBO{ 0 };
		GLuint m_tex_coords_VBO{ 0 };

		glm::vec3* m_color{ new glm::vec3(0.f, 0.f, 0.f) };
		glm::vec3* m_outline_color{ new glm::vec3(1.f, 1.f, 1.f) };
		glm::vec2* m_outline_offset{ new glm::vec2(0.f, 0.f) };

		float m_char_width{ .5f };
		float m_char_edge{ .1f };
		float m_char_border_width{ .4f };
		float m_char_border_edge{ .5f };

	public:
		Text(Font* font, const char* data);
		virtual ~Text();

		virtual inline const Font* GetFont() const { return m_font; }
		virtual inline void SetFont(Font* font) { m_font = font; }
		virtual inline const ReferencePoint GetReferencePoint() const { return m_ref_point; }
		virtual inline void SetReferencePoint(const ReferencePoint refPoint) { m_ref_point = refPoint; }
		virtual inline const char* GetData() const { return m_data; }
		virtual inline const glm::vec3* GetTextColor() const { return m_color; }
		virtual inline void SetTextColor(const glm::vec3& color) { *m_color = color; }
		virtual inline void SetTextColor(const float r, const float g, const float b) { *m_color = glm::vec3(r, g, b); }
		virtual inline const glm::vec3* GetOutlineColor() const { return m_outline_color; }
		virtual inline void SetOutlineColor(const glm::vec3& outlineColor) { *m_outline_color = outlineColor; }
		virtual inline void SetOutlineColor(const float r, const float g, const float b) { *m_outline_color = glm::vec3(r, g, b); }
		virtual inline const glm::vec2* GetOutlineOffset() const { return m_outline_offset; }
		virtual inline void SetOutlineOffset(const glm::vec2& outlineOffset) { *m_outline_offset = outlineOffset; }
		virtual inline const float GetCharWidth() const { return m_char_width; }
		virtual inline void SetCharWidth(const float charWidth) { m_char_width = charWidth; }
		virtual inline const float GetCharEdge() const { return m_char_edge; }
		virtual inline void SetCharEdge(const float charEdge) { m_char_edge = charEdge; }
		virtual inline const float GetCharBorderWidth() const { return m_char_border_width; }
		virtual inline void SetCharBorderWidth(const float charBorderWidth) { m_char_border_width = charBorderWidth; }
		virtual inline const float GetCharBorderEdge() const { return m_char_border_edge; }
		virtual inline void SetCharBorderEdge(const float charBorderEdge) { m_char_border_edge = charBorderEdge; }

		virtual void update(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) override;
		virtual void render(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) const override;
	};
}

namespace internal::text {

	typedef FT_Library FontLoader;

	extern FontLoader ft;

	void purge();
}