#pragma once

//#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
#define CAM_MOVE_SPEED		3
#define CAM_ROTATE_SPEED	3

class Object;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4ViewInv;
	XMFLOAT4X4 m_xmf4x4ProjectionInv;
	XMFLOAT4X4 m_xmf4x4Texture;
	XMFLOAT3 m_xmf3Position;
	float m_xmfTime;
};

class Camera
{
protected:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Direction;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	XMFLOAT3 m_xmf3LookAtWorld;
	XMFLOAT3 m_xmf3Offset;
	float m_fTimeLag;

	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4ViewInv;
	XMFLOAT4X4 m_xmf4x4ProjectionInv;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

public:
	Camera();
	virtual ~Camera();

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio,
	                              float fFOVAngle);
	void RegenerateViewMatrix();

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetPosition() { return (m_xmf3Position); }
	XMFLOAT3& GetLookAtPosition() { return (m_xmf3LookAtWorld); }

	XMFLOAT3& GetRight() { return (m_xmf3Right); }
	XMFLOAT3& GetUp() { return (m_xmf3Up); }
	XMFLOAT3& GetLook() { return (m_xmf3Look); }

	void SetOffset(const XMFLOAT3& xmf3Offset)
	{
		m_xmf3Offset = xmf3Offset;
		m_xmf3Position.x += xmf3Offset.x;
		m_xmf3Position.y += xmf3Offset.y;
		m_xmf3Position.z += xmf3Offset.z;
	}

	void SetOffset(const XMFLOAT3& xmf3Offset, const XMFLOAT3& objPos)
	{
		m_xmf3Offset = xmf3Offset;
		m_xmf3Position = Vector3::Add(objPos, xmf3Offset);
	}

	XMFLOAT3& GetOffset() { return (m_xmf3Offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return (m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return (m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return (m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return (m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return (m_d3dScissorRect); }


	virtual void SetTarget(void* target)
	{
	}

	virtual Object* GetTarget() { return nullptr; }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
		m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

		//m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = m_xmf3Right.z;
		//m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = m_xmf3Up.z;
		//m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = m_xmf3Look.z;
	}

	virtual void Update(float fTimeElapsed);

	virtual void SetLookAt(const XMFLOAT3& xmf3LookAt)
	{
	}

	void MoveForward(float speed)
	{
		m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, m_xmf3Look));
	}

	void MoveRight(float speed)
	{
		m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, m_xmf3Right));
	}

	void MoveUp(float speed) { m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, m_xmf3Up)); }

	void MoveLeft(float speed)
	{
		m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, Vector3::Multiply(-1, m_xmf3Right)));
	}

	void MoveBackward(float speed)
	{
		m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, Vector3::Multiply(-1, m_xmf3Look)));
	}

	void MoveDown(float speed)
	{
		m_xmf3Direction = Vector3::Add(m_xmf3Direction, Vector3::Multiply(speed, Vector3::Multiply(-1, m_xmf3Up)));
	}

protected:
	virtual void Move(const XMFLOAT3& xmf3Shift)
	{
		m_xmf3Position.x += xmf3Shift.x;
		m_xmf3Position.y += xmf3Shift.y;
		m_xmf3Position.z += xmf3Shift.z;
	}
};

class BoardCamera : public Camera
{
public:
	BoardCamera();
	~BoardCamera() override;

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance);
};

class FollowCamera : public Camera
{
	Object* m_pTarget = nullptr;

public:
	FollowCamera();
	~FollowCamera() override;

	void SetTarget(void* target) override;
	Object* GetTarget() override;
	void Update(float fTimeElapsed) override;
	void SetLookAt(const XMFLOAT3& xmf3LookAt) override;
};
