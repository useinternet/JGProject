#include "pch.h"
#include "ContentsView.h"
#include "Common/DragAndDrop.h"
#include "ExternalImpl/JGImGui.h"
#include "ExternalImpl/ImFileDialog.h"
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
		// Copy Paste Delete Move 이정도만 일단 생성
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
		mIsCtrl = ImGui::IsKeyDown((i32)EKeyCode::Ctrl);

		ImGui::Begin("ContentsView", &mOpenGUI);
		// 기능 버튼

		OnGui_TopBar();

		ImGui::Separator();

		// 두개로 나누기 차일드 윈도우
		ImGui::Columns(2, nullptr, true);
		if (mIsColumnInit)
		{
			mIsColumnInit = false;
			f32 winWidth = ImGui::GetWindowWidth();
			ImGui::SetColumnWidth(0, winWidth * 0.25f);
		}
		ImGui::BeginChild("DirectoryPanel", ImVec2(0, 0), true);
		OnGui_ContentsDirectory();
		// ForEach 로 보여주는 디렉토리만 표시
		ImGui::EndChild();


		ImGui::NextColumn();
		ImGui::BeginChild("DetailPanel", ImVec2(0, 0), true);

		OnGui_ContentsDirectoryDetail();
		// ForEach로 보여주는 파일들만 표시
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
			bool isSelectedInfo = Vm->IsSelectedContentsFile(fileInfo);

			node->UserFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

			(isLeaf) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Leaf :
				node->UserFlags &= ~(ImGuiTreeNodeFlags_Leaf);
			(isSelectedInfo) ?
				node->UserFlags |= ImGuiTreeNodeFlags_Selected :
				node->UserFlags &= ~ImGuiTreeNodeFlags_Selected;

			if (isRoot == true)
			{
				isOpen = ImGui::CollapsingHeader((fileInfo->Name + "##ContentsDirectory").c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick);
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
			bool isItemClicked = ImGui::IsItemClicked();

			if (UIManager::GetInstance().ShowContextMenu(GetType()) == true)
			{
				if (node->IsSelected == false)
				{
					mVm->ReleaseSelectedContentsFiles();
				}
				node->IsTarget = true;
				isItemClicked  = true;
			}
			else
			{
				node->IsTarget = false;
			}


			if (isItemClicked)
			{
				if (mIsCtrl == false)
				{
					mVm->ReleaseSelectedContentsFiles();
				}
				node->IsSelected = true;
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
		Vm->ForEach([&](ContentsFileNode* fileNode)
		{
			auto fileInfo = fileNode->FileInfo;
			i32 ICON = GetIconID(fileInfo->FileFormat);
			ImTextureID tID = GetIconTextureID(ICON);
			
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
	void ContentsView::OnGui_TopBar()
	{
		OnGui_ImportDialog();
	}
	void ContentsView::OnGui_ImportDialog()
	{
		ImTextureID icon = 0;
		icon = GetIconTextureID(ICON_IMPORT);
		if (icon != 0)
		{
			if (ImGui::ImageButton(icon, ImVec2(20, 20)) == true)
			{
				ImGui::FileDialog::Instance().Open(GetType().GetName(), "Import Dialog", "", true, Application::GetInstance().GetAssetPath());
			}
		}
		if (ImGui::FileDialog::Instance().IsDone(GetType().GetName()))
		{
			if (ImGui::FileDialog::Instance().HasResult() == true)
			{
				// 현재 위치에서 임포트
				auto results = ImGui::FileDialog::Instance().GetResults();

			}
			ImGui::FileDialog::Instance().Close();
		}
	}
	void ContentsView::LoadIcons()
	{
		mIcons.resize(MAX_ICON);
		mIcons[ICON_NONE]      = UIManager::GetInstance().GetIcon("Text_Icon");
		mIcons[ICON_IMPORT]    = UIManager::GetInstance().GetIcon("Import_Icon");
		mIcons[ICON_DIRECTORY] = UIManager::GetInstance().GetIcon("Directory_Icon");
		mIcons[ICON_MATERIAL]  = UIManager::GetInstance().GetIcon("Material_Icon");
		mIcons[ICON_GAMEWORLD] = UIManager::GetInstance().GetIcon("GameWorld_Icon");
		mIcons[ICON_MESH]      = UIManager::GetInstance().GetIcon("Mesh_Icon");
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
	ImTextureID ContentsView::GetIconTextureID(i32 iconEnum)
	{
		if (iconEnum == -1) return 0;
		ImTextureID id = 0;
		if (mIcons[iconEnum] && mIcons[iconEnum]->Get() && mIcons[iconEnum]->Get()->IsValid())
		{
			id = (ImTextureID)JGImGui::GetInstance().ConvertImGuiTextureID(mIcons[iconEnum]->Get()->GetTextureID());
		}


		return id;
	}
}

