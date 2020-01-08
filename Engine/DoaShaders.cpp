// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::shader {
	using namespace internal::shader;

	std::map<std::string, ShaderProgram*> SHADERS;

	ShaderProgram* const CreateShaderProgram(const std::string& name, const std::string& pathToVertexShader, const std::string& pathToFragmentShader) {
		if (name.length() == 0) {
			std::cout << "doa::shader::CreateShader cannot accept empty string as name!\n";
			throw - 1;
		}
		if (pathToVertexShader.length() == 0) {
			std::cout << "doa::shader::CreateShader cannot accept empty string as pathToVertexShader!\n";
			throw - 1;
		}

		ShaderProgram* shaderProgram{ new ShaderProgram };
		*shaderProgram = glCreateProgram();

		if (!shaderProgram) {
			std::cout << "Doa fucked up while creating shader!\n";
			return NULL;
		}

		std::ifstream vShaderFile{ pathToVertexShader };
		std::string vShaderCode;
		vShaderFile.seekg(0, std::ios::end);
		vShaderCode.reserve(vShaderFile.tellg());
		vShaderFile.seekg(0, std::ios::beg);
		vShaderCode.assign((std::istreambuf_iterator<char>(vShaderFile)), std::istreambuf_iterator<char>());
		GLuint vs = add_shader_to(*shaderProgram, name, vShaderCode.c_str(), GL_VERTEX_SHADER);

		GLuint fs = -1;
		if (pathToFragmentShader.length() > 0) {
			std::ifstream fShaderFile{ pathToFragmentShader };
			std::string fShaderCode;
			fShaderFile.seekg(0, std::ios::end);
			fShaderCode.reserve(fShaderFile.tellg());
			fShaderFile.seekg(0, std::ios::beg);
			fShaderCode.assign((std::istreambuf_iterator<char>(fShaderFile)), std::istreambuf_iterator<char>());
			fs = add_shader_to(*shaderProgram, name, fShaderCode.c_str(), GL_FRAGMENT_SHADER);
		}
		link_and_validate_shader_program(*shaderProgram, name);

		glDetachShader(*shaderProgram, vs);
		if (fs != -1) {
			glDetachShader(*shaderProgram, fs);
		}

		if (SHADERS[name]) {
			glDeleteProgram(*SHADERS[name]);
			delete SHADERS[name];
		}
		SHADERS[name] = shaderProgram;
		return shaderProgram;
	}

	ShaderProgram* const Get(const std::string& name) {
		return SHADERS[name];
	}
}

namespace internal::shader {

	GLuint add_shader_to(ShaderProgram shaderProgram, const std::string& name, const char *shaderCode, GLenum shaderType) {
		GLuint shader{ glCreateShader(shaderType) };

		const GLchar *code[1];
		code[0] = shaderCode;

		GLint length[1];
		length[0] = static_cast<GLint>(strlen(shaderCode));

		glShaderSource(shader, 1, code, length);
		glCompileShader(shader);

		GLint result{ 0 };
		GLchar errorLog[1024] = { 0 };

		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Doa fucked up while compiling shader " << name << "!\n" << errorLog << "\n";
			return -1;
		}

		glAttachShader(shaderProgram, shader);

		glDeleteShader(shader);

		return shader;
	}

	void link_and_validate_shader_program(ShaderProgram shader, const std::string& name) {
		GLint result{ 0 };
		GLchar errorLog[1024] = { 0 };
		glLinkProgram(shader);
		glGetProgramiv(shader, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Doa fucked up while linking shader " << name << "!\n" << errorLog << "\n";
			return;
		}
		glValidateProgram(shader);
		glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
			std::cout << "Doa fucked up while validating shader " << name << "!\n" << errorLog << "\n";
			return;
		}
	}

	void purge() {
		for (auto itr = doa::shader::SHADERS.begin(); itr != doa::shader::SHADERS.end(); ++itr)
		{
			ShaderProgram *p = itr->second;
			glDeleteProgram(*p);
			delete p;
		}
		doa::shader::SHADERS.clear();
	}
}