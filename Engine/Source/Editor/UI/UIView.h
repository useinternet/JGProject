#pragma once
#include "JGCore.h"
#include "Event/EditorEvent.h"
#include "Event/GameEvent.h"
#include "ExternalImpl/ImGuiExternal.h"
#include "Class/Data/StorableVar.h"
namespace JG
{
	class IUIError
	{
	public:
		virtual u64    GetID()    const = 0;
		virtual String ToString() const = 0;
	};

	class IUIErrorReceiver
	{
	protected:
		virtual void ReceiveError(SharedPtr<IUIError> error) = 0;
	};


	class IUIView : public IJGObject, public IUIErrorReceiver
	{
		friend class UIManager;
	protected:
		virtual void Load()        = 0;
		virtual void Initialize()  = 0;
		virtual void OnGUI()       = 0;
		virtual void BeginOnGUI()  = 0;
		virtual void EndOnGUI()    = 0;
		virtual void Destroy()     = 0;
		
	protected:
		virtual void OnEvent(IEvent& e) = 0;
		virtual u64  GetViewID() const = 0;
		virtual void SetViewID(u64 id) = 0;
		virtual bool IsUniqueView() const = 0;
	public:
		virtual void Open()  = 0;
		virtual void Close() = 0;
		virtual bool IsOpen() const = 0;
	public:
		virtual ~IUIView() = default;
	};


	class UIView : public IUIView
	{
		JGCLASS
	private:
		String mTitleName;

		bool mIsOpen     = false;
		bool mIsLoad     = false;

		
		// GUI x 버튼 클릭 여부
		u64  mViewID    = 0;
		bool mIsUniqueView = true;
		bool mIsShowGUI    = true;
	private:
		virtual u64 GetViewID() const override
		{
			return mViewID;
		}
		virtual void SetViewID(u64 id) override
		{
			mViewID = id;
		}
		virtual bool IsUniqueView() const override
		{
			return mIsUniqueView;
		}
	protected:
		virtual void OnEvent(IEvent& e) override { }
		virtual void Load()		  override {}
		virtual void Initialize() override {}

		virtual void OnGUI()	  override {}
		virtual void PreOnGUI()   {}
		virtual void LateOnGUI()  {}
		virtual void Destroy()	  override {}
		virtual void ReceiveError(SharedPtr<IUIError> error) override {}

		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override {
			jsonData->AddMember("IsOpen", IsOpen());
		}
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override {
			auto val = jsonData->GetMember("IsOpen");
			bool open = false;
			if (val)
			{
				open = val->GetBool();
			}
			if (open) Open();
			else Close();

		}


		const String& GetTitleName()
		{
			if (mTitleName.empty() == true)
			{
				mTitleName = StringHelper::ReplaceAll(GetType().GetName(), "JG::", "");
			}
			return mTitleName;
		}
		void SetTitleName(const String& name)
		{
			mTitleName = name;
		}
		void DisableUniqueView()
		{
			mIsUniqueView = false;
		}
	public:
		virtual bool IsOpen() const override {
			return mIsOpen;
		}
		virtual void Open() override
		{
			if (mIsOpen == false)
			{
				mIsOpen = true;
				if (mIsLoad == false)
				{
					Load();
					mIsLoad = true;
				}
				Initialize();
			}
		}
		virtual void Close() override
		{
			if (mIsOpen == true)
			{
				mIsOpen = false;
				Destroy();
			}
		}
	private:
		virtual void BeginOnGUI() override
		{
			PreOnGUI();
			ImGui::Begin((GetTitleName() + "##" + std::to_string(GetViewID())).c_str(), &mIsShowGUI);
		}
		virtual void EndOnGUI() override
		{
			ImGui::End();
			if (mIsShowGUI == false)
			{
				mIsShowGUI = true;
				Close();
			}
			LateOnGUI();
		}



	public:
		virtual ~UIView() = default;
		template<class EventClass>
		void SendEvent(EventClass& e)
		{
			Application::GetInstance().SendEvent(e);
		}
	};


	enum class EPopupType
	{
		Context,
		Modal,
	};

	class IPopupUIView : IJGObject
	{
		friend class UIManager;
	protected:
		virtual bool OnGUI()      = 0;
		virtual void OnEvent(IEvent& e) = 0;
		virtual void Close() = 0;
		virtual EPopupType GetPopupType() = 0;
	protected:
		virtual bool IsOpen() const = 0;
	public:
		virtual ~IPopupUIView() = default;
	};

	template<class PopupInitData>
	class PopupUIView : public IPopupUIView
	{
		friend class UIManager;
		bool mIsOpen = false;
	protected:
		virtual bool OnGUI() override { return false; }
		virtual void OnEvent(IEvent& e) override {}
	protected:
		virtual void Initialize(const PopupInitData& data) = 0;
		virtual void Destroy() = 0;
	private:
		virtual void Open(const PopupInitData& data)
		{
			if (mIsOpen == false)
			{
				mIsOpen = true;
				Initialize(data);
			}
		}
	public:
		virtual void Close() override
		{
			if (mIsOpen == true)
			{
				mIsOpen = false;
				Destroy();
			}
		}

		virtual bool IsOpen() const
		{
			return mIsOpen;
		}
	public:
		virtual ~PopupUIView() = default;
	};


	template<class ContextInitData>
	class ContextUIView : public PopupUIView<ContextInitData>
	{
		JGCLASS
	public:
		virtual ~ContextUIView() = default;
	public:
		virtual EPopupType GetPopupType() { return EPopupType::Context; }
	};

	template<class ModalInitData>
	class ModalUIView : public PopupUIView<ModalInitData>
	{
		JGCLASS
	public:
		virtual ~ModalUIView() = default;
	public:
		virtual EPopupType GetPopupType() { return EPopupType::Modal; }
	};
}