#pragma once
#include"AssetDefine.h"


namespace JgAsset
{
	class AssetAnimTransform
	{
	private:
		BonesByID* m_pBonesByNodeID;
		AssetAnimation* m_pAnim;
		uint32_t   m_BoneCount;
		AssetAnimTransformData m_Matrixs;
	public:
		void Create(float TimePos, BonesByID* BonesByNodeID,
			AssetAnimation* pAnim, uint32_t BoneCount);
		const DirectX::XMFLOAT4X4* GetData_c() const
		{
			return m_Matrixs.data();
		}
		DirectX::XMFLOAT4X4* GetData()
		{
			return &m_Matrixs[0];
		}
	private:
		void ProcessCalcAnimTransform(
			float TimePos,
			uint32_t NodeID,
			DirectX::XMFLOAT4X4& ParentMat,
			BonesByID* BonesByNodeID);

		void CalcPositionFrame(float TimePos, AssetAnimChannel* channel, DirectX::XMFLOAT3& pos);
		void CalcScaleFrame(float TimePos, AssetAnimChannel* channel, DirectX::XMFLOAT3& scale);
		void CalcRotationFrame(float TimePos, AssetAnimChannel* channel, DirectX::XMFLOAT4& quat);

		uint32_t FindPositionIndex(float TimePos, AssetAnimChannel* channel);
		uint32_t FindScaleIndex(float TimePos, AssetAnimChannel* channel);
		uint32_t FindRotationIndex(float TimePos, AssetAnimChannel* channel);

		AssetAnimChannel* FindAnimChannel(const std::string& NodeName);


	};
}