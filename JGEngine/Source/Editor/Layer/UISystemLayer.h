#pragma once
#include "Class/SystemLayer.h"
#include "Class/FileIO.h"
namespace JG
{
	enum class MenuType
	{
		MainMenu,
		ContextMenu,
	};
	class UISystemLayer : public ISystemLayer
	{

	public:
		virtual ~UISystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;

		virtual String GetLayerName() override;
	private:
		void LoadUISettings(const String& fileName);
		void SaveUISettings(const String& fileName);
	private:
		EScheduleResult MenuUpdate();
		static void BeginMenu(const MenuItemNode* Node);
		static void EndMenu(const MenuItemNode* Node);
	};

}
