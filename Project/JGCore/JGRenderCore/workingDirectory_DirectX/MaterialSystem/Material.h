#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"Data/ShaderData.h"

namespace JGRC
{
	enum class EShaderType;
	class MaterialReader;
	class JGBuffer;
	class CORE_EXPORT Material
	{
	private:
		// 임시 모델..
		std::unique_ptr<class TestModel> m_Model;
	private:
		friend MaterialReader;
	private:
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
		~Material();
		void  LoadModel(const char* modelPath,bool bump);
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
		void SetTexture(const TextureData& data);
		void AddCBuffer(const CBufferData& data,const EShaderType type);
	private:
		bool ShaderCompile();
	};
}


