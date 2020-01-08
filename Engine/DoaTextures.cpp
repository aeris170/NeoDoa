// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::texture {
	using namespace internal::texture;

	std::map<std::string, Texture*> TEXTURES;

	Texture* const CreateTexture(const std::string& name, const std::string& pathToTextureImage) {
		//name hiding,     v v v     v v v   must explicitly state the namespace!
		Texture* texture{ internal::texture::CreateTexture(pathToTextureImage) };
		if (TEXTURES[name]) {
			glDeleteTextures(1, TEXTURES[name]);
			delete TEXTURES[name];
		}
		TEXTURES[name] = texture;
		return texture;
	}

	Texture* const Get(const std::string& name) {
		return TEXTURES[name];
	}
}

namespace internal::texture {

	Texture* const CreateTexture(const std::string& pathToTextureImage) {
		Texture* texture{ new Texture };

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data{ stbi_load(pathToTextureImage.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha) };
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Doa fucked up while loading texture " << pathToTextureImage << "\n";
			std::cout << stbi_failure_reason() << std::endl;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texture;
	}

	void purge() {
		for (auto itr = doa::texture::TEXTURES.begin(); itr != doa::texture::TEXTURES.end(); ++itr)
		{
			Texture *t = itr->second;
			glDeleteTextures(1, t);
			delete t;
		}
		doa::texture::TEXTURES.clear();
	}
}