// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::texture {

	typedef GLuint Texture;

	extern std::map<std::string, Texture*> TEXTURES;

	DOA_API Texture* const CreateTexture(const std::string& name, const std::string& pathToTextureImage, const int minFilter = GL_NEAREST_MIPMAP_NEAREST, const int magFilter = GL_NEAREST);

	DOA_API Texture* const Get(const std::string& name);
}

namespace internal::texture {

	typedef GLuint Texture;

	Texture* const create_texture(const std::string& pathToTextureImage, const int minFilter, const int magFilter);

	void purge();
}