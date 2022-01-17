#pragma once
#include "JGCore.h"
#include "UI/UIView.h"


namespace JG
{
	class RenderParamManager;
	class Renderer;
	class DevelopView : public UIView
	{
		JGCLASS

	private:
		bool	 mOpenGUI = true;

		

		struct RendererParamInfo
		{
			String Key;
			String VarName;
			Type   Type;
		};
		struct Param2Info
		{
			String Key;
			String Param1;
			String VarName;
			Type   Type;
		};
		struct Param3Info
		{
			String Key;
			String Param1;
			String Param2;
			String VarName;
			Type   Type;
		};

		struct EtcParamInfo
		{
			String Key;
			String VarName;
			Type   Type;
		};


		RenderParamManager* mRenderParamManager = nullptr;

		// External,
		StorableString mSelectedTextureParamKey = StorableString("DevelopView/SelectedTextureParamKey", "Final");

		Dictionary<String, RendererParamInfo>			    mRendererParmDic;
		Dictionary<String, Dictionary<String, Param2Info>>	mParam2Dic;
		Dictionary<String, Dictionary<String, Dictionary<String, Param3Info>>> mParam3Dic;
		Dictionary<String, EtcParamInfo>      mEtcParamDic;
		//
		StorableBool mIsRPHeaderOpen;
		StorableBool mIsEtcHeaderOpen;
		Dictionary<String, StorableBool> mIsP2HeaderOpen;
		Dictionary<String, StorableBool> mIsP3Header1Open;
		Dictionary<String, Dictionary<String, StorableBool>> mIsP3Header2Open;
	public:
		DevelopView();
		virtual ~DevelopView() = default;
	protected:
		virtual void Load() override;
		virtual void Initialize() override;
		virtual void OnGUI() override;
		virtual void Destroy() override;
		virtual void OnEvent(IEvent& e) override;
	private:
		void Statistics_OnGUI();
		void Dev_OnGUI();
		void RendererParams_OnGUI();
		void Param3Dic_OnGUI(const String& param1, const String& param2, const Dictionary<String, Param3Info>& dic);
		void Param_OnGUI(const String& key, const Type& type);

		template<class T>
		void ParamInfo_OnGUI(const String& key, const T& info)
		{
			ImGui::Text(info.VarName.c_str()); 
			ImGui::TableNextColumn();
			Param_OnGUI(key, info.Type);
			ImGui::TableNextColumn();
		}
	private:
		void InitRenderParamInfo(RenderParamManager* rpManager);
	};



}