#pragma once


#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class JgVertex
	{
	public:
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 normal   = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 tangent  = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 tex      = { 0.0f,0.0f };
	public:
		JgVertex() = default;
		JgVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float cx, float cy) :
			position(DirectX::XMFLOAT3(px, py, pz)),
			normal(DirectX::XMFLOAT3(nx, ny, nz)),
			tangent(DirectX::XMFLOAT3(tx, ty, tz)),
			tex(DirectX::XMFLOAT2(cx, cy)) {}


		uint32_t Size() const {
			return 44;
		}
		static D3D12_INPUT_ELEMENT_DESC* GetInputLayoutDesc();
		static uint32_t GetNumInputLayout();
	};

	class JgSkinnedVertex
	{
	public:
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 normal   = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 tangent  = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 tex      = { 0.0f,0.0f };
		float boneWeights[4]       = { 0.0f,0.0f,0.0f,0.0f };
		int   bondId[4]            = { 0,0,0,0 };
	public:
		JgSkinnedVertex() = default;
		JgSkinnedVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float cx, float cy,
			float bw1, float bw2, float bw3, float bw4,
			int bi1, int bi2, int bi3, int bi4) :
			position(DirectX::XMFLOAT3(px, py, pz)),
			normal(DirectX::XMFLOAT3(nx, ny, nz)),
			tangent(DirectX::XMFLOAT3(tx, ty, tz)),
			tex(DirectX::XMFLOAT2(cx, cy))
		{
			boneWeights[0] = bw1; boneWeights[1] = bw2; boneWeights[2] = bw3; boneWeights[3] = bw4;
			bondId[0] = bi1; bondId[1] = bi2; bondId[2] = bi3; bondId[3] = bi4;
		}
		uint32_t Size() const {
			return 76;
		}
		static D3D12_INPUT_ELEMENT_DESC* GetInputLayoutDesc();
		static uint32_t GetNumInputLayout();
	};

	class JgPosColorVertex
	{
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT4 color    = { 0.0f,0.0f,0.0f,0.0f };
	public:
		JgPosColorVertex() = default;
		JgPosColorVertex(
			float px, float py, float pz,
			float r, float g, float b, float a) :
			position(DirectX::XMFLOAT3(px, py, pz)),
			color(DirectX::XMFLOAT4(r, g, b, a)) {}
		uint32_t Size() const {
			return 28;
		}
		static D3D12_INPUT_ELEMENT_DESC* GetInputLayoutDesc();
		static uint32_t GetNumInputLayout();
	};
}