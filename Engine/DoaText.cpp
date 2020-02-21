// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::text {
	using namespace internal::text;

	std::map<const char*, Font*, internal::char_cmp> FONTS;

	Font* CreateANSIFont(const char* name, const char* pathToFont, int atlasSize, int charCount) {
		if (!name) {
			std::cout << "doa::text::CreateANSIFont cannot accept NULL as name!\n";
			throw - 1;
		}
		if (!pathToFont) {
			std::cout << "doa::text::CreateANSIFont cannot accept NULL as pathToFont!\n";
			throw - 1;
		}
		if (name[0] == 0) {
			std::cout << "doa::text::CreateANSIFont cannot accept 0-length char* as name!\n";
			throw - 1;
		}
		if (pathToFont[0] == 0) {
			std::cout << "doa::text::CreateANSIFont cannot accept 0-length char* as pathToFont!\n";
			throw - 1;
		}
		Font* f{ new Font() };
		if (SDF::render_signed_distance_font(ft, pathToFont, atlasSize, charCount)) {
			//SDF TEX GEN START
			texture::Texture* texture{ new texture::Texture };

			glGenTextures(1, texture);
			glBindTexture(GL_TEXTURE_2D, *texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.f);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasSize, atlasSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, SDF::pdata.data());
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			f->m_sdf_size = atlasSize;
			f->m_sdf_tex = texture;
			//SDF TEX GEN END
			//SDF CHAR GEN START
			for (auto itr{ SDF::all_glyphs.begin() }; itr != SDF::all_glyphs.end(); ++itr) {
				(*f->m_chars)[(*itr)->ID] = *itr;
			}
			//SDF CHAR GEN END
			if (FONTS[name]) {
				delete FONTS[name];
			}
			FONTS[name] = f;
		} else{
			std::cout << "Fucked up creating font " << name << "!\n";
		}
		return f;
	}

	Font* CreateUnicodeFont(const char* name, const char* pathToFont, int atlasSize, int charCount) {
		if (!name) {
			std::cout << "doa::text::CreateUnicodeFont cannot accept NULL as name!\n";
			throw - 1;
		}
		if (!pathToFont) {
			std::cout << "doa::text::CreateUnicodeFont cannot accept NULL as pathToFont!\n";
			throw - 1;
		}
		if (name[0] == 0) {
			std::cout << "doa::text::CreateUnicodeFont cannot accept 0-length char* as name!\n";
			throw - 1;
		}
		if (pathToFont[0] == 0) {
			std::cout << "doa::text::CreateUnicodeFont cannot accept 0-length char* as pathToFont!\n";
			throw - 1;
		}
		Font* f{ new Font() };
		if (SDF::render_signed_distance_font(ft, pathToFont, atlasSize, charCount)) {
			//SDF TEX GEN START
			texture::Texture* texture{ new texture::Texture };

			glGenTextures(1, texture);
			glBindTexture(GL_TEXTURE_2D, *texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.f);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasSize, atlasSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, SDF::pdata.data());
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			f->m_sdf_size = atlasSize;
			f->m_sdf_tex = texture;
			//SDF TEX GEN END
			//SDF CHAR GEN START
			for (auto itr{ SDF::all_glyphs.begin() }; itr != SDF::all_glyphs.end(); ++itr) {
				(*f->m_chars)[(*itr)->ID] = *itr;
			}
			//SDF CHAR GEN END
			if (FONTS[name]) {
				delete FONTS[name];
			}
			FONTS[name] = f;
		}
		else {
			std::cout << "Fucked up creating font " << name << "!\n";
		}
		return f;
	}

	Font* Get(const char* name) {
		Font* f = FONTS[name];
		if (!f) {
			std::cout << "doa::text::FONTS[" + std::string(name) + "] returned NULL!";
		}
		return f;
	}

	Font::Font() {}

	Font::~Font() {
		glDeleteTextures(1, m_sdf_tex);
		delete m_sdf_tex;
		for (auto itr{ m_chars->begin() }; itr != m_chars->end(); ++itr) {
			delete itr->second;
		}
		m_chars->clear();
		delete m_chars;
	}

	Text::Text(Font* font, const char* data) : GameObject(), m_font{ font }, m_data{ data } {
		m_shader = doa::shader::SHADERS["text-shader"];
		m_is_fixed = true;

		while (m_data[m_data_length] != 0) {
			++m_data_length;
		}

		int vertices_count = m_data_length * 18;
		int tex_coords_count = m_data_length * 12;
		GLfloat* vertices = new GLfloat[vertices_count];
		GLfloat* tex_coords = new GLfloat[tex_coords_count];

		float cur_pos{ 0 };
		int vi{ 0 }, ti{ 0 };
		for (int i{ 0 }; i < m_data_length; ++i) {
			SDF::sdf_glyph& character{ *m_font->m_chars[0][m_data[i]] };
			float x{ cur_pos + character.xoff };
			float y{ character.yoff };

			float xPlusCharWidth = x + character.width;
			float yMinusCharHeight = y - character.height;

			vertices[vi++] = x;
			vertices[vi++] = y;
			vertices[vi++] = 0;

			vertices[vi++] = xPlusCharWidth;
			vertices[vi++] = y;
			vertices[vi++] = 0;

			vertices[vi++] = xPlusCharWidth;
			vertices[vi++] = yMinusCharHeight;
			vertices[vi++] = 0;

			vertices[vi++] = xPlusCharWidth;
			vertices[vi++] = yMinusCharHeight;
			vertices[vi++] = 0;

			vertices[vi++] = x;
			vertices[vi++] = yMinusCharHeight;
			vertices[vi++] = 0;

			vertices[vi++] = x;
			vertices[vi++] = y;
			vertices[vi++] = 0;

			m_data_max_x = xPlusCharWidth;

			if (yMinusCharHeight < m_data_min_y) {
				m_data_min_y = yMinusCharHeight;
			}
			if (y > m_data_max_y) {
				m_data_max_y = y;
			}

			tex_coords[ti++] = character.x;
			tex_coords[ti++] = character.y;

			tex_coords[ti++] = character.x + character.width;
			tex_coords[ti++] = character.y;

			tex_coords[ti++] = character.x + character.width;
			tex_coords[ti++] = character.y + character.height;

			tex_coords[ti++] = character.x + character.width;
			tex_coords[ti++] = character.y + character.height;

			tex_coords[ti++] = character.x;
			tex_coords[ti++] = character.y + character.height;

			tex_coords[ti++] = character.x;
			tex_coords[ti++] = character.y;

			cur_pos += character.xadv;
		}

		for (int i{ 0 }; i < tex_coords_count; ++i) {
			tex_coords[i] /= m_font->m_sdf_size;
		}

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_vertices_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertices_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_count, vertices, GL_STATIC_DRAW);
		GLint pos = glGetAttribLocation(*m_shader, "vertex_coords");
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
		glEnableVertexAttribArray(pos);

		glGenBuffers(1, &m_tex_coords_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_tex_coords_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * tex_coords_count, tex_coords, GL_STATIC_DRAW);
		GLint tex_coords_position = glGetAttribLocation(*m_shader, "texture_coords");
		glVertexAttribPointer(tex_coords_position, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
		glEnableVertexAttribArray(tex_coords_position);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] vertices;
		delete[] tex_coords;
	}

	Text::~Text() {
		delete m_color;
		delete m_outline_color;
		delete m_outline_offset;
	}

	void Text::update(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) {}

	void Text::render(const scene::Scene& parent, const std::vector<scene::GameObject*>& objects, const std::vector<scene::Light*>& lights) const {
		glUseProgram(*m_shader);
		glBindVertexArray(m_VAO);

		glm::mat4 modelMatrix = glm::mat4(1);																					// A	Camera space transforms are complete.
		if (!m_is_fixed) {																										// |
			modelMatrix = glm::translate(modelMatrix, glm::vec3(camera::x, camera::y, 0));										// |	Third, translate the object back to it's position.
			modelMatrix = glm::scale(modelMatrix, glm::vec3(camera::z, camera::z, 1));											// |	Second, zoom into the camera origin.
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-camera::x, -camera::y, 0));									// |	First, translate the object to camera origin.
		}																														// |
																																// |	Object space transfroms are complete.
		modelMatrix = glm::translate(modelMatrix, *m_position);																	// |	Third, translate the object.
		modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->x / 180.f * M_PI), glm::vec3(1, 0, 0));						// |	and round X.
		modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->y / 180.f * M_PI), glm::vec3(0, 1, 0));						// |	round Y,
		modelMatrix = glm::rotate(modelMatrix, (float)(m_rotation->z / 180.f * M_PI), glm::vec3(0, 0, 1));						// |	Second, rotate the object round Z,
		modelMatrix = glm::scale(modelMatrix, glm::vec3(*m_scale, 1));															// |	First, scale the object.
																																// |	Reference point transforms are complete.
		if (m_ref_point == TOP_LEFT || m_ref_point == TOP_RIGHT) {																// |	If Text is referenced from top,
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -m_data_max_y, 0));											// |	Translate to top,
		} else {																												// |	Else, Text is referenced from bottom,
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -m_data_min_y, 0));											// |	Translate to bottom.
		}																														// |
		if (m_ref_point == BOTTOM_RIGHT || m_ref_point == TOP_RIGHT) {															// |	If Text is referenced from right,
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-m_data_max_x, 0, 0));											// |	Translate to right.
		}																														// |
		if (m_ref_point == CENTER) {																							// |	Text is referenced from center.
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-m_data_max_x / 2, -(m_data_min_y + m_data_max_y) / 2, 0));		// |	Translate to center.
		}																														// |

		GLint modelMatrixLocation = glGetUniformLocation(*m_shader, "modelMatrix");
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		GLint viewMatrixLocation = glGetUniformLocation(*m_shader, "viewMatrix");
		if (m_is_fixed) {
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_fixed_view_matrix));
		} else {
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_view_matrix));
		}

		GLint projectionMatrixLocation = glGetUniformLocation(*m_shader, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(internal::m_projection_matrix));

		GLint sampler2DLocation = glGetUniformLocation(*m_shader, "characterTextureAtlas");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *m_font->m_sdf_tex);
		glUniform1i(sampler2DLocation, 0);

		GLint colorLocation = glGetUniformLocation(*m_shader, "color");
		GLint outlineColorLocation = glGetUniformLocation(*m_shader, "outlineColor");
		GLint outlineOffsetLocation = glGetUniformLocation(*m_shader, "outlineOffset");
		glUniform3fv(colorLocation, 1, glm::value_ptr(*m_color));
		glUniform3fv(outlineColorLocation, 1, glm::value_ptr(*m_outline_color));
		glUniform2fv(outlineOffsetLocation, 1, glm::value_ptr(*m_outline_offset));

		GLint widthLocation = glGetUniformLocation(*m_shader, "width");
		GLint edgeLocation = glGetUniformLocation(*m_shader, "edge");
		glUniform1f(widthLocation, m_char_width);
		glUniform1f(edgeLocation, m_char_edge);

		GLint borderWidthLocation = glGetUniformLocation(*m_shader, "borderWidth");
		GLint borderEdgeLocation = glGetUniformLocation(*m_shader, "borderEdge");
		glUniform1f(borderWidthLocation, m_char_border_width);
		glUniform1f(borderEdgeLocation, m_char_border_edge);

		glDepthMask(false);
		glDrawArrays(GL_TRIANGLES, 0, 6 * m_data_length);
		glDepthMask(true);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
}

namespace internal::text {

	FontLoader ft;

	void init() {
		if (FT_Init_FreeType(&internal::text::ft)) {
			std::cout << "Fucked up while initializing FreeType!\n";
		}
	}

	void purge() {
		for (auto itr{ doa::text::FONTS.begin() }; itr != doa::text::FONTS.end(); ++itr) {
			delete itr->second;
		}
		doa::text::FONTS.clear();
		FT_Done_FreeType(ft);
	}
}