#pragma once
#include "../../Engine/ECS/Component.h"

class BoneMask;

class AnimatorComponent : public Component
{
public:
    AnimatorComponent() = delete;
    AnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy);
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
    XMFLOAT4X4 m_arrFinalResult[MAX_BONE_NUM];
};


class HumanoidControllerComponent;

class HumanoidAnimatorComponent : public AnimatorComponent
{
public:
    HumanoidAnimatorComponent() = delete;
    HumanoidAnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy);
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
    TargetBoardAnimatorComponent(Object*, const char*);
    ~TargetBoardAnimatorComponent() override;

    void Update(float fTimeElapsed) override;

private:
    float                           m_fStandInterpolationValue; // 1: Stand, 0: Down, [0~1]
    TargetBoardControllerComponent* l_TCC = nullptr;
};
