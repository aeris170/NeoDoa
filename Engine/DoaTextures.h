// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::texture {

	typedef GLuint Texture;

	extern std::map<std::string, Texture*> TEXTURES;

	DOA_API Texture* const CreateTexture(const std::string& name, const std::string& pathToTextureImage);

	DOA_API Texture* const Get(const std::string& name);
}

namespace internal::texture {

	typedef GLuint Texture;

	Texture* const create_texture(const std::string& pathToTextureImage);

	void purge();
}