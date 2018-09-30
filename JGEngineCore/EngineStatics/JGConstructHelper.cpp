#include"JGConstructHelper.h"
#include"../RenderSystem/JGTexture.h"
#include"../EngineFrameWork/Components/StaticMesh2DComponent.h"
#include"../EngineStatics/JGLog.h"
#include"../SoundSystem/JGSound.h"
#include"../EngineFrameWork/2D/Text/JGFontLoader.h"
#include"JMath/JGVector2D.h"
using namespace std;
JGDeviceD* JGConstructHelper::m_pDevice = nullptr;
JGBufferManager* JGConstructHelper::m_BufferManager = nullptr;
static JGConstructHelper* Instance = nullptr;
ConstructObject::~ConstructObject() {}
StaticMesh2DObject::~StaticMesh2DObject() {}
AnimationMesh2DObject::~AnimationMesh2DObject() {}
TextObject::~TextObject() {}
SoundObject::~SoundObject() {}



JGConstructHelper::JGConstructHelper(JGDeviceD* Device, JGBufferManager* BufferManager)
{
	if (Instance == nullptr)
	{
		Instance = this;
		m_pDevice = Device;
		m_BufferManager = BufferManager;
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


JGConstructHelper::StaticMesh2D::StaticMesh2D(const wstring& ComponentName,
	EPivot pivot, const wstring& TexturePath, EReverse Reverse, const EJGUsageType usageType,const wstring& ShaderName)
{
	// 오브젝트 동적할당..
	unique_ptr<StaticMesh2DObject> TempObject = make_unique<StaticMesh2DObject>();
	TempObject->Texture = make_unique<JGTexture>();
	TempObject->Mesh = make_unique<JG2DMesh>();
	TempObject->Pivot = make_unique<EPivot>();

	// 텍스쳐 로드
	bool result = TempObject->Texture->Add(JGConstructHelper::m_pDevice, TexturePath);
	if (!result)
	{
		Success = false;
	}

	JG2DMesh::STexInformaton Information;
	float TexScaleX = 1.0f;
	float TexScaleY = 1.0f;
	switch (Reverse)
	{
	case EReverse::Default:
		TexScaleX = 1.0f;
		TexScaleY = 1.0f;
		break;
	case EReverse::RL:
		TexScaleX = -1.0f;
		TexScaleY = 1.0f;
		break;
	case EReverse::UD:
		TexScaleX = 1.0f;
		TexScaleY = -1.0f;
		break;
	}
	Information.Width  = TexScaleX;
	Information.Height = TexScaleY;
	result = TempObject->Mesh->Construct2DMesh(
		JGConstructHelper::m_BufferManager, ComponentName,
		(float)TempObject->Texture->GetInformation(0).Width,
		(float)TempObject->Texture->GetInformation(0).Height, pivot,Information,
		usageType);
	if (!result)
	{
		Success = false;
	}
	// 셰이더 이름 저장
	TempObject->ShaderName = ShaderName;
	(*TempObject->Pivot) = pivot;
	// 포인터 저장
	Object = TempObject.get();

	// 주소 유지 배열에 저장
	JGConstructHelper::GetInstance()->m_vConstructObject.push_back(move(TempObject));
}


JGConstructHelper::AnimationMesh2D::AnimationMesh2D(
	const std::wstring& ComponentName, EPivot pivot, const size_t TotalFrame, const size_t WidthFrame, const size_t HeightFrame,
	const std::wstring& TexturePath, EReverse Reverse, const EJGUsageType usageType,const std::wstring& ShaderName)
{
	unique_ptr<AnimationMesh2DObject> TempObject = make_unique<AnimationMesh2DObject>();
	TempObject->Texture = make_unique<JGTexture>();
	TempObject->Mesh = make_unique<JG2DMesh>();
	TempObject->Pivot = make_unique<EPivot>();
	// 텍스쳐 로드
	bool result = TempObject->Texture->Add(JGConstructHelper::m_pDevice, TexturePath);
	if (!result)
	{
		Success = false;
	}
	float MeshWidth = 0.0f;
	float MeshHeight = 0.0f;
	float TexWidth = 0.0f;
	float TexHeight = 0.0f;
	if (Success)
	{
		// 애니메이션 한장의 크기를 구한다.
		MeshWidth = (float)TempObject->Texture->GetInformation(0).Width / WidthFrame;
		MeshHeight = (float)TempObject->Texture->GetInformation(0).Height / HeightFrame;
		TexWidth = 1.0f / (float)WidthFrame;
		TexHeight = 1.0f / (float)HeightFrame;
	}
	JG2DMesh::STexInformaton Information;
	float TexScaleX = 1.0f;
	float TexScaleY = 1.0f;
	switch (Reverse)
	{
	case EReverse::Default:
		TexScaleX = 1.0f;
		TexScaleY = 1.0f;
		break;
	case EReverse::RL:
		TexScaleX = -1.0f;
		TexScaleY = 1.0f;
		break;
	case EReverse::UD:
		TexScaleX = 1.0f;
		TexScaleY = -1.0f;
		break;
	}
	Information.Height = TexHeight * TexScaleY;
	Information.Width = TexWidth * TexScaleX;

	// 한장의 크기를 구한 구격으로 메쉬를 생성
	result = TempObject->Mesh->Construct2DMesh(JGConstructHelper::m_BufferManager, ComponentName,
		MeshWidth, MeshHeight, pivot, Information, usageType);
	if (!result)
	{
		Success = false;
	}
	// 각 정보들 저장
	TempObject->ShaderName = ShaderName;
	TempObject->TotalFrame = TotalFrame;
	TempObject->WidthFrame = WidthFrame;
	TempObject->HeightFrame = HeightFrame;
	TempObject->IncreaseWidth = TexWidth;
	TempObject->IncreaseHeight = TexHeight;
	TempObject->ReverseType = Reverse;
	(*TempObject->Pivot) = pivot;
	Object = TempObject.get();


	JGConstructHelper::GetInstance()->m_vConstructObject.push_back(move(TempObject));
}
JGConstructHelper::TextFont::TextFont(const std::string& FontPath, const std::wstring& FontTexturePath,
	const wstring& ShaderName)
{
	unique_ptr<TextObject> TempObject = make_unique<TextObject>();
	TempObject->Texture = make_unique<JGTexture>();
	// 폰트 텍스쳐 저장
	bool result = TempObject->Texture->Add(m_pDevice, FontTexturePath);
	if (!result)
	{
		Success = false;
	}
	// 폰트 생성
	result = JGFontLoader::GetInstance()->LoadFont(JGConstructHelper::m_pDevice, FontPath, FontTexturePath);
	if (!result)
	{
		Success = false;
	}
	// 셰이더 이름 저장.
	TempObject->ShaderName = ShaderName;
	TempObject->FontPath = FontPath;
	// 포인터 저장
	Object = TempObject.get();
	JGConstructHelper::GetInstance()->m_vConstructObject.push_back(move(TempObject));
}


