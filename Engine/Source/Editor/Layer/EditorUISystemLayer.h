#pragma once
#include "Class/SystemLayer.h"
#include "Class/FileIO.h"
#include "Event/EditorEvent.h"
#include "UI/UIManager.h"
namespace JG
{
	enum class MenuType
	{
		MainMenu,
		ContextMenu,
	};
	class EditorUISystemLayer : public ISystemLayer
	{

	public:
		virtual ~EditorUISystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	private:
		void SetMainMenu();
		void LoadUISettings(const String& fileName);
		void SaveUISettings(const String& fileName);
	private:
		void Init();
		EScheduleResult Update();
		EScheduleResult MenuUpdate();
		bool Resize(AppResizeEvent& e);
		void RegisterUIView();
		void RegisterPopupUIView();
		void RegisterInspectorUI();
	private:
		bool ResponseOpenProgressBar(RequestOpenProgressBarEvent& e);
		bool ResponseDisplayProgressBar(RequestDisplayProgressBarEvent& e);
		bool ResponseCloseProgressBar(RequestCloseProgressBarEvent& e);
		bool ResponseOpenMessageBox(RequestOpenMessageBoxEvent& e);
	private:
		static void BeginMenu(const MenuItemNode* Node);
		static void EndMenu(const MenuItemNode* Node);


	};

}
