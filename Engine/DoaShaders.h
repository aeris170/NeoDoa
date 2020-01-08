// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::shader {

	typedef GLuint ShaderProgram;

	extern std::map<std::string, ShaderProgram*> SHADERS;

	DOA_API ShaderProgram* const CreateShaderProgram(const std::string& name, const std::string& pathToVertexShader, const std::string& pathToFragmentShader);

	DOA_API ShaderProgram* const Get(const std::string& name);
}

namespace internal::shader {

	typedef GLuint ShaderProgram;

	GLuint add_shader_to(ShaderProgram shaderProgram, const std::string& name, const char *shaderCode, GLenum shaderType);
	void link_and_validate_shader_program(ShaderProgram shader, const std::string& name);

	void purge();
}