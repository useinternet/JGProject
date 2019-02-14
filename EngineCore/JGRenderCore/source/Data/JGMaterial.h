#pragma once
#include"DxCommon/DxCommon.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Shader/Shader.h"

namespace JGRC
{
	struct RCORE_EXPORT MaterialDesc
	{
		std::string  Name;
		EPSOMode     Mode = EPSOMode::DEFAULT;
		std::wstring ShaderPath;
	};
	struct RCORE_EXPORT MatPersonalData
	{
		UINT CBIndex = 0;
		std::string Name;
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.0f, 0.0f, 0.0f };
		float Roughness = 0.0f;
		float Metallic  = 0.0f;
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

		// 프레임 알림이
		int UpdateNotify = CPU_FRAMERESOURCE_NUM;

		void ClearNotify()    { UpdateNotify = CPU_FRAMERESOURCE_NUM; }
		void UpdatePerFrame() { UpdateNotify--; }
		bool IsCanUpdate()    { return UpdateNotify > 0; }
	};
	struct RCORE_EXPORT MatShareData
	{
		UINT TextureIndex[8];
	};
	//
	class RCORE_EXPORT JGMaterial
	{
	private:
		MatShareData m_ShareData;
		std::unordered_map<std::string, MatPersonalData*> m_PersonalData;
		std::unique_ptr<MaterialDesc>  m_Desc;
		// 지연된 텍스쳐 업데이트를 위한 Que
		std::queue<std::pair<ETextureSlot, std::wstring>> m_TextureDataQue;
		std::unordered_map<ETextureSlot, std::wstring>    m_TexturePaths;

		int UpdateNotify = CPU_FRAMERESOURCE_NUM;
	private:
		JGMaterial(const JGMaterial& material) = delete;
		JGMaterial& operator=(const JGMaterial& material) = delete;
	public:
		JGMaterial(const MaterialDesc& desc);
		~JGMaterial() {}
	public:
		void Update(class FrameResource* CurrentFrameResource);
		EShaderFlag GetFlag();
	public:  // 머터리얼 데이터 Set 접근자.
		MatPersonalData* AddData(const std::string& name);
		MatPersonalData* GetData(const std::string& name);
		void SetTexture(ETextureSlot slot,const std::wstring& TexturePath);
	public:
		const std::wstring& GetTexturePath(ETextureSlot slot) { return m_TexturePaths[slot]; }
	public:  // 머터리얼 Get접근자
		MaterialDesc*       GetDesc() const   { return m_Desc.get(); }
		void ClearNotify()    { UpdateNotify = CPU_FRAMERESOURCE_NUM; }
		void UpdatePerFrame() { UpdateNotify--; }
		bool IsCanUpdate()    { return UpdateNotify > 0; }
	};
}