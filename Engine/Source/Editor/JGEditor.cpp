#include "pch.h"
#include "JGEditor.h"
#include "Layer/DevLayer.h"
#include "Layer/ImGuiSystemLayer.h"
#include "Layer/UISystemLayer.h"
#include "Layer/GameLogicSystemLayer.h"
namespace JG
{
	void JGEditor::OpenImpl()
	{

	}

	void JGEditor::CloseImpl()
	{
	}
	void JGEditor::LoadImpl()
	{
		PushLayer(CreateSharedPtr<ImGuiSystemLayer>());
		PushLayer(CreateSharedPtr<GameLogicSystemLayer>());
		PushLayer(CreateSharedPtr<UISystemLayer>());
	}
}
