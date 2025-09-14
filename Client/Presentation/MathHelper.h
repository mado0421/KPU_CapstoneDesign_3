#pragma once
#include <DirectXMath.h>
using namespace DirectX;

inline bool  IsZero(float fValue) { return fabsf(fValue) < FLT_EPSILON; }
inline bool  IsEqual(float fA, float fB) { return IsZero(fA - fB); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }

inline void Swap(float* pfS, float* pfT)
{
    float fTemp = *pfS;
    *pfS        = *pfT;
    *pfT        = fTemp;
}

inline bool IsIn(float target, float min, float max)
{
    if (target < min) return false;
    if (target > max) return false;
    return true;
}

inline void Clamp(float& val, const float min, const float max)
{
    val = min(val, max);
    val = max(val, min);
}

constexpr auto vector2Epsilon{XMFLOAT2(FLT_EPSILON, FLT_EPSILON)};
constexpr auto vector3Epsilon{XMFLOAT3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)};

inline void quat_2_euler_d3d(const XMFLOAT4& q, float& yaw, float& pitch, float& roll)
{
    float sqw = q.w * q.w;
    float sqx = q.x * q.x;
    float sqy = q.y * q.y;
    float sqz = q.z * q.z;
    pitch     = asinf(2.0f * (q.w * q.x - q.y * q.z));                          // rotation about x-axis
    yaw       = atan2f(2.0f * (q.x * q.z + q.w * q.y), -sqx - sqy + sqz + sqw); // rotation about y-axis
    roll      = atan2f(2.0f * (q.x * q.y + q.w * q.z), -sqx + sqy - sqz + sqw); // rotation about z-axis
}

namespace Vector2
{
    inline bool CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs) { return XMVector3NearEqual(XMLoadFloat2(&lhs), XMLoadFloat2(&rhs), XMLoadFloat2(&vector2Epsilon)); }

    inline XMFLOAT2 Add(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
    {
        XMFLOAT2 xmf2Result;
        XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + XMLoadFloat2(&xmf2Vector2));
        return xmf2Result;
    }

    inline XMFLOAT2 Add(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2, float fScalar)
    {
        XMFLOAT2 xmf2Result;
        XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + XMLoadFloat2(&xmf2Vector2) * fScalar);
        return xmf2Result;
    }

    inline XMFLOAT2 Subtract(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
    {
        XMFLOAT2 xmf2Result;
        XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) - XMLoadFloat2(&xmf2Vector2));
        return xmf2Result;
    }

    inline XMFLOAT2 Lerp(const XMFLOAT2& v0, const XMFLOAT2& v1, float t) { return Add(v0, Subtract(v1, v0), t); }
}

namespace Vector4
{
    inline XMFLOAT4 QuatIdentity()
    {
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, XMQuaternionIdentity());
        return quat;
    }

    inline XMFLOAT4 QuatSlerp(const XMFLOAT4& q0, const XMFLOAT4& q1, float t)
    {
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), t));
        return quat;
    }

    inline XMFLOAT4 QuatFromMtx(const XMFLOAT4X4& mtx)
    {
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&mtx)));
        return quat;
    }

    inline XMFLOAT4 QuatFromAngle(const XMFLOAT3& v)
    {
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(v.y), XMConvertToRadians(v.z), XMConvertToRadians(v.x)));
        return quat;
    }

    inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
        return xmf4Result;
    }

    inline XMFLOAT4 Multiply(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) * XMLoadFloat4(&xmf4Vector2));
        return xmf4Result;
    }

    inline XMFLOAT4 Multiply(float fScalar, const XMFLOAT4& xmf4Vector)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
        return xmf4Result;
    }

    inline XMVECTOR QuatFromXYZAngle(const XMFLOAT3& angle) { return XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)); }
}

