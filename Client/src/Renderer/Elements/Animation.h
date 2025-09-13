#pragma once

using ClipPair = vector<pair<string, float>>;

struct Key
{
	/// <summary>
	///		Quaternion 
	/// </summary>
	XMFLOAT4 rotation;
	XMFLOAT3 translation;
};

struct Bone
{
	XMFLOAT4X4  to_dressed_pose_inv;
	XMFLOAT4X4  to_parent;
	int         parent_idx;
	vector<Key> keys;
};

struct AnimationClip
{
	string         name;
	vector<Bone>   bones;
	vector<double> times;
	double         length;
};

namespace animation
{
	void GetFrameIdxAndNormalizedTime(const AnimationClip* clip, double elapsed_time, double& normalized_time,
									  XMINT4&              key_indices);

	/// <summary>
	///		Return the interpolated "local rotation (quaternion)" for a specific bone in an animation clip over time.
	/// </summary>
	XMVECTOR GetLocalTransform(const AnimationClip* clip, int bone_idx, double normalized_time, XMINT4 key_indices);
};
