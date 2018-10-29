#include"ShaderObjectManager.h"
using namespace JGRC;
using namespace std;
ShaderObjectManager* ShaderObjectManager::Instance  = nullptr;

ShaderObjectManager::ShaderObjectManager() {}
ShaderObjectManager::~ShaderObjectManager()
{
	for (auto& iter : m_vShaderObjects)
	{
	
		switch (iter->ShaderType)
		{
		case EShaderType::Vertex:
			iter->Layout->Release();
			((ID3D11VertexShader*)iter->ShaderBuffer)->Release();
			break;
		case EShaderType::Pixel:
			((ID3D11PixelShader*)iter->ShaderBuffer)->Release();
			break;
		default:
			break;
		}
		iter->ShaderBuffer = nullptr;
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
	object->ShaderType   = type;
	object->ShaderBuffer = nullptr;
	ShaderObject* result = object.get();
	m_vShaderObjects.push_back(move(object));
	return result;
}