#include"JGConfigLoader.h"
#include"../JGSuperClass.h"
using namespace std;

JGConfigLoader::JGConfigLoader()
{

}
JGConfigLoader::~JGConfigLoader()
{

}
void JGConfigLoader::InitConfigLoader(JGSuperClass* SuperClass)
{
	m_pSuperClass = SuperClass;
}
JGCommandManager* JGConfigLoader::GetCommandManager()
{
	return m_pSuperClass->GetCommandManager();
}
