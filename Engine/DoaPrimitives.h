// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::primitives {

	enum Type { TRIANGLE = 3, SQUARE = 4, PENTAGON = 5, HEXAGON = 6, HEPTAGON = 7, OCTAGON = 8, NONAGON = 9, DECAGON = 10, CIRCLE = 100 };
	enum RenderMode { POINTS, DRAW, FILL };
	enum DisplayMode { COLOR, TEXTURE, ANIMATION, COLOR_OVER_TEXTURE, COLOR_OVER_ANIMATION};

	class DOA_API Primitive : public scene::GameObject {
	private:
		Type m_type;
		RenderMode m_rMode;
		DisplayMode m_dMode;

		bool m_is_fixed{ false };

		GLfloat *m_colors{ NULL };
		int m_colors_count{ 0 };
		mutable bool m_colors_changed{ false };

		int m_indices_count{ 0 };
		int m_line_indices_count{ 0 };

		GLuint m_VAO{ 0 };
		GLuint m_vertices_VBO{ 0 };
		GLuint m_tex_coords_VBO{ 0 };
		GLuint m_colors_VBO{ 0 };
		GLuint m_indices_IBO{ 0 };
		GLuint m_line_indices_IBO{ 0 };

		texture::Texture *m_texture{ 0 };

		animation::Animation *m_anim{ 0 };
		mutable animation::AnimationFrame *m_current_animation_frame{ 0 };
		mutable int m_current_animation_index{ 0 };

		void next_frame() const;

	public:
		Primitive(Type type, RenderMode rMode, DisplayMode dMode);
		virtual ~Primitive();

		virtual inline const Type GetType() const { return m_type; }
		virtual inline void SetType(const Type type) { throw "not implemented"; }
		virtual inline const RenderMode GetRenderMode() const { return m_rMode; }
		virtual inline void SetRenderMode(const RenderMode rMode) { m_rMode = rMode; }
		virtual inline const DisplayMode GetDisplayMode() const { return m_dMode; }
		virtual inline void SetDisplayMode(const DisplayMode dMode) { m_dMode = dMode; }
		virtual inline const bool IsFixed() const { return m_is_fixed; }
		virtual inline void SetFixed(const bool isFixed) { m_is_fixed = isFixed; }
		virtual inline float* GetVertexColors() const { return m_colors; }
		virtual inline void SetVertexColor(const int vertex_index, const glm::vec3& color) {
			if (vertex_index < 0 || vertex_index > m_type) return;
			int i{ vertex_index * 4 };
			m_colors[i] = color.r;
			m_colors[i + 1] = color.g;
			m_colors[i + 2] = color.b;
			m_colors_changed = true;
		}
		virtual inline void SetVertexColor(const int vertex_index, const glm::vec4& color) {
			if (vertex_index < 0 || vertex_index > m_type) return;
			int i{ vertex_index * 4 };
			m_colors[i] = color.r;
			m_colors[i + 1] = color.g;
			m_colors[i + 2] = color.b;
			m_colors[i + 3] = color.a;
			m_colors_changed = true;
		}
		virtual inline void SetAllVerticesColors(const glm::vec3& color) {
			for (int i{ 0 }; i < m_colors_count; i += 4) {
				m_colors[i] = color.r;
				m_colors[i + 1] = color.g;
				m_colors[i + 2] = color.b;
			}
			m_colors_changed = true;
		}
		virtual inline void SetAllVerticesColors(const glm::vec4& color) {
			for (int i{ 0 }; i < m_colors_count; i += 4) {
				m_colors[i] = color.r;
				m_colors[i + 1] = color.g;
				m_colors[i + 2] = color.b;
				m_colors[i + 3] = color.a;
			}
			m_colors_changed = true;
		}
		virtual inline const texture::Texture* GetTextureID() const { return m_texture; }
		virtual inline void SetTexture(texture::Texture *texture) { m_texture = texture; }

		virtual inline const animation::Animation* GetAnimation() const { return m_anim; }
		virtual inline void SetAnimation(animation::Animation *anim) { m_anim = anim; m_current_animation_frame = (*anim->frames)[0]; }

		virtual void update(const scene::Scene& parent, const std::vector<GameObject*>* const objects, const std::vector<scene::Light*>* const lights) const;
		virtual void render(const scene::Scene& parent, const std::vector<GameObject*>* const objects, const std::vector<scene::Light*>* const lights) const;

	private:
		static GLfloat s_triangle_vertices[];
		static int     s_triangle_vertices_count;
		static GLuint  s_triangle_indices[];
		static int     s_triangle_indices_count;
		static GLuint  s_triangle_line_indices[];
		static int     s_triangle_line_indices_count;
		static GLfloat s_triangle_texture_coordinates[];
		static int     s_triangle_texture_coordinates_count;
		static GLuint  s_triangle_VAO;
		static GLuint  s_triangle_vertices_VBO;
		static GLuint  s_triangle_tex_coords_VBO;
		static GLuint  s_triangle_indices_IBO;
		static GLuint  s_triangle_line_indices_IBO;

		static GLfloat s_square_vertices[];
		static int     s_square_vertices_count;
		static GLuint  s_square_indices[];
		static int     s_square_indices_count;
		static GLuint  s_square_line_indices[];
		static int     s_square_line_indices_count;
		static GLfloat s_square_texture_coordinates[];
		static int     s_square_texture_coordinates_count;
		static GLuint  s_square_VAO;
		static GLuint  s_square_vertices_VBO;
		static GLuint  s_square_tex_coords_VBO;
		static GLuint  s_square_indices_IBO;
		static GLuint  s_square_line_indices_IBO;

		static GLfloat s_pentagon_vertices[];
		static int     s_pentagon_vertices_count;
		static GLuint  s_pentagon_indices[];
		static int     s_pentagon_indices_count;
		static GLuint  s_pentagon_line_indices[];
		static int     s_pentagon_line_indices_count;
		static GLfloat s_pentagon_texture_coordinates[];
		static int     s_pentagon_texture_coordinates_count;
		static GLuint  s_pentagon_VAO;
		static GLuint  s_pentagon_vertices_VBO;
		static GLuint  s_pentagon_tex_coords_VBO;
		static GLuint  s_pentagon_indices_IBO;
		static GLuint  s_pentagon_line_indices_IBO;

		static GLfloat s_hexagon_vertices[];
		static int     s_hexagon_vertices_count;
		static GLuint  s_hexagon_indices[];
		static int     s_hexagon_indices_count;
		static GLuint  s_hexagon_line_indices[];
		static int     s_hexagon_line_indices_count;
		static GLfloat s_hexagon_texture_coordinates[];
		static int     s_hexagon_texture_coordinates_count;
		static GLuint  s_hexagon_VAO;
		static GLuint  s_hexagon_vertices_VBO;
		static GLuint  s_hexagon_tex_coords_VBO;
		static GLuint  s_hexagon_indices_IBO;
		static GLuint  s_hexagon_line_indices_IBO;

		static GLfloat s_heptagon_vertices[];
		static int     s_heptagon_vertices_count;
		static GLuint  s_heptagon_indices[];
		static int     s_heptagon_indices_count;
		static GLuint  s_heptagon_line_indices[];
		static int     s_heptagon_line_indices_count;
		static GLfloat s_heptagon_texture_coordinates[];
		static int     s_heptagon_texture_coordinates_count;
		static GLuint  s_heptagon_VAO;
		static GLuint  s_heptagon_vertices_VBO;
		static GLuint  s_heptagon_tex_coords_VBO;
		static GLuint  s_heptagon_indices_IBO;
		static GLuint  s_heptagon_line_indices_IBO;

		static GLfloat s_octagon_vertices[];
		static int     s_octagon_vertices_count;
		static GLuint  s_octagon_indices[];
		static int     s_octagon_indices_count;
		static GLuint  s_octagon_line_indices[];
		static int     s_octagon_line_indices_count;
		static GLfloat s_octagon_texture_coordinates[];
		static int     s_octagon_texture_coordinates_count;
		static GLuint  s_octagon_VAO;
		static GLuint  s_octagon_vertices_VBO;
		static GLuint  s_octagon_tex_coords_VBO;
		static GLuint  s_octagon_indices_IBO;
		static GLuint  s_octagon_line_indices_IBO;

		static GLfloat s_nonagon_vertices[];
		static int     s_nonagon_vertices_count;
		static GLuint  s_nonagon_indices[];
		static int     s_nonagon_indices_count;
		static GLuint  s_nonagon_line_indices[];
		static int     s_nonagon_line_indices_count;
		static GLfloat s_nonagon_texture_coordinates[];
		static int     s_nonagon_texture_coordinates_count;
		static GLuint  s_nonagon_VAO;
		static GLuint  s_nonagon_vertices_VBO;
		static GLuint  s_nonagon_tex_coords_VBO;
		static GLuint  s_nonagon_indices_IBO;
		static GLuint  s_nonagon_line_indices_IBO;

		static GLfloat s_decagon_vertices[];
		static int     s_decagon_vertices_count;
		static GLuint  s_decagon_indices[];
		static int     s_decagon_indices_count;
		static GLuint  s_decagon_line_indices[];
		static int     s_decagon_line_indices_count;
		static GLfloat s_decagon_texture_coordinates[];
		static int     s_decagon_texture_coordinates_count;
		static GLuint  s_decagon_VAO;
		static GLuint  s_decagon_vertices_VBO;
		static GLuint  s_decagon_tex_coords_VBO;
		static GLuint  s_decagon_indices_IBO;
		static GLuint  s_decagon_line_indices_IBO;

		static GLfloat s_circle_vertices[];
		static int     s_circle_vertices_count;
		static GLuint  s_circle_indices[];
		static int     s_circle_indices_count;
		static GLuint  s_circle_line_indices[];
		static int     s_circle_line_indices_count;
		static GLfloat s_circle_texture_coordinates[];
		static int     s_circle_texture_coordinates_count;
		static GLuint  s_circle_VAO;
		static GLuint  s_circle_vertices_VBO;
		static GLuint  s_circle_tex_coords_VBO;
		static GLuint  s_circle_indices_IBO;
		static GLuint  s_circle_line_indices_IBO;

		static void get_buffers(Primitive* p);

	public:
		static void generate_buffers();
		static void purge();
	};
}