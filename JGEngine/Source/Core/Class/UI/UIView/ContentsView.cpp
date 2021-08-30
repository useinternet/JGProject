#include "pch.h"
#include "ContentsView.h"
#include "Common/DragAndDrop.h"
#include "ExternalImpl/JGImGui.h"
namespace JG
{
	ContentsView::ContentsView()
	{
		UIManager::GetInstance().RegisterMainMenuItem("Windows/ContentsView", 0, [&]()
		{
			Open();
		}, nullptr);



	}
	void ContentsView::Load()
	{
		// Copy Paste Delete Move �������� �ϴ� ����
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Folder", 0, [&]() {
			if (mVm != nullptr)
			{
				mVm->Create_Folder->Execute();
			}
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Material/Surface", 0, [&]() {
			if (mVm != nullptr)
			{
				mVm->Create_Material_Surface->Execute();
			}
		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Create/Material/Lighting", 0, [&]() {

		}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Copy", 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Paste", 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Move", 20, [&]() {}, nullptr);
		UIManager::GetInstance().RegisterContextMenuItem(GetType(), "Delete", 20, [&]() 
		{
			if (mVm != nullptr)
			{
				mVm->Delete->Execute();
			}
		}, nullptr);
		LoadIcons();
	}
	void ContentsView::Initialize()
	{
		mIsColumnInit = true;
		mVm = GetViewModel();
	}
	void ContentsView::OnGUI()
	{
		ImGui::Begin("ContentsView", &mOpenGUI);
		// ��� ��ư

		ImGui::Button("Button01"); ImGui::SameLine(); ImGui::Button("Button02");
		ImGui::Separator();

		// �ΰ��� ������ ���ϵ� ������
		ImGui::Columns(2, nullptr, true);
		if (mIsColumnInit)
		{
			mIsColumnInit = false;
			f32 winWidth = ImGui::GetWindowWidth();
			ImGui::SetColumnWidth(0, winWidth * 0.25f);
		}
		ImGui::BeginChild("DirectoryPanel", ImVec2(0, 0), true);
		OnGui_ContentsDirectory();
		// ForEach �� �����ִ� ���丮�� ǥ��
		ImGui::EndChild();


		ImGui::NextColumn();
		ImGui::BeginChild("DetailPanel", ImVec2(0, 0), true);

		OnGui_ContentsDirectoryDetail();
		// ForEach�� �����ִ� ���ϵ鸸 ǥ��
		ImGui::EndChild();
		


		ImGui::End();

		if (mOpenGUI == false)
		{
			mOpenGUI = true;
			Close();
		}
	}
	void ContentsView::Destroy()
	{
	}
	void ContentsView::OnGui_ContentsDirectory()
	{
		auto Vm = GetViewModel();
		if (Vm == nullptr)
		{
			return;
		}

		Vm->ForEeach(
			[&](ContentsDirectoryNode* node) -> bool
		{
			bool isOpen = false;
			auto fileInfo = Vm->GetContentsFileInfo(node->Path);
			if (fileInfo == nullptr)
			{
				return false;
			}
			bool isRoot = fileInfo->OwnerDirectory == nullptr;
			bool isLeaf = fileInfo->DirectoryList.size() == 0;
			bool isSelectedInfo = Vm->IsSelectedContentsDirectory(fileInfo);

			node->UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Leaf :
				node->UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);
			(isSelectedInfo) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Selected :
				node->UserFlags &= ~ImGuiTreeNodeFlags_Selected;

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader((fileInfo->Name + "##ContentsDirectory").c_str());
				node->IsTreePop = false;
			}
			else
			{
				isOpen = ImGui::TreeNodeEx((void*)node, node->UserFlags, fileInfo->Name.c_str());
				node->IsIgnoreSelect = (isOpen != node->IsTreePop);
				node->IsTreePop = isOpen;
			}

			return isOpen;
		},
			[&](ContentsDirectoryNode* node)
		{
			static bool isContextOpen = false;
			ImGui::PushID(node);
			node->IsSelected = ImGui::IsItemClicked();
			if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
			{
				node->IsTarget = true;
			}
			else
			{
				node->IsTarget = false;
			}


			ImGui::PopID();
		},
			[&](ContentsDirectoryNode* node)
		{
			ImGui::TreePop();
		});
	}
	void ContentsView::OnGui_ContentsDirectoryDetail()
	{
		auto Vm = GetViewModel();
		if (Vm == nullptr)
		{
			return;
		}
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
		Vm->ForEach([&](ContentsFileInfo* fileInfo)
		{

			ImTextureID tID = 0;
			i32 ICON = GetIconID(fileInfo->FileFormat);

			if (ICON != -1)
			{
				if (mIcons[ICON] && mIcons[ICON]->Get() && mIcons[ICON]->Get()->IsValid())
				{
					tID = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[ICON]->Get()->GetTextureID());
				}
			}
			
			if (tID != 0)
			{
				ImGui::Image(tID, ImVec2(20, 20));
			}
			else {
				ImGui::Dummy(ImVec2(20, 20));
			}
			ImGui::SameLine();

			if (ImGui::Selectable(fileInfo->Name.c_str(), false, ImGuiSelectableFlags_None, ImVec2(0, 20.0f)) == true)
			{
				Vm->SelectedAssetFile(fileInfo->Path);
			}
			DragAndDropSource<DDDContentsFile>([&](DDDContentsFile* ddd)
			{
				ddd->FilePath = fileInfo->Path;
				ImGui::TextUnformatted(fileInfo->Name.c_str());
			});
		});
		ImGui::PopStyleVar();
	}
	void ContentsView::LoadIcons()
	{
		mIcons.resize(MAX_ICON);
		mIcons[ICON_NONE]      = UIManager::GetInstance().GetIcon("Text_Icon");
		mIcons[ICON_DIRECTORY] = UIManager::GetInstance().GetIcon("Directory_Icon");
		mIcons[ICON_MATERIAL]  = UIManager::GetInstance().GetIcon("Material_Icon");
		mIcons[ICON_GAMEWORLD] = UIManager::GetInstance().GetIcon("GameWorld_Icon");
		mIcons[ICON_MESH] = UIManager::GetInstance().GetIcon("Mesh_Icon");
		mIcons[ICON_TEXTURE]   = UIManager::GetInstance().GetIcon("Texture_Icon");
	}
	i32 ContentsView::GetIconID(EAssetFormat format)
	{
		switch (format)
		{
		case JG::EAssetFormat::Directory: return ICON_DIRECTORY;
		case JG::EAssetFormat::Material:  return ICON_MATERIAL;
		case JG::EAssetFormat::Texture:   return ICON_TEXTURE;
		case JG::EAssetFormat::GameWorld: return ICON_GAMEWORLD;
		case JG::EAssetFormat::Mesh:   return ICON_MESH;
		case JG::EAssetFormat::Skeletal:
		default:
			return ICON_NONE;
		}
		return -1;
	}
}

