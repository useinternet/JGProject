#include"MaterialReader.h"
#include"Shader/ShaderTool/ShaderObjectManager.h"
using namespace JGRC;
using namespace std;

MaterialReader::MaterialReader()
{
	m_Stream = make_unique<fstream>();
}
MaterialReader::~MaterialReader()
{

}

ShaderObject* MaterialReader::Read(const string& materialPath)
{

	return nullptr;
}