namespace Vector3
{
    inline bool IsZero(const XMFLOAT3& xmf3Vector)
    {
        if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z)) return true;
        return false;
    }

    inline XMFLOAT3 ScalarProduct(const XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize) XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
        else XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
        return xmf3Result;
    }

    inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
        return xmf3Result;
    }

    inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2) * fScalar);
        return xmf3Result;
    }

    inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
        return xmf3Result;
    }

    inline float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
        return xmf3Result.x;
    }

    inline XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool bNormalize = true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize) XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
        else XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
        return xmf3Result;
    }

    inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 m_xmf3Normal;
        XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
        return m_xmf3Normal;
    }

    inline XMFLOAT3 Multiply(float fScalar, const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, fScalar * XMLoadFloat3(&xmf3Vector));
        return xmf3Result;
    }

    inline float Length(const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
        return xmf3Result.x;
    }

    inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
    {
        XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
        return XMConvertToDegrees(XMVectorGetX(xmvAngle));
        //return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
    }

    inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2) { return Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)); }

    inline XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
        return xmf3Result;
    }

    inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
        return xmf3Result;
    }

    inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix) { return TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)); }

    inline bool CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs) { return XMVector3NearEqual(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs), XMLoadFloat3(&vector3Epsilon)); }

    inline XMFLOAT3 Lerp(const XMFLOAT3& v0, const XMFLOAT3& v1, float t) { return Add(v0, Subtract(v1, v0), t); }

    inline XMFLOAT3 PosFromMtx(const XMFLOAT4X4& mtx) { return XMFLOAT3{mtx._41, mtx._42, mtx._43}; }

    inline XMFLOAT3 AngleFromQuat(const XMFLOAT4& quat)
    {
        XMFLOAT3 tmp;
        quat_2_euler_d3d(quat, tmp.y, tmp.x, tmp.z);
        tmp.x = XMConvertToDegrees(tmp.x);
        tmp.y = XMConvertToDegrees(tmp.y);
        tmp.z = XMConvertToDegrees(tmp.z);
        return tmp;
    }

    inline XMFLOAT3 AngleFromMtx(const XMFLOAT4X4& mtx)
    {
        XMFLOAT4 tmp = Vector4::QuatFromMtx(mtx);
        return AngleFromQuat(tmp);
    }
}

namespace Matrix4x4
{
    inline XMFLOAT4X4 Identity()
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMMATRIX& xmmtxMatrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 OrthographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ));
        return xmmtx4x4Result;
    }

    inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
        return xmmtx4x4Result;
    }

    inline void ToTransform(XMFLOAT4X4* mtx, const XMFLOAT3& pos, const XMFLOAT4& quat)
    {
        XMStoreFloat4x4(mtx, XMMatrixRotationQuaternion(XMLoadFloat4(&quat)));
        mtx->_41 = pos.x;
        mtx->_42 = pos.y;
        mtx->_43 = pos.z;
    }

    inline void InterpolateMtx(XMFLOAT4X4* pOutputMtx, const XMFLOAT4X4& mtx0, const XMFLOAT4X4& mtx1, float t)
    {
        ToTransform(pOutputMtx, Vector3::Lerp(Vector3::PosFromMtx(mtx0), Vector3::PosFromMtx(mtx1), t), Vector4::QuatSlerp(Vector4::QuatFromMtx(mtx0), Vector4::QuatFromMtx(mtx1), t));
    }

    inline XMMATRIX MakeFromXYZAngle(const XMFLOAT3& angle) { return XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)); }

    inline XMFLOAT4X4 InverseTranspose(const XMFLOAT4X4& m)
    {
        XMFLOAT4X4 a = m;
        a._41        = 0.0f;
        a._42        = 0.0f;
        a._43        = 0.0f;
        a._44        = 1.0f;

        XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&a));
        XMStoreFloat4x4(&a, XMMatrixTranspose(XMMatrixInverse(&det, XMLoadFloat4x4(&a))));
        return a;
    }
}

inline XMMATRIX XMMatrixRotationRollPitchYawDegree(float x, float y, float z) { return XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)); }

inline float GetDistance(XMFLOAT3 a, XMFLOAT3 b) { return Vector3::Length(Vector3::Subtract(b, a)); }
