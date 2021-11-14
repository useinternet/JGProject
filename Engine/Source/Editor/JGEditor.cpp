#include "pch.h"
#include "JGEditor.h"
#include "Layer/EditorUISystemLayer.h"
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
		PushLayer(CreateSharedPtr<EditorUISystemLayer>());
		PushLayer(CreateSharedPtr<GameLogicSystemLayer>());
	}
}
