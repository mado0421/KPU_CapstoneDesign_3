#pragma once
#include "Presentation/Game/Component/Component.h"

class BoneMask;

class AnimatorComponent : public Component
{
public:
    AnimatorComponent() = delete;
    AnimatorComponent(GameObject* pObject, const char* strClipNameForBoneHierarchy);
    ~AnimatorComponent() override;

    XMFLOAT4X4* GetFinalResultAnimationTransform();
    XMMATRIX    GetToWorldTransform(int boneIdx);
    XMMATRIX    GetFinalResultTransform(int boneIdx);

protected:
    void CalcToWorld();

    XMFLOAT4X4 m_arrToDressInv[MAX_BONE_NUM];
    XMFLOAT4X4 m_arrToParent[MAX_BONE_NUM];
    int        m_arrParentIdx[MAX_BONE_NUM];
    int        m_numBone;

    XMFLOAT4X4 m_arrToWorld[MAX_BONE_NUM];
    XMFLOAT4   m_arrLocalRotation[MAX_BONE_NUM];
};


class HumanoidControllerComponent;

class HumanoidAnimatorComponent : public AnimatorComponent
{
public:
    HumanoidAnimatorComponent() = delete;
    HumanoidAnimatorComponent(GameObject* pObject, const char* strClipNameForBoneHierarchy);
    ~HumanoidAnimatorComponent() override;

    void Update(float fTimeElapsed) override;

protected:
    BoneMask* m_pAimingMask = nullptr;

private:
    HumanoidControllerComponent* l_HCC = nullptr;
};

class TargetBoardControllerComponent;

class TargetBoardAnimatorComponent : public AnimatorComponent
{
public:
    TargetBoardAnimatorComponent() = delete;
    TargetBoardAnimatorComponent(GameObject*, const char*);
    ~TargetBoardAnimatorComponent() override;

    void Update(float fTimeElapsed) override;

private:
    float                           m_fStandInterpolationValue; // 1: Stand, 0: Down, [0~1]
    TargetBoardControllerComponent* l_TCC = nullptr;
};
