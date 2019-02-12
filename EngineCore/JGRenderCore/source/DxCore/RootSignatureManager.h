#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	enum class ERootSigType
	{
		SSAO,
		Common,
		CommonSkinned
	};
	class ShaderRootSignatureBase;
	class RCORE_EXPORT RootSignatureManager
	{
	private:
		std::vector<std::shared_ptr<ShaderRootSignatureBase>>      m_RootSigMems;
		std::unordered_map<ERootSigType, ShaderRootSignatureBase*> m_RootSigs;
	public:
		RootSignatureManager() = default;
	public:
		void CreateManager(ID3D12Device* Device);
		ShaderRootSignatureBase* GetRootSig(ERootSigType type);
	private:
		void CreateRootSig(ERootSigType type, ID3D12Device* Device);
	};


}