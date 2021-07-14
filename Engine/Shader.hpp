#pragma once

#include <optional>
#include <unordered_map>

#include "TypedefsAndConstants.hpp"

struct Shader : std::enable_shared_from_this<Shader> {
	std::string _name;
	ProgramID _glProgramID{ 0 };
	std::unordered_map<std::string, UniformLocation> _uniforms;

	void Use();

	bool Uniformi(std::string_view name, int value);
	bool Uniformiv(std::string_view name, int* values, int size);
	bool Uniform1f(std::string_view name, float* values, int count = 1);
	bool Uniform2f(std::string_view name, float* values, int count = 1);
	bool Uniform3f(std::string_view name, float* values, int count = 1);
	bool Uniform4f(std::string_view name, float* values, int count = 1);
	bool UniformMat3(std::string_view name, float* values, bool transpose = false);
	bool UniformMat4(std::string_view name, float* values, bool transpose = false);

	// don't call, use CreateShader
	Shader(std::string_view name, ProgramID glProgramID) noexcept;
	~Shader() noexcept;
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;

private:
	UniformLocation UniformLoc(std::string_view name);
};

std::weak_ptr<Shader> CreateShader(std::string_view name, const char* vShaderPath, const char* fShaderPath);
std::weak_ptr<Shader> FindShader(std::string_view name);

