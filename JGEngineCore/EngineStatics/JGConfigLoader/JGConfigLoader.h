#pragma once
#include"../Engine.h"
#include"../JsonCpp/json.h"

class JGSuperClass;
class JGCommandManager;
class ENGINE_EXPORT JGConfigLoader
{

private:
	JGSuperClass* m_pSuperClass;

public:
	JGConfigLoader(const JGConfigLoader& copy) = delete;
	JGConfigLoader();
	~JGConfigLoader();
	void InitConfigLoader(JGSuperClass* SuperClass);
	virtual bool LoadConfig(const std::string& ConfigPath) = 0;

protected:
	JGCommandManager* GetCommandManager();
};
