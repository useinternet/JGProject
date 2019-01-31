#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	struct JGBoneData;
	struct JGBoneNode;
	struct RCORE_EXPORT JGVectorFrame
	{
		float TimePos;
		DirectX::XMFLOAT3 Value;
	};
	struct RCORE_EXPORT JGQuatFrame
	{
		float TimePos;
		DirectX::XMFLOAT4 Value;
	};
	struct RCORE_EXPORT JGAnimChannel
	{
		std::string Name;
		
		std::vector<JGVectorFrame> AnimPositionFrames;
		std::vector<JGVectorFrame> AnimScaleFrames;
		std::vector<JGQuatFrame>   AnimRotationFrames;
	};
	class RCORE_EXPORT JGAnimation
	{
		friend class JGAnimationHelper;
	public:
		std::string Name;
		std::vector<JGAnimChannel>       AnimChannels;
		std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
		DirectX::XMFLOAT4X4              InvGlobalTransform;
		float Duration      = 0.0f;
		float TickPerSecond = 0.0f;
	private:
		std::vector<DirectX::XMFLOAT4X4> UpdateAnimation(float TimePos, JGBoneNode* Root, UINT BoneCount);
	
		void ReadAnim(float deltaTime, 
			          JGBoneNode* node,
			          std::vector<DirectX::XMFLOAT4X4>& finalTransform,
			          DirectX::XMFLOAT4X4& ParentMat);
		JGAnimChannel* FindAnimChannel(JGBoneNode* node);
		void CalcPositionFrame(float TimePos, JGAnimChannel* channel, DirectX::XMFLOAT3& pos);
		void CalcScaleFrame(float TimePos, JGAnimChannel* channel, DirectX::XMFLOAT3& scale);
		void CalcQuatFrame(float TimePos, JGAnimChannel* channel, DirectX::XMFLOAT4& quat);



		UINT FindPositionIndex(float TimePos, JGAnimChannel* channel);
		UINT FindScaleIndex(float TimePos, JGAnimChannel* channel);
		UINT FindQuatIndex(float TimePos, JGAnimChannel* channel);
	};
	class AnimTransKeyPack
	{
	public:
		JGAnimation* Anim;
		JGBoneNode*  Root;
		UINT         BoneCount;
		bool operator==(const AnimTransKeyPack& pack) const
		{
			return (pack.Anim == Anim && Root == pack.Root && BoneCount == pack.BoneCount);
		}
	};
	struct AnimTransKeyPackHash
	{
		std::size_t operator()(const AnimTransKeyPack& k) const
		{
			return ((std::hash<UINT64>()((UINT64)k.Anim)
				^ (std::hash<UINT64>()((UINT64)k.Root) << 1)) >> 1)
				^ (std::hash<UINT>()((UINT64)k.BoneCount) << 1);
		}
	};
	class JGAnimationHelper
	{

		static std::unordered_map<AnimTransKeyPack, std::vector<std::vector<DirectX::XMFLOAT4X4>>, AnimTransKeyPackHash> AnimationTransforms;
	public:
		bool  IsPlay = false;
	private:
		float TimePos = 0.0f;
		std::vector<std::vector<DirectX::XMFLOAT4X4>> AnimTransform;
		std::vector<DirectX::XMFLOAT4X4> FinalTransform;
	public:
		void BuildAnimationData(JGAnimation* anim, JGBoneNode* Root, UINT BoneCount, const std::string& meshName);
		void UpdateAnimatoin(const GameTimer& gt, JGAnimation* anim, JGBoneNode* Root, UINT BoneCount);
		SkinnedData Get();
	};

}