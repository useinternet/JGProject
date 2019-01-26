#pragma once
#include"DxCommon/DxCommon.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Shader/Shader.h"

// 인스턴스 모드와 매크로 조작을 어케 할지 고민고민.
namespace JGRC
{
	enum class ETextureSlot
	{
		Diffuse = 0,
		Normal  = 1,
		Custom0,
		Custom1,
		Custom2,
		Custom3,
		Custom4,
		Custom5
	};
	/*
	Exp : 머터리얼 목록 
	* 이름
	* PSO 모드
	* 셰이더 경로
	* 반사맵 여부
	* 굴절맵 여부 */
	struct MaterialDesc
	{
		std::string  Name;
		EPSOMode  Mode = EPSOMode::DEFAULT;
		std::wstring ShaderPath;
		bool bCubMapDynamic = false;
		bool bCubeMapStatic = false;
	};
	//
	class RCORE_EXPORT JGMaterial
	{
	private:
		// 머터리얼 힙 인덱스 및 PSO
		UINT m_MatCBIndex  = 0;

		// 머터리얼 데이터 (목록, 저장 값)
		std::unique_ptr<MaterialData> m_Data;
		std::unique_ptr<MaterialDesc> m_Desc;
		// 지연된 텍스쳐 업데이트를 위한 Que
		std::queue<std::pair<ETextureSlot, std::wstring>> m_TextureDataQue;
		std::unordered_map<ETextureSlot, std::wstring>    m_TexturePaths;
		// 프레임 알림이
		int UpdateNotify = CPU_FRAMERESOURCE_NUM;
	private:
		JGMaterial(const JGMaterial& material) = delete;
		JGMaterial& operator=(const JGMaterial& material) = delete;
	public:
		JGMaterial(UINT Index, const MaterialDesc& desc);
		~JGMaterial() {}
	public:
		void Update(class FrameResource* CurrentFrameResource);
		std::vector<ShaderMacroPack> GetMacroPack();
	public:  // 머터리얼 데이터 Set 접근자.
		void SetTexture(ETextureSlot slot,const std::wstring& TexturePath);
		void SetMaterialData(const MaterialData& Data);
		void SetDiffuseAlbedo(float r, float g, float b, float a);
		void SetFresnelR0(float x, float y, float z);
		void SetRoughness(float x);
		void SetRefractive(float x);
		void SetReflectivity(float x);
		void SetMatTransform(const DirectX::XMFLOAT4X4& mat);
	public:
		const std::wstring& GetTexturePath(ETextureSlot slot) { return m_TexturePaths[slot]; }
	public:  // 머터리얼 Get접근자
		MaterialDesc*       GetDesc() const { return m_Desc.get(); }
		const MaterialData& GetData() const { return *m_Data; }
		UINT CBIndex() const { return m_MatCBIndex; }
	private: // 프레임 알림 함수들
		void ClearNotify() { UpdateNotify = CPU_FRAMERESOURCE_NUM; }
		void UpdatePerFrame() { UpdateNotify--; }
		bool IsCanUpdate()    { return UpdateNotify > 0; }
	};
	//
	//
	//
	class RCORE_EXPORT JGMaterialCreater
	{
		typedef std::unordered_map<std::string,  std::unique_ptr<JGMaterial>> MaterialArray;
	private:
		MaterialArray m_Materials;
		int m_MatCBIndex = -1;
	private:
		JGMaterialCreater(const JGMaterialCreater& copy) = delete;
		JGMaterialCreater& operator=(const JGMaterialCreater& rhs) = delete;
	public:
		JGMaterialCreater()  = default;
		~JGMaterialCreater() = default;
	public: // 머터리얼 생성 및 가져오기
		JGMaterial*    CreateMaterial(const MaterialDesc& Desc);
		JGMaterial*    GetMaterial(const std::string& name);
		MaterialArray& GetArray() { return m_Materials; }
	public:
		UINT Size() const { return (UINT)m_Materials.size(); }
	};
}