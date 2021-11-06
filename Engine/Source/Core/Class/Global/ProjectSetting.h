#pragma once
#include "JGCore.h"

namespace JG
{

	class ProjectSetting : public GlobalSingleton<ProjectSetting>
	{
		friend class Application;





		String mStartGameWorldPath;
		
		// Json Key
	public:
		ProjectSetting();
		virtual ~ProjectSetting();
	public:
		String GetStartGameWorldPath() const;

	public:
		void SetStartGameWorldPath(const String& path);
	public:
		void Save();

	private:
		void Load();
	private:
		void Save_StartSetting(SharedPtr<Json> json);
	private:
		void Load_StartSetting(SharedPtr<Json> json);
	};
}


