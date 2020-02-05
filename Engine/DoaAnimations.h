// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::animation {

	struct DOA_API Animation;
	struct DOA_API AnimationFrame;

	extern std::map<const char*, Animation*, internal::char_cmp> ANIMATIONS;

	DOA_API Animation* const CreateAnimation(const char* name, const std::vector<doa::texture::Texture*>& textures, const std::chrono::milliseconds& durationOfAFrame);
	DOA_API Animation* const CreateAnimation(const char* name, const std::vector<doa::texture::Texture*>& textures, const std::vector<std::chrono::milliseconds>& durationsOfFrames);

	DOA_API Animation* const Get(const char* name);

	struct DOA_API AnimationFrame {
		std::chrono::milliseconds* duration{ 0 };
		doa::texture::Texture* texture{ 0 };

		~AnimationFrame();
	};

	struct DOA_API Animation {
		std::vector<AnimationFrame*>* frames{ 0 };

		~Animation();

	private:

		friend Animation* const animation::CreateAnimation(const char* name, const std::vector<doa::texture::Texture*>& textures, const std::chrono::milliseconds& durationOfAFrame);
		friend Animation* const animation::CreateAnimation(const char* name, const std::vector<doa::texture::Texture*>& textures, const std::vector<std::chrono::milliseconds>& durationsOfFrames);
	};
}

namespace internal::animation {

	void purge();
}