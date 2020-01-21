// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::animation {
	using namespace internal::animation;

	std::map<std::string, Animation*> ANIMATIONS;

	Animation* const CreateAnimation(const std::string& name, const std::vector<doa::texture::Texture*>& textures, const std::chrono::milliseconds& durationOfAFrame) {
		Animation* anim{ new Animation() };
		anim->frames = new std::vector<AnimationFrame*>(textures.size());
		for (int i{ 0 }; i < textures.size(); ++i) {
			AnimationFrame* f{ new AnimationFrame() };
			f->duration = new std::chrono::milliseconds(durationOfAFrame);
			f->texture = textures[i];
			(*(anim->frames))[i] = f;
		}
		if (ANIMATIONS[name]) {
			delete ANIMATIONS[name];
		}
		ANIMATIONS[name] = anim;
		return anim;
	}

	Animation* const CreateAnimation(const std::string& name, const std::vector<doa::texture::Texture*>& textures, const std::vector<std::chrono::milliseconds>& durations) {
		Animation* anim{ new Animation() };
		anim->frames = new std::vector<AnimationFrame*>(textures.size());
		for (int i{ 0 }; i < textures.size(); ++i) {
			AnimationFrame* f{ new AnimationFrame() };
			f->duration = new std::chrono::milliseconds(durations[i]);
			f->texture = textures[i];
			(*(anim->frames))[i] = f;
		}
		if (ANIMATIONS[name]) {
			delete ANIMATIONS[name];
		}
		ANIMATIONS[name] = anim;
		return anim;
	}

	Animation* const Get(const std::string& name) {
		return ANIMATIONS[name];
	}

	AnimationFrame::~AnimationFrame() {
		delete duration;
	}

	Animation::~Animation() {
		for (size_t i{ 0 }; i < frames->size(); ++i) {
			delete (*frames)[i];
		}
		delete frames;
	}
}

namespace internal::animation {

	void purge() {
		for (auto itr{ doa::animation::ANIMATIONS.begin() }; itr != doa::animation::ANIMATIONS.end(); ++itr) {
			doa::animation::Animation* p = itr->second;
			delete p;
		}
		doa::animation::ANIMATIONS.clear();
	}
}