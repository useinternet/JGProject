#include"StringUtil.h"
#include<cstring>
#include<assert.h>
using namespace std;
bool StringUtil::FindString(const string& sentence, const char* str)
{
	uint count = 0;
	uint strLength = strlen(str);
	for (uint i = 0; i < sentence.length(); ++i)
	{
		if (sentence[i] == str[count])
		{
			count++;
		}
		else
		{
			count = 0;
		}
		if (count == strLength)
		{
			return true;
		}
	}
	return false;
}
void StringUtil::DelString(string& str, const char* delstr)
{
	bool bSearching = false;    // 현재 탐색중인지..
	uint len = str.length();
	uint dellen = strlen(delstr);
	uint count = 0;
	string result;                  // 탐색할때 임시 문자열
	string tmpStr;


	for (uint i = 0; i < len; ++i)
	{
		// 문자가 같으면 탐색 시작.
		if (str[i] == delstr[count])
		{
			bSearching = true;
			tmpStr += str[i];
			count++;
		}
		// 아니면 초기화
		else
		{
			if (bSearching && count < dellen)
			{
				result += tmpStr;

			}
			result += str[i];
			tmpStr = "";
			bSearching = false;
			count = 0;
		}
		// 탐색 중이었고 지울 문자가 아니었을때 임시로 저장하고있었던 문자열을 덧붙인다.
		// 그게 아니라면 그냥 추가 하지 않는다.

	}
	str = result;
}
void StringUtil::DelChar(std::string& str, const char c)
{
	for (auto iter = str.begin(); iter < str.end();)
	{
		if (*iter == c)
		{
			iter = str.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
void StringUtil::DelBlank(string& str)
{
	DelChar(str, ' ');
}
void StringUtil::Path_DelEndPath(std::string& str)
{
	string result;
	uint count = 0;  // 삭제할 단어의 수
	uint delPos = 0; // 삭제를 시작할 단어의 위치
    // 삭제할 위치를 탐색
	for (auto iter = str.end() - 1; iter >= str.begin(); --iter)
	{
		if (*iter == '/')
		{
			break;
		}
		count++;
	}
	// 삭제
	delPos = str.length() - count;
	for (auto iter = str.begin() + delPos; iter < str.end();)
	{
		iter = str.erase(iter);
	}
}