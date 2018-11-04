#include"Material.h"
#include"MaterialReader.h"
#include"DirectX/DirectX.h"
#include"Shader/ShaderTool/ShaderObject.h"
#include"Shader/ShaderTool/ShaderCompiler.h"
#include"BufferSystem/JGBuffer.h"
#include"BufferSystem/JGBufferManager.h"
#include"Texture/TextureManager.h"
//임시 인크루드
#include"TestClass/TestModel.h"


//
using namespace JGRC;
using namespace std;
JGBufferManager* Material::m_BfManager = nullptr;
DirectX*         Material::m_DirectX   = nullptr;

Material::Material()
{
	m_DirectX = DirectX::GetInstance();
	m_BfManager = JGBufferManager::GetInstance();
	m_InputLayoutData  = make_unique<InputLayoutData>();
	m_SamplerStateData = make_unique<SamplerStateData>();
	m_TextureData      = make_unique<TextureData>();

	m_Model = make_unique<TestModel>();

}
Material::Material(Material&& m)
{
	for (auto& iter : m.m_vShaderObj)
	{
		m_vShaderObj.push_back(move(iter));
	}
	m_InputLayoutData = move(m.m_InputLayoutData);
	m_SamplerStateData = move(m.m_SamplerStateData);
	m_TextureData = move(m.m_TextureData);
	m_mCBufferData = m.m_mCBufferData;
}
Material::~Material()
{
	for (auto& iter : m_mJGBuffers)
	{
		for (auto& buffer : iter.second)
		{
			m_BfManager->DeleteBuffer(buffer);
		}
	}
	m_Model->Shutdown();

	//  임시 //
	if (m_Sampler)
	{
		m_Sampler->Release();
		m_Sampler = nullptr;
	}
}
void  Material::LoadModel(const char* modelPath,bool bump)
{
	m_Model->Initialize(m_DirectX->GetDevice(), modelPath, bump);
}
void  Material::Render()
{
	m_Model->Render(m_DirectX->GetContext());
	
	// 상수 버퍼 작성
	for (auto& type : m_mJGBuffers)
	{
		UINT count = 0;
		for (auto& buffer : type.second)
		{
			buffer->Write(EMapType::Write_Discard, m_mCBufferData[type.first][count]->getData());
			switch (type.first)
			{
			case EShaderType::Pixel:
				m_DirectX->GetContext()->PSSetConstantBuffers(count++, 1, buffer->GetAddress());
				break;
			case EShaderType::Vertex:
				m_DirectX->GetContext()->VSSetConstantBuffers(count++, 1, buffer->GetAddress());
				break;
			default:
				break;
			}
		}
	}
	// 임시
	if (m_TextureData.get())
	{
		uint count = 0;
		for (auto& texture : m_TextureData->vTexture)
		{
			m_DirectX->GetContext()->PSSetShaderResources(count++, 1, texture->GetAddress(0));
		}
	}
	// 임시

	// 셰이더 입력
	for (auto obj : m_vShaderObj)
	{
		switch (obj->Type)
		{
		case EShaderType::Pixel:
			obj->Buffer ?
				m_DirectX->GetContext()->PSSetShader((ID3D11PixelShader*)(obj->Buffer), nullptr, 0) :
				JGLOG(log_Error, "JGRC::Material", " obj's Buffer is nullptr When PSSetShader");
			break;
		case EShaderType::Vertex:
			obj->InputLayout ?
				m_DirectX->GetContext()->IASetInputLayout(obj->InputLayout) :
				JGLOG(log_Error, "JGRC::Material", " obj's InputLayout is nullptr When IASetInputLayout");
			obj->Buffer ?
				m_DirectX->GetContext()->VSSetShader((ID3D11VertexShader*)(obj->Buffer), nullptr, 0) :
				JGLOG(log_Error, "JGRC::Material", " obj's Buffer is nullptr When PSSetShader");
			break;
		default:
			JGLOG(log_Warning, "JGRC::Material", "ShaderObjType is none or header type When Rendering");
			break;
		}
	}

	// 임시 //
	if (m_Sampler)
	{
		m_DirectX->GetContext()->PSSetSamplers(0, 1, &m_Sampler);
	}
	// 임시 //
	m_DirectX->GetContext()->DrawIndexed(m_Model->GetIndexCount(), 0, 0);
}
real* Material::GetParam(const std::string& paramName)
{
	for (auto& type : m_mCBufferData)
	{
		for (auto& cbf : type.second)
		{
			return cbf->getValue(paramName);
		}
	}
	return nullptr;
}
void  Material::SetParam(const std::string& paramName, void* Data)
{
	bool bfind = false;
	string cbufferName;
	for (auto& type : m_mCBufferData)
	{
		for (auto& cbf : type.second)
		{
			if (cbf->setValue(paramName, (real*)Data))
			{
				cbufferName = cbf->Name;
				bfind = true;
			}
		}
	}
	if (!bfind)
	{
		JGLOG(log_Error, "JGRC::Material", paramName + "is not exist in" + cbufferName);
	}
}
uint  Material::GetParamCount(const std::string& paramName)
{
	for (auto& type : m_mCBufferData)
	{
		for (auto& cbf : type.second)
		{
			return cbf->getCount(paramName);
		}
	}
	return 0;
}
ShaderObject* Material::GetShaderObject(const uint idx)
{
	return m_vShaderObj[idx];
}
void Material::AddHlslPath(const std::string& path)
{
	m_vHlslPaths.push_back(path);
}
void Material::AddShaderObject(ShaderObject* obj)
{
	CBufferDataArray arr;
	m_mCBufferData.insert(pair<EShaderType, CBufferDataArray>(obj->Type, arr));
	m_vShaderObj.push_back(obj);
}
void Material::SetInputLayout(const InputLayoutData& data)
{
	*m_InputLayoutData = data;
	for (uint i = 0; i < m_InputLayoutData->vDescs.size(); ++i)
	{
		m_InputLayoutData->vDescs[i].SemanticName = m_InputLayoutData->vSementicNames[i].c_str();
	}
}
void Material::SetSamplerState(const SamplerStateData& data)
{
	*m_SamplerStateData = data;
}
void Material::SetTexture(const TextureData& data)
{
	*m_TextureData = data;
}
void Material::AddCBuffer(const CBufferData& data, const EShaderType type)
{
	unique_ptr<CBufferData> buffer = make_unique<CBufferData>();
	*buffer = data;

	m_mCBufferData[type].push_back(move(buffer));
}
bool Material::ShaderCompile()
{
	bool result = true;
	InputLayoutData* LyData = nullptr;
	// 컴파일
	for (uint i = 0; i < m_vHlslPaths.size(); ++i)
	{
		
		switch (m_vShaderObj[i]->Type)
		{
		case EShaderType::Pixel:
			LyData = nullptr;
			break;
		case EShaderType::Vertex:
			LyData = m_InputLayoutData.get();
			break;
		default:
			JGLOG(log_Error, "JGRC::Material", m_vHlslPaths[i] + "// ShaderType is none!");
			return false;
		}
		wstring path;
		path.assign(m_vHlslPaths[i].begin(), m_vHlslPaths[i].end());
		ShaderCompiler compiler(path, m_vShaderObj[i], LyData);
		if (!compiler.Success)
		{
			JGLOG(log_Error, "JGRC::Material", m_vHlslPaths[i] + "// ShaderCompileFailed!");
			return false;
		}
	} 
	// 상수 버퍼 생성
	for (auto& iter : m_mCBufferData)
	{
		JGBufferArray arr;
		m_mJGBuffers.insert(pair<EShaderType, JGBufferArray>(iter.first, move(arr)));
		for (auto& buffer : iter.second)
		{
			JGBuffer* CBf = m_BfManager->CreateBuffer(EBufferType::ConstantBuffer,
				EUsageType::Dynamic, ECPUType::Access_Write, nullptr, buffer->Size, 1);
			m_mJGBuffers[iter.first].push_back(CBf);
		}
	}
	// 밑에 두개는 구현 하고 다시 컴파일 일단 여기까지..


	// 임시 //
	// 샘플러 상태 
	if (m_SamplerStateData.get())
	{
		for (auto& desc : m_SamplerStateData->vDescs)
		{
			m_DirectX->GetDevice()->CreateSamplerState(
				&desc, &m_Sampler);
		}
	}
	return true;
}
void Material::AddTexturePath(const string& TexturePath)
{
	
	JGTexture* tt = TextureManager::GetInstance()->CreateTexture();
	tt->AddTexture(TexturePath);

	if (m_TextureData.get())
	{
		m_TextureData->vTexture.push_back(tt);
	}

}