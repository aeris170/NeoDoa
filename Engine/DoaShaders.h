// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::shader {

	typedef GLuint ShaderProgram;

	extern std::map<const char*, ShaderProgram*, internal::char_cmp> SHADERS;

	DOA_API ShaderProgram* const CreateShaderProgram(const char* name, const char* pathToVertexShader, const char* pathToFragmentShader = NULL);

	DOA_API ShaderProgram* const Get(const char* name);
}

namespace internal::shader {

	typedef GLuint ShaderProgram;

	GLuint add_shader_to(ShaderProgram shaderProgram, const char* name, const char *shaderCode, GLenum shaderType);
	void link_and_validate_shader_program(ShaderProgram shader, const char* name);

	void purge();
}