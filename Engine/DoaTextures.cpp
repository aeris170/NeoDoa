// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::texture {
	using namespace internal::texture;

	std::map<std::string, Texture*> TEXTURES;

	Texture* const CreateTexture(const char* name, const char* pathToTextureImage, const int minFilter, const int magFilter) {
		if (name[0] == 0) {
			std::cout << "doa::texture::CreateTexture cannot accept empty string as name!\n";
			throw - 1;
		}
		if (pathToTextureImage[0] == 0) {
			std::cout << "doa::texture::CreateTexture cannot accept empty string as pathToTextureImage!\n";
			throw - 1;
		}
		Texture* texture{ create_texture(pathToTextureImage, minFilter, magFilter) };
		if (TEXTURES[name]) {
			glDeleteTextures(1, TEXTURES[name]);
			delete TEXTURES[name];
		}
		TEXTURES[name] = texture;
		return texture;
	}

	Texture* const Get(const char* name) {
		return TEXTURES[name];
	}
}

namespace internal::texture {

	Texture* const create_texture(const char* pathToTextureImage, const int minFilter, const int magFilter) {
		Texture* texture{ new Texture };

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.f);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data{ stbi_load(pathToTextureImage, &width, &height, &nrChannels, STBI_rgb_alpha) };
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else {
			std::cout << "Fucked up while loading texture " << pathToTextureImage << "\n";
			std::cout << stbi_failure_reason() << "\n";
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texture;
	}

	void purge() {
		for (auto itr{ doa::texture::TEXTURES.begin() }; itr != doa::texture::TEXTURES.end(); ++itr) {
			Texture* t = itr->second;
			glDeleteTextures(1, t);
			delete t;
		}
		doa::texture::TEXTURES.clear();
	}
}