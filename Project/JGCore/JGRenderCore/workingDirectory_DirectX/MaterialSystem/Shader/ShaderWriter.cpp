#include"ShaderWriter.h"
using namespace std;
using namespace JGRC;

void ShaderWriter::AddEditor(HlslEditor* edit)
{
	m_Editor.push_back(edit);
}
bool ShaderWriter::Write(const std::string& Path)
{
	ofstream fout;
	fout.open(Path + ".shader");
	if (!fout.is_open())
	{
		JGLOG(log_Error, "JGRC::ShaderWriter", "Failed Write Shader" + Path)
		return false;
	}
	fout << "##Count : " << m_Editor.size() << endl;
	for (auto& edit : m_Editor)
	{
		edit->WriteData(fout);
	}
	return true;
}