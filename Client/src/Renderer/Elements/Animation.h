#pragma once

using ClipPair = vector<pair<string, float>>;

struct Keyframe
{
    XMFLOAT4 xmf4QuatRotation;
    XMFLOAT3 xmf3Translation;
};

struct Bone
{
    XMFLOAT4X4       toDressposeInv;
    XMFLOAT4X4       toParent;
    int              parentIdx;
    vector<Keyframe> keys;
};

struct AnimClip
{
    string         strClipName;
    vector<Bone>   vecBone;
    vector<double> vecTimes;
    double         fClipLength = 0;
};

class AnimationManager
{
public:
    void Initialize();

    void      AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    bool      IsAleadyExist(const char* name);
    AnimClip* GetAnimClip(const char* name);

private:
    unordered_map<string, AnimClip*> m_uomAnimClip;
};

// Object���� vecAnimation�� �޾Ƽ� toWorld�� animTransform�� ä���ִ� ����
// Object���� vecAnimation�� �޾Ƽ� toWorld�� animTransform�� Blend ���ִ� ����
namespace AnimationCalculate
{
    void     GetFrameIdxAndNormalizedTime(AnimClip* clip, float fTime, float& OutfNormalizedTime, XMINT4& OutIdx);
    XMVECTOR GetLocalTransform(AnimClip* clip, int boneIdx, float fNormalizedTime, XMINT4 frameIdx);

    void     InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out);
    XMFLOAT3 Interpolate(XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3, float t);
};
