#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"Data/ShaderData.h"

namespace JGRC
{
	enum class EShaderType;
	class MaterialReader;
	class JGBuffer;
	class Mesh;
	class CORE_EXPORT Material
	{
	private:
		Mesh* m_Mesh = nullptr;
	private:
		friend MaterialReader;
	protected:
		typedef std::vector<std::shared_ptr<CBufferData>> CBufferDataArray;
		typedef std::vector<JGBuffer*> JGBufferArray;
		static class JGBufferManager* m_BfManager;
		static class DirectX*         m_DirectX;
	private:
		std::vector<class ShaderObject*>   m_vShaderObj;
		std::vector<std::string>           m_vHlslPaths;
		std::unique_ptr<InputLayoutData>   m_InputLayoutData;
		std::unique_ptr<SamplerStateData>  m_SamplerStateData;
		std::unique_ptr<TextureData>       m_TextureData;
		std::map<EShaderType, CBufferDataArray> m_mCBufferData;
		std::map<EShaderType, JGBufferArray> m_mJGBuffers;

		// 임시 //
		ID3D11SamplerState* m_Sampler = nullptr;
	public:
		Material();
		Material(Material&& m);
		virtual ~Material();
		void  SetMesh(Mesh* mesh);
		void  Render();
		real* GetParam(const std::string& paramName);
		void  SetParam(const std::string& paramName, void* Data);
		uint  GetParamCount(const std::string& paramName);
		// 임시 텍스쳐 경로 //
		void AddTexturePath(const std::string& TexturePath);
	private:
		ShaderObject* GetShaderObject(const uint idx);
		void AddHlslPath(const std::string& path);
		void AddShaderObject(ShaderObject* obj);
		void SetInputLayout(const InputLayoutData& data);
		void SetSamplerState(const SamplerStateData& data);
		void AddCBuffer(const CBufferData& data,const EShaderType type);
		void SetTexture(const TextureData& data);
	protected:
		bool ShaderCompile();
		bool MeshRendering();
		bool WriteConstantBuffer();
		bool InputShaderResource();
		bool InputShader_Sampler();
		bool Draw();
	};
}


