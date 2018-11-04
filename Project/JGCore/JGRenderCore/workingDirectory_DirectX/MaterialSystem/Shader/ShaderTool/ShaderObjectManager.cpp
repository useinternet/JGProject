#include"ShaderObjectManager.h"
using namespace JGRC;
using namespace std;
ShaderObjectManager* ShaderObjectManager::Instance  = nullptr;

ShaderObjectManager::ShaderObjectManager() {}
ShaderObjectManager::~ShaderObjectManager()
{
	for (auto& iter : m_vShaderObjects)
	{
		if (iter->Buffer)
		{
			switch (iter->Type)
			{
			case EShaderType::Vertex:
				((ID3D11VertexShader*)iter->Buffer)->Release();
				break;
			case EShaderType::Pixel:
				((ID3D11PixelShader*)iter->Buffer)->Release();
				break;
			default:
				break;
			}
		}
		iter->Buffer = nullptr;
		if (iter->InputLayout)
		{
			iter->InputLayout->Release();
			iter->InputLayout = nullptr;
		}
	}
}

ShaderObjectManager* ShaderObjectManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new ShaderObjectManager;
	}
	return Instance;
}
void ShaderObjectManager::Release()
{
	delete Instance;
	Instance = nullptr;
}
ShaderObject* ShaderObjectManager::CreateShaderObject(EShaderType type)
{
	std::unique_ptr<ShaderObject> object = make_unique<ShaderObject>();
	object->Type = type;
	ShaderObject* result = object.get();
	m_vShaderObjects.push_back(move(object));
	return result;
}