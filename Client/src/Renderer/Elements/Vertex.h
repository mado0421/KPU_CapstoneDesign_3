#pragma once
class Vertex
{
public:
	Vertex() = default;

	Vertex(const XMFLOAT3 position, const XMFLOAT3 normal, const XMFLOAT2 uv)
		: position(position), normal(normal), uv(uv) {}

	Vertex(const XMFLOAT3 position, const XMFLOAT3 normal, const XMFLOAT3 tangent, const XMFLOAT2 uv)
		: position(position), normal(normal), tangent(tangent), uv(uv) {}

	Vertex(const XMFLOAT3 position, const XMFLOAT3     normal, const XMFLOAT3 tangent, const XMFLOAT2 uv,
		   const XMINT4   bone_indices, const XMFLOAT4 bone_weights)
		: position(position),
		  normal(normal),
		  tangent(tangent),
		  bone_indices(bone_indices),
		  bone_weights(bone_weights),
		  uv(uv) {}

	XMFLOAT3 position     = XMFLOAT3(0, 0, 0);
	XMFLOAT3 normal       = XMFLOAT3(0, 0, 0);
	XMFLOAT3 tangent      = XMFLOAT3(0, 0, 0);
	XMINT4   bone_indices = XMINT4(0, 0, 0, 0);
	XMFLOAT4 bone_weights = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT2 uv           = XMFLOAT2(0, 0);
};
