#include"JGConfigLoaderManager.h"
#include"JGInputConfigLoader.h"
#include"../JGSuperClass.h"
using namespace std;

bool JGConfigLoaderManager::LoadConfig(JGSuperClass* SuperClass)
{
	bool result = true;
	unique_ptr<JGInputConfigLoader> InputConfig = make_unique<JGInputConfigLoader>();
	InputConfig->InitConfigLoader(SuperClass);
	result = InputConfig->LoadConfig("../JGEngineCore/EngineStatics/Config/InputConfig.Json");
	if (!result)
	{
		return false;
	}
	return true;
}