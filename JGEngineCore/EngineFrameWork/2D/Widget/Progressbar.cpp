#include"Progressbar.h"
#include"../../Components/StaticMesh2DComponent.h"
Progressbar::Progressbar()
{
	RegisterComponentID(typeid(this));
}
Progressbar::~Progressbar()
{

}

void Progressbar::CreateProgressBar(const std::wstring& FullBarPath, const std::wstring& EmptyBarPath, std::wstring& ProgressingBarPath)
{
}

void Progressbar::SetBindingData(const float Aspect)
{
	m_Aspect = Aspect;
}
