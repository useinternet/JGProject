#pragma once
#include "Object/ReObject.h"
#include "Object/Shader/ShaderDefined.h"
#include "Object/DxObject/Resource.h"
namespace RE
{
	class GraphicsShaderModule;
	class GraphicsPipelineState;
	class Texture;
	class Shader;

	/* 머터리얼의 기능
	- 셰이더 변수 조작
	- Pixel 코드 조작
	- 텍스쳐 주입
	- 각 머터리얼 용도에 맞게 탑재된 셰이더 모듈로 돌린다.
	- 컴파일 하기전 텍스쳐 이름을 바꾼다.
	**  먼저 구현할 것 **
	- 텍스쳐 주입
	*/
	class RenderItem;
	class SBDConstantBuffer;
	class ReMaterialController;
	enum class ERenderItemUsage;
	class RENDERENGINE_API ReMaterial : public ReObject
	{

	public:
		GraphicsPipelineState* GetPSO() const {
			return m_PSO.get();
		}
		ReMaterial(const std::string& name, ERenderItemUsage usage);
	public:
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		bool AddTexture(const std::string& name, const std::string& asset_texture_path);
		bool AddTexture(const std::string& name, const Texture& t = Texture());
		bool SetTexture(uint32_t index, const std::string& asset_texture_path);
		bool SetTexture(const std::string& name, const std::string& asset_texture_path);
		bool SetTexture(uint32_t index, const Texture& t);
		bool SetTexture(const std::string& name, const Texture& t);
		bool EraseTexture(const std::string& name);
		bool EraseTexture(uint32_t index);
		bool TextureReName(const std::string& origin_name, const std::string& new_name);
		void SetTextureCount(uint32_t count);
		uint32_t GetTextureCount() const;
		void SetCode(const std::string& code);
		bool Compile();
		uint32_t       GetBindedTextureIndex(const std::string& name);
		uint32_t       GetBindedTextureCount() const;
		const std::vector<Texture>& GetBindedTextures() const;
		ERenderItemUsage GetUsage() const {
			return m_Usage;
		}
		SBDConstantBuffer* GetMaterialCB() const {
			return m_MaterialCB.get();
		}

		void EraseValueInMaterialCB(const std::string& name);
		ReMaterialController* GetController();

		bool                  DeleteController(ReMaterialController* matController);
	public: // 값 조절
		void AddIntInMaterialCB(const std::string& name, int init_value = 0.0f);
		void AddInt2InMaterialCB(const std::string& name, const JVector2Int& init_value = JVector2Int());
		void AddInt3InMaterialCB(const std::string& name, const JVector3Int& init_value = JVector3Int());
		void AddInt4InMaterialCB(const std::string& name, const JVector4Int& init_value = JVector4Int());

		void AddUintInMaterialCB(const std::string& name,  uint32_t init_value = 0.0f);
		void AddUint2InMaterialCB(const std::string& name, const JVector2Uint& init_value = JVector2Uint());
		void AddUint3InMaterialCB(const std::string& name, const JVector3Uint& init_value = JVector3Uint());
		void AddUint4InMaterialCB(const std::string& name, const JVector4Uint& init_value = JVector4Uint());

		void AddFloatInMaterialCB(const std::string& name, float init_value = 0.0f);
		void AddFloat2InMaterialCB(const std::string& name, const JVector2& init_value = JVector2());
		void AddFloat3InMaterialCB(const std::string& name, const JVector3& init_value = JVector3());
		void AddFloat4InMaterialCB(const std::string& name, const JVector4& init_value = JVector4());

		void AddMatrixInMaterialCB(const std::string& name, const JMatrix& init_value = JMatrix::Identity());
	private:
		using ReMaterialControllerPool = std::unordered_map<ReMaterialController*, std::shared_ptr<ReMaterialController>>;
		std::shared_ptr<GraphicsPipelineState>        m_PSO;
		std::unordered_map<std::string, uint32_t>     m_TextureIndexMap;
		std::vector<Texture>                          m_BindedTextures;
		std::shared_ptr<SBDConstantBuffer>            m_MaterialCB;
		std::string m_PixelCode;
		std::shared_ptr<Shader> m_CompiledPixelShader;

