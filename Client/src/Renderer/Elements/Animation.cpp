#include "pch.h"
#include "Animation.h"

void animation::GetFrameIdxAndNormalizedTime(const AnimationClip* clip, const double       elapsed_time,
											 double&              normalized_time, XMINT4& key_indices)
{
	const double time = fmod(elapsed_time, clip->length);

	for (int time_idx = 0; time_idx < clip->times.size(); time_idx++)
	{
		if (clip->times[time_idx] <= time && time <= clip->times[time_idx + 1])
		{
			key_indices.y = time_idx;
			key_indices.z = time_idx + 1;

			if (time_idx != 0) key_indices.x = key_indices.y - 1;
			else key_indices.x               = 0;

			if (key_indices.z != clip->times.size() - 1) key_indices.w = key_indices.z + 1;
			else key_indices.w                                         = key_indices.z;

			normalized_time = (time - clip->times[key_indices.y]) / (clip->times[key_indices.z] - clip->times[
				key_indices.y]);
			return;
		}
	}
}

XMVECTOR animation::GetLocalTransform(const AnimationClip* clip, const int bone_idx, const double normalized_time,
									  const XMINT4         key_indices)
{
	if (0 == normalized_time) return XMLoadFloat4(&clip->bones[bone_idx].keys[key_indices.x].rotation);

	return XMQuaternionSlerp(XMLoadFloat4(&clip->bones[bone_idx].keys[key_indices.y].rotation),
							 XMLoadFloat4(&clip->bones[bone_idx].keys[key_indices.z].rotation), normalized_time);
}
