#pragma once
#include"GUI/EditorGUI/IGWindow.h"


namespace JE
{
	class FileViewer : public IGWindow
	{
	private:
	

	public:
		virtual void OnLoad() override;
		virtual void OnGUI() override;
		virtual void OnDestroy() override;

	};
}