#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"

namespace JGRC
{
	enum class ESamplerType
	{
		Wrap,Clamp,
	};
	
	class SamplerState
	{
	private:
		static SamplerState * instance;
	private:
		typedef std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStateArray;
		SamplerStateArray m_umSamplerStates;
	private:
		SamplerState(const SamplerState& copy) = delete;
		SamplerState& operator=(const SamplerState& SS) = delete;
		SamplerState();
		~SamplerState();
	public:
		static SamplerState* GetInstance();
		static void Release();
	    bool AddSampler(const std::wstring& SamplerName,const ESamplerType type, const D3D11_FILTER filter, const float MipLODBias = 0 ,const UINT MaxAnisotropy = 1,
			D3D11_COMPARISON_FUNC func = D3D11_COMPARISON_ALWAYS, const jgVec4& color = jgVec4(),const float MinLOD = 0,const float MaxLOD = D3D11_FLOAT32_MAX);
	public:
		ID3D11SamplerState* Get(const std::wstring& name);
		ID3D11SamplerState** GetAddress(const std::wstring& name);
	};
}