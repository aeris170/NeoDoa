// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa {
	namespace primitive {

		Primitive::Primitive(Type type, RenderMode rMode, DisplayMode dMode) : GameObject(), m_type{ type }, m_rMode{ rMode }, m_dMode{ dMode } {
			get_buffers(this);
			m_colors_count = (type + 1) * 4;
			m_colors = new float[m_colors_count];
			for (int i = 0; i < m_colors_count; i += 4) {
				m_colors[i] = 0;
				m_colors[i + 1] = 0;
				m_colors[i + 2] = 0;
				m_colors[i + 3] = 1;
			}

			m_shader = doa::shader::SHADERS["primitives-shader"];

			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertices_VBO);

			GLint pos = glGetAttribLocation(*m_shader, "vertex_coords");
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(pos);

			glGenBuffers(1, &m_colors_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_colors_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_colors_count, m_colors, GL_STATIC_DRAW);

			GLint color = glGetAttribLocation(*m_shader, "vertex_color");
			glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(color);

			glBindBuffer(GL_ARRAY_BUFFER, m_tex_coords_VBO);

			GLint tex_coords = glGetAttribLocation(*m_shader, "texture_coords");
			glVertexAttribPointer(tex_coords, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(tex_coords);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		Primitive::~Primitive()
		{
			glDeleteBuffers(1, &m_colors_VBO);
			delete[] m_colors;
		}

		void Primitive::update(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) {}

		void Primitive::render(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) const {
			glUseProgram(*m_shader);
			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, m_colors_VBO);
			GLint color = glGetAttribLocation(*m_shader, "vertex_color");
			glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(color);

			if (m_colors_changed) {
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_colors_count, m_colors);
				m_colors_changed = false;
			}

			glm::mat4 modelMatrix = glm::mat4(1);																// A	Camera space transforms are complete.
			if (!m_is_fixed) {																					// |
				modelMatrix = glm::translate(modelMatrix, glm::vec3(camera::x, camera::y, 0));					// |	Third, translate the object back to it's position.
				modelMatrix = glm::scale(modelMatrix, glm::vec3(camera::z, camera::z, 1));						// |	Second, zoom into the camera origin.
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-camera::x, -camera::y, 0));				// |	First, translate the object to camera origin.
			}																									// |
																												// |	Object space transfroms are complete.
			modelMatrix = glm::translate(modelMatrix, *m_position);												// |	Third, translate the object.
			modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->x / 180.f * M_PI), glm::vec3(1, 0, 0));	// |	and round X.
			modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->y / 180.f * M_PI), glm::vec3(0, 1, 0));	// |	round Y,
			modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->z / 180.f * M_PI), glm::vec3(0, 0, 1));	// |	Second, rotate the object round Z,
			modelMatrix = glm::scale(modelMatrix, glm::vec3(*m_scale, 1));										// |	First, scale the object.

			GLint modelMatrixLocation = glGetUniformLocation(*m_shader, "modelMatrix");
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			GLint viewMatrixLocation = glGetUniformLocation(*m_shader, "viewMatrix");
			if (m_is_fixed) {
				glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_fixed_view_matrix));
			}
			else {
				glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_view_matrix));
			}

			GLint projectionMatrixLocation = glGetUniformLocation(*m_shader, "projectionMatrix");
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_projection_matrix));

			GLint applyColorsLocation = glGetUniformLocation(*m_shader, "applyColors");
			GLint applyTextureLocation = glGetUniformLocation(*m_shader, "applyTexture");
			GLint sampler2DLocation = glGetUniformLocation(*m_shader, "textureSample");
			switch (m_dMode) {
			case COLOR:
				glUniform1i(applyColorsLocation, 1);
				glUniform1i(applyTextureLocation, 0);
				break;
			case TEXTURE:
				glUniform1i(applyColorsLocation, 0);
				glUniform1i(applyTextureLocation, 1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *m_texture);
				glUniform1i(sampler2DLocation, 0);
				break;
			case COLOR_OVER_TEXTURE:
				glUniform1i(applyColorsLocation, 1);
				glUniform1i(applyTextureLocation, 1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *m_texture);
				glUniform1i(sampler2DLocation, 0);
				break;
			case ANIMATION:
				glUniform1i(applyColorsLocation, 0);
				glUniform1i(applyTextureLocation, 1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *m_current_animation_frame->texture);
				next_frame();
				glUniform1i(sampler2DLocation, 0);
				break;
			case COLOR_OVER_ANIMATION:
				glUniform1i(applyColorsLocation, 1);
				glUniform1i(applyTextureLocation, 1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, *m_current_animation_frame->texture);
				next_frame();
				glUniform1i(sampler2DLocation, 0);
				break;
			}

			GLint ambientLightColorLocation = glGetUniformLocation(*m_shader, "ambientLightColor");
			glUniform3fv(ambientLightColorLocation, 1, glm::value_ptr(*scene::ACTIVE_SCENE->GetAmbientLight()));

			int lightCount = static_cast<int>(parent.LightCount());
			GLint numLightsLocation = glGetUniformLocation(*m_shader, "numLights");
			glUniform1i(numLightsLocation, lightCount);

			std::stringstream lp;
			std::stringstream lc;
			std::stringstream li;
			std::stringstream lr;
			const char pos[] = "lightPositions[";
			const char col[] = "lightColors[";
			const char intens[] = "lightIntensities[";
			const char radi[] = "lightRadii[";

			for (int i{ 0 }; i < lightCount; ++i) {
				lp.clear();
				lc.clear();
				li.clear();
				lr.clear();

				lp << pos << i << "]";
				lc << col << i << "]";
				li << intens << i << "]";
				lr << radi << i << "]";

				scene::Light* l = lights[i];

				GLint lightPositionsLocation = glGetUniformLocation(*m_shader, lp.str().c_str());
				glm::vec3 pos = *l->GetPosition();
				pos.x = camera::z * (pos.x - camera::x) + camera::x;
				pos.y = camera::z * (pos.y - camera::y) + camera::y;
				glUniform3fv(lightPositionsLocation, 1, glm::value_ptr(pos));

				GLint lightColorsLocation = glGetUniformLocation(*m_shader, lc.str().c_str());
				glUniform3fv(lightColorsLocation, 1, glm::value_ptr(*l->GetColor()));

				GLint lightIntensitiesLocation = glGetUniformLocation(*m_shader, li.str().c_str());
				glUniform1f(lightIntensitiesLocation, l->GetIntensity());

				GLint lightRadiiLocation = glGetUniformLocation(*m_shader, lr.str().c_str());
				glUniform1f(lightRadiiLocation, l->GetRadius() * camera::z);
			}

			switch (m_rMode) {
			case POINTS:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_IBO);
				glDrawElements(GL_POINTS, m_indices_count, GL_UNSIGNED_INT, 0);
				break;
			case DRAW:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_line_indices_IBO);
				glDrawElements(GL_LINES, m_line_indices_count, GL_UNSIGNED_INT, 0);
				break;
			case FILL:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_IBO);
				glDrawElements(GL_TRIANGLE_FAN, m_indices_count, GL_UNSIGNED_INT, 0);
				break;
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0); //m_color_VBO unbind here {render() + 3}
			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glUseProgram(0);
		}

		void Primitive::next_frame() const {
			static std::chrono::duration<double> elapsed;
			static auto before = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			elapsed += now - before;
			before = now;
			auto anim_duration = *m_current_animation_frame->duration;
			if (elapsed > anim_duration){
				std::vector<animation::AnimationFrame*>& frames = *m_anim->frames; //mind the &, it's a reference variable!!
				m_current_animation_frame = frames[++m_current_animation_index % frames.size()];
				elapsed -= anim_duration;
			}
		}

		GLfloat Primitive::s_triangle_vertices[12]{
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-1.f, -1.f, 0.f,
			1.f, -1.f, 0.f
		};
		int Primitive::s_triangle_vertices_count{ 12 };
		GLuint Primitive::s_triangle_indices[7]{ 0, 1, 2, 2, 3, 3, 1 };
		int Primitive::s_triangle_indices_count{ 7 };
		GLuint Primitive::s_triangle_line_indices[12]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 1 };
		int Primitive::s_triangle_line_indices_count{ 12 };
		GLfloat Primitive::s_triangle_texture_coordinates[8]{
			.5f, .5f,
			.5f, 1.f,
			0.f, 0.f,
			1.f, 0.f
		};
		int Primitive::s_triangle_texture_coordinates_count{ 8 };
		GLuint Primitive::s_triangle_VAO{ 0 };
		GLuint Primitive::s_triangle_vertices_VBO{ 0 };
		GLuint Primitive::s_triangle_tex_coords_VBO{ 0 };
		GLuint Primitive::s_triangle_indices_IBO{ 0 };
		GLuint Primitive::s_triangle_line_indices_IBO{ 0 };

		GLfloat Primitive::s_square_vertices[15]{
			0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,
			-1.f, 1.f, 0.f,
			-1.f, -1.f, 0.f,
			1.f, -1.f, 0.f
		};
		int Primitive::s_square_vertices_count = 15;
		GLuint Primitive::s_square_indices[9]{ 0, 1, 2, 2, 3, 3, 4, 4, 1 };
		int Primitive::s_square_indices_count = 9;
		GLuint Primitive::s_square_line_indices[16]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 1 };
		int Primitive::s_square_line_indices_count = 16;
		GLfloat Primitive::s_square_texture_coordinates[10]{
				.5f, .5f,
				1.f, 1.f,
				0.f, 1.f,
				0.f, 0.f,
				1.f, 0.f
		};
		int Primitive::s_square_texture_coordinates_count = 10;
		GLuint Primitive::s_square_VAO = 0;
		GLuint Primitive::s_square_vertices_VBO = 0;
		GLuint Primitive::s_square_tex_coords_VBO = 0;
		GLuint Primitive::s_square_indices_IBO = 0;
		GLuint Primitive::s_square_line_indices_IBO = 0;

		GLfloat Primitive::s_pentagon_vertices[18]{
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-.9511f, .309f, 0.f,
			-.5878f, -.809f, 0.f,
			.5878f, -.809f, 0.f,
			.9511f, .309f, 0.f
		};
		int Primitive::s_pentagon_vertices_count = 18;
		GLuint Primitive::s_pentagon_indices[11]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 1 };
		int Primitive::s_pentagon_indices_count = 11;
		GLuint Primitive::s_pentagon_line_indices[20]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 1 };
		int Primitive::s_pentagon_line_indices_count = 20;
		GLfloat Primitive::s_pentagon_texture_coordinates[12]{
			.5f, .5f,
			.5f, 1.f,
			.02445f, .6545f,
			.2061f, .0955f,
			.7939f, .0955f,
			.97555f, .6545f
		};
		int Primitive::s_pentagon_texture_coordinates_count = 12;
		GLuint Primitive::s_pentagon_VAO = 0;
		GLuint Primitive::s_pentagon_vertices_VBO = 0;
		GLuint Primitive::s_pentagon_tex_coords_VBO = 0;
		GLuint Primitive::s_pentagon_indices_IBO = 0;
		GLuint Primitive::s_pentagon_line_indices_IBO = 0;

		GLfloat Primitive::s_hexagon_vertices[21]{
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			.5f, .866f, 0.f,
			-.5f, .866f, 0.f,
			-1.f, 0.f, 0.f,
			-.5f, -.866f, 0.f,
			.5f, -.866f, 0.f
		};
		int Primitive::s_hexagon_vertices_count = 21;
		GLuint Primitive::s_hexagon_indices[13]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 1 };
		int Primitive::s_hexagon_indices_count = 13;
		GLuint Primitive::s_hexagon_line_indices[24]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 1 };
		int Primitive::s_hexagon_line_indices_count = 24;
		GLfloat Primitive::s_hexagon_texture_coordinates[14]{
			.5f, .5f,
			1.f, .5f,
			.75f, .933f,
			.25f, .933f,
			0.f, .5f,
			.25f, .067f,
			.75f, .067f
		};
		int Primitive::s_hexagon_texture_coordinates_count = 14;
		GLuint Primitive::s_hexagon_VAO = 0;
		GLuint Primitive::s_hexagon_vertices_VBO = 0;
		GLuint Primitive::s_hexagon_tex_coords_VBO = 0;
		GLuint Primitive::s_hexagon_indices_IBO = 0;
		GLuint Primitive::s_hexagon_line_indices_IBO = 0;

		GLfloat Primitive::s_heptagon_vertices[24]{
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-.7818f, .6235f, 0.f,
			-.9749f, -.2225f, 0.f,
			-.4339f, -.901f, 0.f,
			.4339f, -.901f, 0.f,
			.9749f, -.2225f, 0.f,
			.7818f, .6235f, 0.f,
		};
		int Primitive::s_heptagon_vertices_count = 24;
		GLuint Primitive::s_heptagon_indices[15]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 1 };
		int Primitive::s_heptagon_indices_count = 15;
		GLuint Primitive::s_heptagon_line_indices[28]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 7, 0, 7, 7, 1 };
		int Primitive::s_heptagon_line_indices_count = 28;
		GLfloat Primitive::s_heptagon_texture_coordinates[16]{
			.5f, .5f,
			.5f, 1.f,
			.1091f, .81175f,
			.01255f, .38875f,
			.28305f, .0495f,
			.71695f, .0495f,
			.98745f, .38875f,
			.8909f, .81175f,
		};
		int Primitive::s_heptagon_texture_coordinates_count = 16;
		GLuint Primitive::s_heptagon_VAO = 0;
		GLuint Primitive::s_heptagon_vertices_VBO = 0;
		GLuint Primitive::s_heptagon_tex_coords_VBO = 0;
		GLuint Primitive::s_heptagon_indices_IBO = 0;
		GLuint Primitive::s_heptagon_line_indices_IBO = 0;

		GLfloat Primitive::s_octagon_vertices[27]{
			0.f, 0.f, 0.f,
			.9239f, .3827f, 0.f,
			.3827f, .9239f, 0.f,
			-.3827f, .9239f, 0.f,
			-.9239f, .3827f, 0.f,
			-.9239f, -.3827f, 0.f,
			-.3827f, -.9239f, 0.f,
			.3827f, -.9239f, 0.f,
			.9239f, -.3827f, 0.f,
		};
		int Primitive::s_octagon_vertices_count = 27;
		GLuint Primitive::s_octagon_indices[17]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 1 };
		int Primitive::s_octagon_indices_count = 17;
		GLuint Primitive::s_octagon_line_indices[32]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 7, 0, 7, 7, 8, 0, 8, 8, 1 };
		int Primitive::s_octagon_line_indices_count = 32;
		GLfloat Primitive::s_octagon_texture_coordinates[18]{
			.5f, .5f,
			.96195f, .69135f,
			.69135f, .96195f,
			.30865f, .96195f,
			.03805f, .69135f,
			.03805f, .30865f,
			.30865f, .03805f,
			.69135f, .03805f,
			.96195f, .30865f,
		};
		int Primitive::s_octagon_texture_coordinates_count = 18;
		GLuint Primitive::s_octagon_VAO = 0;
		GLuint Primitive::s_octagon_vertices_VBO = 0;
		GLuint Primitive::s_octagon_tex_coords_VBO = 0;
		GLuint Primitive::s_octagon_indices_IBO = 0;
		GLuint Primitive::s_octagon_line_indices_IBO = 0;

		GLfloat Primitive::s_nonagon_vertices[30]{
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			-.6428f, .766f, 0.f,
			-.9848f, .1736f, 0.f,
			-.866f, -.5f, 0.f,
			-.342f, -.9397f, 0.f,
			.342f, -.9397f, 0.f,
			.866f, -.5f, 0.f,
			.9848f, .1736f, 0.f,
			.6428f, .766f, 0.f,
		};
		int Primitive::s_nonagon_vertices_count = 30;
		GLuint Primitive::s_nonagon_indices[19]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 1 };
		int Primitive::s_nonagon_indices_count = 19;
		GLuint Primitive::s_nonagon_line_indices[36]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 7, 0, 7, 7, 8, 0, 8, 8, 9, 0, 9, 9, 1 };
		int Primitive::s_nonagon_line_indices_count = 36;
		GLfloat Primitive::s_nonagon_texture_coordinates[20]{
			.5f, .5f,
			.5f, 1.f,
			.1786f, .883f,
			.00760001f, .5868f,
			.067f, .25f,
			.329f, .03015f,
			.671f, .03015f,
			.933f, .25f,
			.9924f, .5868f,
			.8214f, .883f
		};
		int Primitive::s_nonagon_texture_coordinates_count = 20;
		GLuint Primitive::s_nonagon_VAO = 0;
		GLuint Primitive::s_nonagon_vertices_VBO = 0;
		GLuint Primitive::s_nonagon_tex_coords_VBO = 0;
		GLuint Primitive::s_nonagon_indices_IBO = 0;
		GLuint Primitive::s_nonagon_line_indices_IBO = 0;

		GLfloat Primitive::s_decagon_vertices[33]{
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			.809f, .5878f, 0.f,
			.309f, .9511f, 0.f,
			-.309f, .9511f, 0.f,
			-.809f, .5878f, 0.f,
			-1.f, 0.f, 0.f,
			-.809f, -.5878f, 0.f,
			-.309f, -.9511f, 0.f,
			.309f, -.9511f, 0.f,
			.809f, -.5878f, 0.f,
		};
		int Primitive::s_decagon_vertices_count = 33;
		GLuint Primitive::s_decagon_indices[21]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 1 };
		int Primitive::s_decagon_indices_count = 21;
		GLuint Primitive::s_decagon_line_indices[40]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 7, 0, 7, 7, 8, 0, 8, 8, 9, 0, 9, 9, 10, 0, 10, 10, 1 };
		int Primitive::s_decagon_line_indices_count = 40;
		GLfloat Primitive::s_decagon_texture_coordinates[22]{
			.5f, .5f,
			1.f, .5f,
			.9045f, .7939f,
			.6545f, .97555f,
			.3455f, .97555f,
			.0955f, .7939f,
			0.f, .5f,
			.0955f, .2061f,
			.3455f, .02445f,
			.6545f, .02445f,
			.9045f, .2061f,
		};
		int Primitive::s_decagon_texture_coordinates_count = 22;
		GLuint Primitive::s_decagon_VAO = 0;
		GLuint Primitive::s_decagon_vertices_VBO = 0;
		GLuint Primitive::s_decagon_tex_coords_VBO = 0;
		GLuint Primitive::s_decagon_indices_IBO = 0;
		GLuint Primitive::s_decagon_line_indices_IBO = 0;

		GLfloat Primitive::s_circle_vertices[303]{
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			.998f, .0627f, 0.f,
			.9921f, .1253f, 0.f,
			.9822f, .1873f, 0.f,
			.9685f, .2486f, 0.f,
			.951f, .309f, 0.f,
			.9297f, .3681f, 0.f,
			.9048f, .4257f, 0.f,
			.8763f, .4817f, 0.f,
			.8443f, .5358f, 0.f,
			.809f, .5877f, 0.f,
			.7705f, .6374f, 0.f,
			.7289f, .6845f, 0.f,
			.6845f, .7289f, 0.f,
			.6374f, .7705f, 0.f,
			.5877f, .809f, 0.f,
			.5358f, .8443f, 0.f,
			.4817f, .8763f, 0.f,
			.4257f, .9048f, 0.f,
			.3681f, .9297f, 0.f,
			.309f, .951f, 0.f,
			.2486f, .9685f, 0.f,
			.1873f, .9822f, 0.f,
			.1253f, .9921f, 0.f,
			.0627f, .998f, 0.f,
			0.f, 1.f, 0.f,
			-.0628f, .998f, 0.f,
			-.1254f, .9921f, 0.f,
			-.1874f, .9822f, 0.f,
			-.2487f, .9685f, 0.f,
			-.3091f, .951f, 0.f,
			-.3682f, .9297f, 0.f,
			-.4258f, .9048f, 0.f,
			-.4818f, .8763f, 0.f,
			-.5359f, .8443f, 0.f,
			-.5878f, .809f, 0.f,
			-.6375f, .7705f, 0.f,
			-.6846f, .7289f, 0.f,
			-.729f, .6845f, 0.f,
			-.7706f, .6374f, 0.f,
			-.8091f, .5877f, 0.f,
			-.8444f, .5358f, 0.f,
			-.8764f, .4817f, 0.f,
			-.9049f, .4257f, 0.f,
			-.9298f, .3681f, 0.f,
			-.9511f, .309f, 0.f,
			-.9686f, .2486f, 0.f,
			-.9823f, .1873f, 0.f,
			-.9922f, .1253f, 0.f,
			-.9981f, .0627f, 0.f,
			-1.f, 0.f, 0.f,
			-.9981f, -.0628f, 0.f,
			-.9922f, -.1254f, 0.f,
			-.9823f, -.1874f, 0.f,
			-.9686f, -.2487f, 0.f,
			-.9511f, -.3091f, 0.f,
			-.9298f, -.3682f, 0.f,
			-.9049f, -.4258f, 0.f,
			-.8764f, -.4818f, 0.f,
			-.8444f, -.5359f, 0.f,
			-.8091f, -.5878f, 0.f,
			-.7706f, -.6375f, 0.f,
			-.729f, -.6846f, 0.f,
			-.6846f, -.729f, 0.f,
			-.6375f, -.7706f, 0.f,
			-.5878f, -.8091f, 0.f,
			-.5359f, -.8444f, 0.f,
			-.4818f, -.8764f, 0.f,
			-.4258f, -.9049f, 0.f,
			-.3682f, -.9298f, 0.f,
			-.3091f, -.9511f, 0.f,
			-.2487f, -.9686f, 0.f,
			-.1874f, -.9823f, 0.f,
			-.1254f, -.9922f, 0.f,
			-.0628f, -.9981f, 0.f,
			0.f, -1, 0.f,
			.0627f, -.9981f, 0.f,
			.1253f, -.9922f, 0.f,
			.1873f, -.9823f, 0.f,
			.2486f, -.9686f, 0.f,
			.309f, -.9511f, 0.f,
			.3681f, -.9298f, 0.f,
			.4257f, -.9049f, 0.f,
			.4817f, -.8764f, 0.f,
			.5358f, -.8444f, 0.f,
			.5877f, -.8091f, 0.f,
			.6374f, -.7706f, 0.f,
			.6845f, -.729f, 0.f,
			.7289f, -.6846f, 0.f,
			.7705f, -.6375f, 0.f,
			.809f, -.5878f, 0.f,
			.8443f, -.5359f, 0.f,
			.8763f, -.4818f, 0.f,
			.9048f, -.4258f, 0.f,
			.9297f, -.3682f, 0.f,
			.951f, -.3091f, 0.f,
			.9685f, -.2487f, 0.f,
			.9822f, -.1874f, 0.f,
			.9921f, -.1254f, 0.f,
			.998f, -.0628f, 0.f
		};
		int Primitive::s_circle_vertices_count = 303;
		GLuint Primitive::s_circle_indices[201]{ 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 100, 100, 1 };
		int Primitive::s_circle_indices_count = 201;
		GLuint Primitive::s_circle_line_indices[400]{ 0, 1, 1, 2, 0, 2, 2, 3, 0, 3, 3, 4, 0, 4, 4, 5, 0, 5, 5, 6, 0, 6, 6, 7, 0, 7, 7, 8, 0, 8, 8, 9, 0, 9, 9, 10, 0, 10, 10, 11, 0, 11, 11, 12, 0, 12, 12, 13, 0, 13, 13, 14, 0, 14, 14, 15, 0, 15, 15, 16, 0, 16, 16, 17, 0, 17, 17, 18, 0, 18, 18, 19, 0, 19, 19, 20, 0, 20, 20, 21, 0, 21, 21, 22, 0, 22, 22, 23, 0, 23, 23, 24, 0, 24, 24, 25, 0, 25, 25, 26, 0, 26, 26, 27, 0, 27, 27, 28, 0, 28, 28, 29, 0, 29, 29, 30, 0, 30, 30, 31, 0, 31, 31, 32, 0, 32, 32, 33, 0, 33, 33, 34, 0, 34, 34, 35, 0, 35, 35, 36, 0, 36, 36, 37, 0, 37, 37, 38, 0, 38, 38, 39, 0, 39, 39, 40, 0, 40, 40, 41, 0, 41, 41, 42, 0, 42, 42, 43, 0, 43, 43, 44, 0, 44, 44, 45, 0, 45, 45, 46, 0, 46, 46, 47, 0, 47, 47, 48, 0, 48, 48, 49, 0, 49, 49, 50, 0, 50, 50, 51, 0, 51, 51, 52, 0, 52, 52, 53, 0, 53, 53, 54, 0, 54, 54, 55, 0, 55, 55, 56, 0, 56, 56, 57, 0, 57, 57, 58, 0, 58, 58, 59, 0, 59, 59, 60, 0, 60, 60, 61, 0, 61, 61, 62, 0, 62, 62, 63, 0, 63, 63, 64, 0, 64, 64, 65, 0, 65, 65, 66, 0, 66, 66, 67, 0, 67, 67, 68, 0, 68, 68, 69, 0, 69, 69, 70, 0, 70, 70, 71, 0, 71, 71, 72, 0, 72, 72, 73, 0, 73, 73, 74, 0, 74, 74, 75, 0, 75, 75, 76, 0, 76, 76, 77, 0, 77, 77, 78, 0, 78, 78, 79, 0, 79, 79, 80, 0, 80, 80, 81, 0, 81, 81, 82, 0, 82, 82, 83, 0, 83, 83, 84, 0, 84, 84, 85, 0, 85, 85, 86, 0, 86, 86, 87, 0, 87, 87, 88, 0, 88, 88, 89, 0, 89, 89, 90, 0, 90, 90, 91, 0, 91, 91, 92, 0, 92, 92, 93, 0, 93, 93, 94, 0, 94, 94, 95, 0, 95, 95, 96, 0, 96, 96, 97, 0, 97, 97, 98, 0, 98, 98, 99, 0, 99, 99, 100, 0, 100, 100, 1 };
		int Primitive::s_circle_line_indices_count = 400;
		GLfloat Primitive::s_circle_texture_coordinates[202]{
			.5f, .5f,
			1.f, .5f,
			.999f, .53135f,
			.99605f, .56265f,
			.9911f, .59365f,
			.98425f, .6243f,
			.9755f, .6545f,
			.96485f, .68405f,
			.9524f, .71285f,
			.93815f, .74085f,
			.92215f, .7679f,
			.9045f, .79385f,
			.88525f, .8187f,
			.86445f, .84225f,
			.84225f, .86445f,
			.8187f, .88525f,
			.79385f, .9045f,
			.7679f, .92215f,
			.74085f, .93815f,
			.71285f, .9524f,
			.68405f, .96485f,
			.6545f, .9755f,
			.6243f, .98425f,
			.59365f, .9911f,
			.56265f, .99605f,
			.53135f, .999f,
			.5f, 1.f,
			.4686f, .999f,
			.4373f, .99605f,
			.4063f, .9911f,
			.37565f, .98425f,
			.34545f, .9755f,
			.3159f, .96485f,
			.2871f, .9524f,
			.2591f, .93815f,
			.23205f, .92215f,
			.2061f, .9045f,
			.18125f, .88525f,
			.1577f, .86445f,
			.1355f, .84225f,
			.1147f, .8187f,
			.09545f, .79385f,
			.0778f, .7679f,
			.0618f, .74085f,
			.04755f, .71285f,
			.0351f, .68405f,
			.02445f, .6545f,
			.0157f, .6243f,
			.00885001f, .59365f,
			.00389999f, .56265f,
			.000950009f, .53135f,
			0.f, .5f,
			.000950009f, .4686f,
			.00389999f, .4373f,
			.00885001f, .4063f,
			.0157f, .37565f,
			.02445f, .34545f,
			.0351f, .3159f,
			.04755f, .2871f,
			.0618f, .2591f,
			.0778f, .23205f,
			.09545f, .2061f,
			.1147f, .18125f,
			.1355f, .1577f,
			.1577f, .1355f,
			.18125f, .1147f,
			.2061f, .09545f,
			.23205f, .0778f,
			.2591f, .0618f,
			.2871f, .04755f,
			.3159f, .0351f,
			.34545f, .02445f,
			.37565f, .0157f,
			.4063f, .00885001f,
			.4373f, .00389999f,
			.4686f, .000950009f,
			.5f, 0.f,
			.53135f, .000950009f,
			.56265f, .00389999f,
			.59365f, .00885001f,
			.6243f, .0157f,
			.6545f, .02445f,
			.68405f, .0351f,
			.71285f, .04755f,
			.74085f, .0618f,
			.7679f, .0778f,
			.79385f, .09545f,
			.8187f, .1147f,
			.84225f, .1355f,
			.86445f, .1577f,
			.88525f, .18125f,
			.9045f, .2061f,
			.92215f, .23205f,
			.93815f, .2591f,
			.9524f, .2871f,
			.96485f, .3159f,
			.9755f, .34545f,
			.98425f, .37565f,
			.9911f, .4063f,
			.99605f, .4373f,
			.999f, .4686f,
		};
		int Primitive::s_circle_texture_coordinates_count = 202;
		GLuint Primitive::s_circle_VAO = 0;
		GLuint Primitive::s_circle_vertices_VBO = 0;
		GLuint Primitive::s_circle_tex_coords_VBO = 0;
		GLuint Primitive::s_circle_indices_IBO = 0;
		GLuint Primitive::s_circle_line_indices_IBO = 0;

		void Primitive::get_buffers(Primitive* p) {
			switch (p->m_type) {
			case TRIANGLE:
				p->m_VAO = s_triangle_VAO;
				p->m_vertices_VBO = s_triangle_vertices_VBO;
				p->m_tex_coords_VBO = s_triangle_tex_coords_VBO;
				p->m_indices_IBO = s_triangle_indices_IBO;
				p->m_line_indices_IBO = s_triangle_line_indices_IBO;
				p->m_indices_count = s_triangle_indices_count;
				p->m_line_indices_count = s_triangle_line_indices_count;
				break;
			case SQUARE:
				p->m_VAO = s_square_VAO;
				p->m_vertices_VBO = s_square_vertices_VBO;
				p->m_tex_coords_VBO = s_square_tex_coords_VBO;
				p->m_indices_IBO = s_square_indices_IBO;
				p->m_line_indices_IBO = s_square_line_indices_IBO;
				p->m_indices_count = s_square_indices_count;
				p->m_line_indices_count = s_square_line_indices_count;
				break;
			case PENTAGON:
				p->m_VAO = s_pentagon_VAO;
				p->m_vertices_VBO = s_pentagon_vertices_VBO;
				p->m_tex_coords_VBO = s_pentagon_tex_coords_VBO;
				p->m_indices_IBO = s_pentagon_indices_IBO;
				p->m_line_indices_IBO = s_pentagon_line_indices_IBO;
				p->m_indices_count = s_pentagon_indices_count;
				p->m_line_indices_count = s_pentagon_line_indices_count;
				break;
			case HEXAGON:
				p->m_VAO = s_hexagon_VAO;
				p->m_vertices_VBO = s_hexagon_vertices_VBO;
				p->m_tex_coords_VBO = s_hexagon_tex_coords_VBO;
				p->m_indices_IBO = s_hexagon_indices_IBO;
				p->m_line_indices_IBO = s_hexagon_line_indices_IBO;
				p->m_indices_count = s_hexagon_indices_count;
				p->m_line_indices_count = s_hexagon_line_indices_count;
				break;
			case HEPTAGON:
				p->m_VAO = s_heptagon_VAO;
				p->m_vertices_VBO = s_heptagon_vertices_VBO;
				p->m_tex_coords_VBO = s_heptagon_tex_coords_VBO;
				p->m_indices_IBO = s_heptagon_indices_IBO;
				p->m_line_indices_IBO = s_heptagon_line_indices_IBO;
				p->m_indices_count = s_heptagon_indices_count;
				p->m_line_indices_count = s_heptagon_line_indices_count;
				break;
			case OCTAGON:
				p->m_VAO = s_octagon_VAO;
				p->m_vertices_VBO = s_octagon_vertices_VBO;
				p->m_tex_coords_VBO = s_octagon_tex_coords_VBO;
				p->m_indices_IBO = s_octagon_indices_IBO;
				p->m_line_indices_IBO = s_octagon_line_indices_IBO;
				p->m_indices_count = s_octagon_indices_count;
				p->m_line_indices_count = s_octagon_line_indices_count;
				break;
			case NONAGON:
				p->m_VAO = s_nonagon_VAO;
				p->m_vertices_VBO = s_nonagon_vertices_VBO;
				p->m_tex_coords_VBO = s_nonagon_tex_coords_VBO;
				p->m_indices_IBO = s_nonagon_indices_IBO;
				p->m_line_indices_IBO = s_nonagon_line_indices_IBO;
				p->m_indices_count = s_nonagon_indices_count;
				p->m_line_indices_count = s_nonagon_line_indices_count;
				break;
			case DECAGON:
				p->m_VAO = s_decagon_VAO;
				p->m_vertices_VBO = s_decagon_vertices_VBO;
				p->m_tex_coords_VBO = s_decagon_tex_coords_VBO;
				p->m_indices_IBO = s_decagon_indices_IBO;
				p->m_line_indices_IBO = s_decagon_line_indices_IBO;
				p->m_indices_count = s_decagon_indices_count;
				p->m_line_indices_count = s_decagon_line_indices_count;
				break;
			case CIRCLE:
				p->m_VAO = s_circle_VAO;
				p->m_vertices_VBO = s_circle_vertices_VBO;
				p->m_tex_coords_VBO = s_circle_tex_coords_VBO;
				p->m_indices_IBO = s_circle_indices_IBO;
				p->m_line_indices_IBO = s_circle_line_indices_IBO;
				p->m_indices_count = s_circle_indices_count;
				p->m_line_indices_count = s_circle_line_indices_count;
				break;
			}
		}

		void Primitive::generate_buffers() {
			{ //TRIANGLE
				glGenVertexArrays(1, &s_triangle_VAO);
				glBindVertexArray(s_triangle_VAO);

				glGenBuffers(1, &s_triangle_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_triangle_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_triangle_vertices), s_triangle_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_triangle_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_triangle_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_triangle_texture_coordinates), s_triangle_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_triangle_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_triangle_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_triangle_indices), s_triangle_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_triangle_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_triangle_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_triangle_line_indices), s_triangle_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //SQUARE
				glGenVertexArrays(1, &s_square_VAO);
				glBindVertexArray(s_square_VAO);

				glGenBuffers(1, &s_square_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_square_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_square_vertices), s_square_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_square_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_square_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_square_texture_coordinates), s_square_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_square_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_square_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_square_indices), s_square_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_square_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_square_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_square_line_indices), s_square_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //PENTAGON
				glGenVertexArrays(1, &s_pentagon_VAO);
				glBindVertexArray(s_pentagon_VAO);

				glGenBuffers(1, &s_pentagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_pentagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_pentagon_vertices), s_pentagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_pentagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_pentagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_pentagon_texture_coordinates), s_pentagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_pentagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_pentagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_pentagon_indices), s_pentagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_pentagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_pentagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_pentagon_line_indices), s_pentagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //HEXAGON
				glGenVertexArrays(1, &s_hexagon_VAO);
				glBindVertexArray(s_hexagon_VAO);

				glGenBuffers(1, &s_hexagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_hexagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_hexagon_vertices), s_hexagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_hexagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_hexagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_hexagon_texture_coordinates), s_hexagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_hexagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_hexagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_hexagon_indices), s_hexagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_hexagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_hexagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_hexagon_line_indices), s_hexagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //HEPTAGON
				glGenVertexArrays(1, &s_heptagon_VAO);
				glBindVertexArray(s_heptagon_VAO);

				glGenBuffers(1, &s_heptagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_heptagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_heptagon_vertices), s_heptagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_heptagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_heptagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_heptagon_texture_coordinates), s_heptagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_heptagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_heptagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_heptagon_indices), s_heptagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_heptagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_heptagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_heptagon_line_indices), s_heptagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //OCTAGON
				glGenVertexArrays(1, &s_octagon_VAO);
				glBindVertexArray(s_octagon_VAO);

				glGenBuffers(1, &s_octagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_octagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_octagon_vertices), s_octagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_octagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_octagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_octagon_texture_coordinates), s_octagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_octagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_octagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_octagon_indices), s_octagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_octagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_octagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_octagon_line_indices), s_octagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //NONAGON
				glGenVertexArrays(1, &s_nonagon_VAO);
				glBindVertexArray(s_nonagon_VAO);

				glGenBuffers(1, &s_nonagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_nonagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_nonagon_vertices), s_nonagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_nonagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_nonagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_nonagon_texture_coordinates), s_nonagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_nonagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_nonagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_nonagon_indices), s_nonagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_nonagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_nonagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_nonagon_line_indices), s_nonagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //DECAGON
				glGenVertexArrays(1, &s_decagon_VAO);
				glBindVertexArray(s_decagon_VAO);

				glGenBuffers(1, &s_decagon_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_decagon_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_decagon_vertices), s_decagon_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_decagon_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_decagon_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_decagon_texture_coordinates), s_decagon_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_decagon_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_decagon_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_decagon_indices), s_decagon_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_decagon_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_decagon_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_decagon_line_indices), s_decagon_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			{ //CIRCLE
				glGenVertexArrays(1, &s_circle_VAO);
				glBindVertexArray(s_circle_VAO);

				glGenBuffers(1, &s_circle_vertices_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_circle_vertices_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_circle_vertices), s_circle_vertices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_circle_tex_coords_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, s_circle_tex_coords_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(s_circle_texture_coordinates), s_circle_texture_coordinates, GL_STATIC_DRAW);

				glGenBuffers(1, &s_circle_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_circle_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_circle_indices), s_circle_indices, GL_STATIC_DRAW);

				glGenBuffers(1, &s_circle_line_indices_IBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_circle_line_indices_IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_circle_line_indices), s_circle_line_indices, GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		void Primitive::purge() {
			{ //TRIANGLE
				glDeleteVertexArrays(1, &s_triangle_VAO);
				glDeleteBuffers(1, &s_triangle_vertices_VBO);
				glDeleteBuffers(1, &s_triangle_tex_coords_VBO);
				glDeleteBuffers(1, &s_triangle_indices_IBO);
				glDeleteBuffers(1, &s_triangle_line_indices_IBO);
			}

			{ //SQUARE
				glDeleteVertexArrays(1, &s_square_VAO);
				glDeleteBuffers(1, &s_square_vertices_VBO);
				glDeleteBuffers(1, &s_square_tex_coords_VBO);
				glDeleteBuffers(1, &s_square_indices_IBO);
				glDeleteBuffers(1, &s_square_line_indices_IBO);
			}

			{ //PENTAGON
				glDeleteVertexArrays(1, &s_pentagon_VAO);
				glDeleteBuffers(1, &s_pentagon_vertices_VBO);
				glDeleteBuffers(1, &s_pentagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_pentagon_indices_IBO);
				glDeleteBuffers(1, &s_pentagon_line_indices_IBO);
			}

			{ //HEXAGON
				glDeleteVertexArrays(1, &s_hexagon_VAO);
				glDeleteBuffers(1, &s_hexagon_vertices_VBO);
				glDeleteBuffers(1, &s_hexagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_hexagon_indices_IBO);
				glDeleteBuffers(1, &s_hexagon_line_indices_IBO);
			}

			{ //HEPTAGON
				glDeleteVertexArrays(1, &s_heptagon_VAO);
				glDeleteBuffers(1, &s_heptagon_vertices_VBO);
				glDeleteBuffers(1, &s_heptagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_heptagon_indices_IBO);
				glDeleteBuffers(1, &s_heptagon_line_indices_IBO);
			}

			{ //OCTAGON
				glDeleteVertexArrays(1, &s_octagon_VAO);
				glDeleteBuffers(1, &s_octagon_vertices_VBO);
				glDeleteBuffers(1, &s_octagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_octagon_indices_IBO);
				glDeleteBuffers(1, &s_octagon_line_indices_IBO);
			}

			{ //NONAGON
				glDeleteVertexArrays(1, &s_nonagon_VAO);
				glDeleteBuffers(1, &s_nonagon_vertices_VBO);
				glDeleteBuffers(1, &s_nonagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_nonagon_indices_IBO);
				glDeleteBuffers(1, &s_nonagon_line_indices_IBO);
			}

			{ //DECAGON
				glDeleteVertexArrays(1, &s_decagon_VAO);
				glDeleteBuffers(1, &s_decagon_vertices_VBO);
				glDeleteBuffers(1, &s_decagon_tex_coords_VBO);
				glDeleteBuffers(1, &s_decagon_indices_IBO);
				glDeleteBuffers(1, &s_decagon_line_indices_IBO);
			}

			{ //CIRCLE
				glDeleteVertexArrays(1, &s_circle_VAO);
				glDeleteBuffers(1, &s_circle_vertices_VBO);
				glDeleteBuffers(1, &s_circle_tex_coords_VBO);
				glDeleteBuffers(1, &s_circle_indices_IBO);
				glDeleteBuffers(1, &s_circle_line_indices_IBO);
			}
		}
	}
}