#include"Text.h"
#include"../../Components/TextComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"
using namespace std;
Text::Text()
{
	RegisterComponentID(typeid(this));
}

Text::~Text()
{
}

bool Text::SetFont(const std::string & FontPath, const std::wstring & FontTexturePath)
{
	m_Text = make_unique<TextComponent>();
	m_Text->RegisterName(GetComponentName() + TT("_Text"));
	JGConstructHelper::TextFont FontMesh(
		FontPath, FontTexturePath);
	if (FontMesh.Success)
	{
		m_Text->SetConstructObject(FontMesh.Object);
	}
	AddChild(m_Text.get());

	return true;
}

TextComponent* Text::Get()
{
	return m_Text.get();
}
