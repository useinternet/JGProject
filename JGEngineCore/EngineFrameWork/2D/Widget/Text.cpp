#include"Text.h"
#include"../../Components/TextComponent.h"

using namespace std;
Text::Text()
{
	RegisterComponentID(typeid(this));
	m_Text = make_unique<TextComponent>();
}

Text::~Text()
{
}
