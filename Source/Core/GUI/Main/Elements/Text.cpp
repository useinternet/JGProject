#include "pch.h"
#include "Text.h"
#include "Components/DrawComponent.h"

using namespace std;

namespace GUI
{



	void Text::Awake()
	{
		m_TMCom = CreateComponent<TextMeshComponent>(TT("TextMeshComponent"));
	}

	void Text::Start()
	{

	}

	void Text::Tick()
	{
	}

	void Text::Destroy()
	{
	}

}
