#pragma once

struct AnimationClip;

class AnimationManager
{
public:
	void Initialize();

	void           AddAnimationClip(const char* name);
	bool           IsExist(const char* name) const;
	AnimationClip* GetAnimClip(const char* name);

private:
	unordered_map<string, AnimationClip*> animation_clips_;
};
