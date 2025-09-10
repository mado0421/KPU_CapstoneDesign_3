#pragma once
class Vertex
{
public:
	XMFLOAT3 m_xmf3Pos;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3Tangent;
	//XMFLOAT3 m_xmf3BiTangent;
	XMINT4 m_xmi4BoneIndices;
	XMFLOAT4 m_xmi4BoneWeights;
	XMFLOAT2 m_xmf2UV;

	Vertex() = default;

	Vertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3Normal, XMFLOAT2 xmf2UV)
		: m_xmf3Pos(xmf3Pos),
		  m_xmf3Normal(xmf3Normal),
		  m_xmf3Tangent(XMFLOAT3(0, 0, 0)),
		  //m_xmf3BiTangent(XMFLOAT3(0, 0, 0)),
		  m_xmi4BoneIndices(XMINT4(0, 0, 0, 0)),
		  m_xmi4BoneWeights(XMFLOAT4(0, 0, 0, 0)),
		  m_xmf2UV(xmf2UV)
	{
	}

	Vertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3Normal, XMFLOAT3 xmf3Tangent/*, XMFLOAT3 xmf3BiTangent*/, XMFLOAT2 xmf2UV)
		: m_xmf3Pos(xmf3Pos),
		  m_xmf3Normal(xmf3Normal),
		  m_xmf3Tangent(xmf3Tangent),
		  //m_xmf3BiTangent(xmf3BiTangent),
		  m_xmi4BoneIndices(XMINT4(0, 0, 0, 0)),
		  m_xmi4BoneWeights(XMFLOAT4(0, 0, 0, 0)),
		  m_xmf2UV(xmf2UV)
	{
	}

	Vertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3Normal, XMFLOAT3 xmf3Tangent/*, XMFLOAT3 xmf3BiTangent*/, XMFLOAT2 xmf2UV,
	       XMINT4 xmi4BoneIdx, XMFLOAT4 xmf4BoneWeight)
		: m_xmf3Pos(xmf3Pos),
		  m_xmf3Normal(xmf3Normal),
		  m_xmf3Tangent(xmf3Tangent),
		  //m_xmf3BiTangent(xmf3BiTangent),
		  m_xmi4BoneIndices(xmi4BoneIdx),
		  m_xmi4BoneWeights(xmf4BoneWeight),
		  m_xmf2UV(xmf2UV)
	{
	}
};
