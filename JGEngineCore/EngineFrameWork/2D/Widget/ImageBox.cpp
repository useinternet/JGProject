#include"ImageBox.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"
#include"../../../EngineStatics/JMath/JGMath.h"
#include"../../Object/Object.h"
using namespace std;
map<wstring, ConstructObject*> ImageBox::m_mImagePath;
ImageBox::ImageBox()
{
	RegisterComponentID(typeid(this));

}

ImageBox::~ImageBox()
{

}
void ImageBox::BeginComponent(World* world)
{
	Widget::BeginComponent(world);

	m_Image->SetOwnerObject(GetOwnerObject());

}
void ImageBox::Tick(const float DeltaTime)
{
	Widget::Tick(DeltaTime);

	if (IsChangeScale())
	{
		m_Image->SetComponentScale(GetComponentScale());
	}




}
bool ImageBox::CreateImage(const std::wstring& ImagePath, const EPivot pivot)
{

	m_Image = make_unique<StaticMesh2DComponent>();
	
	AddChild(m_Image.get());
	m_Image->RegisterName(GetComponentName());
	m_Image->UseDefaultViewMatrix();


	auto iter = m_mImagePath.find(ImagePath);
	if (iter != m_mImagePath.end())
	{
		m_Image->SetConstructObject(m_mImagePath[ImagePath]);
		return true;
	}
	/// 리소스 오브젝트 생성..
	JGConstructHelper::StaticMesh2D ImageMesh(
		m_Image->GetComponentName(),
		pivot, ImagePath);
	if (ImageMesh.Success)
	{
		m_Image->SetConstructObject(ImageMesh.Object);
		m_mImagePath.insert(pair<wstring, ConstructObject*>(ImagePath, ImageMesh.Object));
	}
	return true;
}
void ImageBox::SetBlend(const float a)
{
	m_BlendAlpha = a;
	JGMath::DrawRegion(m_BlendAlpha, 0.0f, 1.0f);
	m_Image->SetAlphaBlend(m_BlendAlpha);
}
void ImageBox::AddBlend(const float a)
{
	m_BlendAlpha += a;
	JGMath::DrawRegion(m_BlendAlpha, 0.0f, 1.0f);
	m_Image->SetAlphaBlend(m_BlendAlpha);
}

void ImageBox::ReSizeImage(const float width, const float height)
{
	float originWidth = m_Image->GetTextureWdith();
	float originHeight = m_Image->GetTextureHeight();

	m_Image->SetComponentScale(width / originWidth, height / originHeight);
}
