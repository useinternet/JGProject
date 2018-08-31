#include"JGConstructHelper.h"
#include"../RenderSystem/JGTexture.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../EngineFrameWork/Components/StaticMesh2DComponent.h"
#include"../EngineStatics/JGLog.h"
using namespace std;

static JGConstructHelper* Instance = nullptr;

JGConstructHelper::JGConstructHelper()
{
	if (Instance == nullptr)
	{
		Instance = this;
	}
	else
	{
		JGLog::Write(ELogLevel::Warning, TT("JGConstructHelper has already been initialized."));
	}
}
JGConstructHelper* JGConstructHelper::GetInstance()
{
	return Instance;
}


JGConstructHelper::StaticMesh2D::StaticMesh2D(JGDeviceD* Device, JGBufferManager* BufferManager, const wstring& ComponentName,
	EPivot pivot, const wstring& TexturePath, const wstring& ShaderName)
{
	// 오브젝트 동적할당..
	unique_ptr<StaticMesh2DObject> TempObject = make_unique<StaticMesh2DObject>();
	TempObject->Texture = make_unique<JGTexture>();
	TempObject->Mesh = make_unique<JG2DMesh>();

	// 텍스쳐 로드
	bool result = TempObject->Texture->Add(Device, TexturePath);
	if (!result)
	{
		Success = false;
	}
	// 메쉬 로드
	result = TempObject->Mesh->Construct2DMesh(
		BufferManager, ComponentName,
		(float)TempObject->Texture->GetInformation(0).Width,
		(float)TempObject->Texture->GetInformation(0).Height, pivot);
	if (!result)
	{
		Success = false;
	}
	// 셰이더 이름 저장
	TempObject->ShaderName = ShaderName;

	// 포인터 저장
	Object = TempObject.get();

	// 주소 유지 배열에 저장
	JGConstructHelper::GetInstance()->m_vStaticMesh2DObject.push_back(move(TempObject));
}

