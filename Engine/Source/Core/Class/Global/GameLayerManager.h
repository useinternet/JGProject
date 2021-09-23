#pragma once


#include "JGCore.h"

namespace JG
{
	class GameLayer
	{
		friend class GameLayerManager;
	public:
		constexpr static const char* DEFAULT_LAYER = "Default";
	public:
		static u64 GetMask(const String& layerName);
	private:
		String Name;
	};

	class GameLayerManager : public GlobalSingleton<GameLayerManager>
	{
		friend class Application;
	private:
		List<UniquePtr<GameLayer>>     mGameLayerList;
		Dictionary<String, GameLayer*> mGameLayerByName;
		Dictionary<String, u64> mGameLayerMask;
	public:
		GameLayerManager();
	public:
		void RegisterGameLayer(const String& layerName);
		void UnRegisterGameLayer(const String& layerName);
		u64 GetMask(const String& layerName) const;
		bool IsRegisterLayer(const String& layerName);

		void ForEach(const std::function<void(const String&)>& action);
	private:
		void MakeLayerMask();
	};


}