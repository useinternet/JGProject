#include"ImageBox.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"
#include"../../../EngineStatics/JMath/JGMath.h"
#include"../../Object/Object.h"
using namespace std;

ImageBox::ImageBox()
{
	RegisterComponentID(typeid(this));
	m_Image = make_unique<StaticMesh2DComponent>();
}

ImageBox::~ImageBox()
{

}
bool ImageBox::CreateImage(const std::wstring & ImagePath, const EPivot pivot)
{
	AddChild(m_Image.get());
	m_Image->RegisterName(GetComponentName());

	JGConstructHelper::StaticMesh2D ImageMesh(
		GetDevice(), GetBufferManager(), m_Image->GetComponentName(),
		pivot, ImagePath);
	if (ImageMesh.Success)
	{
		m_Image->SetConstructObject(ImageMesh.Object);
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