#include<PCH.h>
#include"DirectXCommon.h"
#include"Dx12Exception.h"

using namespace Common;


DxException::DxException(long hr, const std::wstring& functionName, const std::string& filename, int lineNumber)
:ErrorCode(hr), FunctionName(Util::AnsiToString(functionName)), Filename(filename), LineNumber(lineNumber)
{
	DX12_LOG(Common::Log::Critical, ToString().c_str());
}