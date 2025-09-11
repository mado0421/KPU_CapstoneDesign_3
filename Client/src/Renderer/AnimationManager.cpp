#include "pch.h"
#include "AnimationManager.h"

#include "Elements/Animation.h"

#include "src/IO/Importer.h"

void AnimationManager::Initialize() { animation_clips_.clear(); }

void AnimationManager::AddAnimationClip(const char* name)
{
	AnimClipDataImporter importer;
	AnimationClip* const animation_clip = new AnimationClip();
	*animation_clip                     = importer.Load(name);

	animation_clips_[name] = animation_clip;
}

bool AnimationManager::IsExist(const char* name) const { return animation_clips_.contains(name); }

AnimationClip* AnimationManager::GetAnimClip(const char* name) { return animation_clips_[name]; }
