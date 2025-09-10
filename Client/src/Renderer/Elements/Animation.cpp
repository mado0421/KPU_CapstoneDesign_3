#include "pch.h"
#include "Animation.h"
#include "../../IO/Importer.h"

//==================================================================
// Animation Manager
//==================================================================
void AnimationManager::Initialize() { m_uomAnimClip.clear(); }

void AnimationManager::AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    AnimClipDataImporter importer;
    auto                 animClip = new AnimClip();
    *animClip                     = importer.Load(fileName);

    m_uomAnimClip[fileName] = animClip;
}

bool AnimationManager::IsAleadyExist(const char* name) { return m_uomAnimClip.contains(name); }

AnimClip* AnimationManager::GetAnimClip(const char* name) { return m_uomAnimClip[name]; }

//==================================================================
// Animation Calculator
//==================================================================
void AnimationCalculate::GetFrameIdxAndNormalizedTime(AnimClip* clip, const float fTime, float& OutfNormalizedTime, XMINT4& OutIdx)
{
    float time = fTime;
    while (time > clip->fClipLength) time -= clip->fClipLength;

    for (int timeIdx = 0; timeIdx < clip->vecTimes.size(); timeIdx++)
    {
        if (time == clip->vecTimes[timeIdx])
        {
            OutfNormalizedTime = 0;
            OutIdx.x           = OutIdx.y = OutIdx.z = OutIdx.w = timeIdx;
            return;
        }
        if (clip->vecTimes[timeIdx] < time && time <= clip->vecTimes[timeIdx + 1])
        {
            OutIdx.y = timeIdx;
            OutIdx.z = timeIdx + 1;

            if (timeIdx != 0) OutIdx.x = OutIdx.y - 1;
            else OutIdx.x              = 0;
            if (OutIdx.z != clip->vecTimes.size() - 1) OutIdx.w = OutIdx.z + 1;
            else OutIdx.w                                       = OutIdx.z;
            // KeySelect End

            OutfNormalizedTime = (time - clip->vecTimes[OutIdx.y]) / (clip->vecTimes[OutIdx.z] - clip->vecTimes[OutIdx.y]);
            return;
        }
    }
}

XMVECTOR AnimationCalculate::GetLocalTransform(AnimClip* clip, const int boneIdx, const float fNormalizedTime, const XMINT4 frameIdx)
{
    if (0 == fNormalizedTime) return XMLoadFloat4(&clip->vecBone[boneIdx].keys[frameIdx.x].xmf4QuatRotation);

    return XMQuaternionSlerp(XMLoadFloat4(&clip->vecBone[boneIdx].keys[frameIdx.y].xmf4QuatRotation), XMLoadFloat4(&clip->vecBone[boneIdx].keys[frameIdx.z].xmf4QuatRotation), fNormalizedTime);
}

void AnimationCalculate::InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out)
{
    XMStoreFloat4(&out.xmf4QuatRotation, XMQuaternionSlerp(XMLoadFloat4(&k1.xmf4QuatRotation), XMLoadFloat4(&k2.xmf4QuatRotation), t));
    out.xmf3Translation = Interpolate(k0.xmf3Translation, k1.xmf3Translation, k2.xmf3Translation, k3.xmf3Translation, t);
}

XMFLOAT3 AnimationCalculate::Interpolate(const XMFLOAT3 v0, const XMFLOAT3 v1, const XMFLOAT3 v2, const XMFLOAT3 v3, float t)
{
    XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3), t);
    XMFLOAT3 result;

    XMStoreFloat3(&result, resultV);
    return result;
}