		ERenderItemUsage m_Usage;
		uint32_t m_MatControllerCount = 0;
		ReMaterialControllerPool m_MatControllerPool;
	};

	class RENDERENGINE_API ReMaterialController : public ReObject
	{
		friend ReMaterial;
	private:
		ReMaterial* m_MatOwner = nullptr;
		std::shared_ptr<SBDConstantBuffer>    m_MaterialCB;
		std::vector<Texture>                  m_BindedTextures;
	public:
		ReMaterialController(ReMaterial* owner);
	public:
		std::vector<byte> GetCBData() const;
		void     SetTexture(const std::string& name, const std::string& asset_texture_path);
		void     SetTexture(const std::string& name, const Texture& t);
		uint32_t GetTextureCount() const;
		SBDConstantBuffer* GetSBDMatCB() const {
			return m_MaterialCB.get();
		}
		const std::vector<Texture>& GetTextureArray() const;

		ReMaterial* GetMatOwner() const
		{
			return m_MatOwner;
		}
		ERenderItemUsage GetUsage() const {
			return m_MatOwner->GetUsage();
		}
	public:
		bool SetValueAsInt(const std::string& name, int value);
		bool SetValueAsInt2(const std::string& name, const JVector2Int& value);
		bool SetValueAsInt3(const std::string& name, const JVector3Int& value);
		bool SetValueAsInt4(const std::string& name, const JVector4Int&  value);
		bool SetValueAsUInt(const std::string& name, uint32_t value);
		bool SetValueAsUInt2(const std::string& name, const JVector2Uint& value);
		bool SetValueAsUInt3(const std::string& name, const JVector3Uint& value);
		bool SetValueAsUInt4(const std::string& name, const JVector4Uint& value);
		bool SetValueAsFloat(const std::string& name, float value);
		bool SetValueAsFloat2(const std::string& name, const JVector2& value);
		bool SetValueAsFloat3(const std::string& name, const JVector3& value);
		bool SetValueAsFloat4(const std::string& name, const JVector4& value);
		bool SetValueAsMatrix(const std::string& name, const JMatrix& value);
		bool GetValueAsInt(const std::string& name, int& out);
		bool GetValueAsInt2(const std::string& name,  JVector2Int& out);
		bool GetValueAsInt3(const std::string& name,  JVector3Int& out);
		bool GetValueAsInt4(const std::string& name,  JVector4Int& out);
		bool GetValueAsUInt(const std::string& name, uint32_t& out);
		bool GetValueAsUInt2(const std::string& name,  JVector2Uint& out);
		bool GetValueAsUInt3(const std::string& name,  JVector3Uint& out);
		bool GetValueAsUInt4(const std::string& name,  JVector4Uint& out);
		bool GetValueAsFloat(const std::string& name, float& out);
		bool GetValueAsFloat2(const std::string& name,  JVector2& out);
		bool GetValueAsFloat3(const std::string& name,  JVector3& out);
		bool GetValueAsFloat4(const std::string& name,  JVector4& out);
		bool GetValueAsMatrix(const std::string& name,  JMatrix& out);
	};


	class ReMaterialManager : public ReObject
	{
	public:
		ReMaterialManager(const std::string& name = "ReMaterialManager");
	public:
		static bool RegisterMaterial(const std::string& name, const ReMaterial& mat);
		static ReMaterial* GetMaterial(const std::string& name);
		static ReMaterialController* GetMaterialController(const std::string& mat_name);
		static bool DeleteMaterialController(ReMaterialController* mat_controller);
	private:
		std::unordered_map<std::string, std::shared_ptr<ReMaterial>> m_MaterialPool;
	};
}