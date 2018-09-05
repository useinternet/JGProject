#pragma once
#include"JGConfigLoader.h"


enum class EKey;
class ENGINE_EXPORT JGInputConfigLoader : public JGConfigLoader
{
private:
	std::map<const std::string,const EKey> m_StringToEnums;


public:
	JGInputConfigLoader(const JGInputConfigLoader& copy) = delete;
	JGInputConfigLoader();
	~JGInputConfigLoader();

	virtual bool LoadConfig(const std::string& ConfigPath) override;

	void InitStringToEnum();

};