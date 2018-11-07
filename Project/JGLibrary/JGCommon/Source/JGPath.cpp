#include"JGPath.h"
using namespace JGPath;
using namespace std;

void Path::SetPath(const string& path)
{
	m_Path = path;
	m_Pathw.assign(m_Path.begin(), m_Path.end());
}
Path::Path(const std::string& path)
{
	SetPath(path);
}
std::string Path::operator/(const std::string& path)
{
	return m_Path + "/" + path;
}
std::wstring Path::operator/(const std::wstring& path)
{
	return m_Pathw + L"/" + path;
}