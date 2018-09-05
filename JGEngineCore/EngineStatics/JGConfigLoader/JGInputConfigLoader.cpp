#include"JGInputConfigLoader.h"
#include"../../InputSystem/JGCommandManager.h"
#include"../../InputSystem/JGPressManager.h"
#include"../JGLog.h"
#include"../JGSuperClass.h"
using namespace std;
JGInputConfigLoader::JGInputConfigLoader()
{
	InitStringToEnum();
}
JGInputConfigLoader::~JGInputConfigLoader()
{

}
bool JGInputConfigLoader::LoadConfig(const string& ConfigPath)
{
	ifstream fstream;
	fstream.open(ConfigPath);
	if (!fstream.is_open())
	{
		JGLog::Write(ELogLevel::Error, TT("%s LoadConfig Failed."), ConfigPath);
		return false;
	}

	Json::Value Root;
	fstream >> Root;


	Json::Value CommandList = Root["CommandList"];
	Json::ValueIterator Start = CommandList.begin();

	while (Start != CommandList.end())
	{
		Json::Value JCommandName = (*Start)["CommandName"];
		std::string String = JCommandName.asString();
		std::wstring CommandName = TT("");
		// string -> wstring으로 변환..
		CommandName.assign(String.begin(), String.end());

		// 키 값 받아오기..
		Json::Value JKey = (*Start)["BindKey"];
		EKey BindKey = m_StringToEnums[JKey.asString()];


		// 키 등록
		GetCommandManager()->RegisterKeyCommand(BindKey, CommandName);
		++Start;
	}


	return true;
}


void JGInputConfigLoader::InitStringToEnum()
{
	m_StringToEnums.insert(pair<const string, const EKey>("_0", EKey::_0));
	m_StringToEnums.insert(pair<const string, const EKey>("_1", EKey::_1));
	m_StringToEnums.insert(pair<const string, const EKey>("_2", EKey::_2));
	m_StringToEnums.insert(pair<const string, const EKey>("_3", EKey::_3));
	m_StringToEnums.insert(pair<const string, const EKey>("_4", EKey::_4));
	m_StringToEnums.insert(pair<const string, const EKey>("_5", EKey::_5));
	m_StringToEnums.insert(pair<const string, const EKey>("_6", EKey::_6));
	m_StringToEnums.insert(pair<const string, const EKey>("_7", EKey::_7));
	m_StringToEnums.insert(pair<const string, const EKey>("_8", EKey::_8));
	m_StringToEnums.insert(pair<const string, const EKey>("_9", EKey::_9));
	m_StringToEnums.insert(pair<const string, const EKey>("F1", EKey::F1));
	m_StringToEnums.insert(pair<const string, const EKey>("F2", EKey::F2));
	m_StringToEnums.insert(pair<const string, const EKey>("F3", EKey::F3));
	m_StringToEnums.insert(pair<const string, const EKey>("F4", EKey::F4));
	m_StringToEnums.insert(pair<const string, const EKey>("F5", EKey::F5));
	m_StringToEnums.insert(pair<const string, const EKey>("F6", EKey::F6));
	m_StringToEnums.insert(pair<const string, const EKey>("F7", EKey::F7));
	m_StringToEnums.insert(pair<const string, const EKey>("F8", EKey::F8));
	m_StringToEnums.insert(pair<const string, const EKey>("F9", EKey::F9));
	m_StringToEnums.insert(pair<const string, const EKey>("F10", EKey::F10));
	m_StringToEnums.insert(pair<const string, const EKey>("F11", EKey::F11));
	m_StringToEnums.insert(pair<const string, const EKey>("F12", EKey::F12));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad0", EKey::NumPad0));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad1", EKey::NumPad1));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad2", EKey::NumPad2));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad3", EKey::NumPad3));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad4", EKey::NumPad4));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad5", EKey::NumPad5));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad6", EKey::NumPad6));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad7", EKey::NumPad7));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad8", EKey::NumPad8));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPad9", EKey::NumPad9));
	m_StringToEnums.insert(pair<const string, const EKey>("NumLock", EKey::NumLock));
	m_StringToEnums.insert(pair<const string, const EKey>("NumPadEnter", EKey::NumPadEnter));
	m_StringToEnums.insert(pair<const string, const EKey>("Escape", EKey::Escape));
	m_StringToEnums.insert(pair<const string, const EKey>("Delete", EKey::Delete));
	m_StringToEnums.insert(pair<const string, const EKey>("End", EKey::End));
	m_StringToEnums.insert(pair<const string, const EKey>("Home", EKey::Home));
	m_StringToEnums.insert(pair<const string, const EKey>("Insert", EKey::Insert));
	m_StringToEnums.insert(pair<const string, const EKey>("PgDn", EKey::PgDn));
	m_StringToEnums.insert(pair<const string, const EKey>("PgUp", EKey::PgUp));
	m_StringToEnums.insert(pair<const string, const EKey>("Shift", EKey::Shift));
	m_StringToEnums.insert(pair<const string, const EKey>("Ctrl", EKey::Ctrl));
	m_StringToEnums.insert(pair<const string, const EKey>("Alt", EKey::Alt));
	m_StringToEnums.insert(pair<const string, const EKey>("CapsLock", EKey::CapsLock));
	m_StringToEnums.insert(pair<const string, const EKey>("Tab", EKey::Tab));
	m_StringToEnums.insert(pair<const string, const EKey>("A", EKey::A));
	m_StringToEnums.insert(pair<const string, const EKey>("B", EKey::B));
	m_StringToEnums.insert(pair<const string, const EKey>("C", EKey::C));
	m_StringToEnums.insert(pair<const string, const EKey>("D", EKey::D));
	m_StringToEnums.insert(pair<const string, const EKey>("E", EKey::E));
	m_StringToEnums.insert(pair<const string, const EKey>("F", EKey::F));
	m_StringToEnums.insert(pair<const string, const EKey>("G", EKey::G));
	m_StringToEnums.insert(pair<const string, const EKey>("H", EKey::H));
	m_StringToEnums.insert(pair<const string, const EKey>("I", EKey::I));
	m_StringToEnums.insert(pair<const string, const EKey>("J", EKey::J));
	m_StringToEnums.insert(pair<const string, const EKey>("K", EKey::K));
	m_StringToEnums.insert(pair<const string, const EKey>("L", EKey::L));
	m_StringToEnums.insert(pair<const string, const EKey>("M", EKey::M));
	m_StringToEnums.insert(pair<const string, const EKey>("N", EKey::N));
	m_StringToEnums.insert(pair<const string, const EKey>("O", EKey::O));
	m_StringToEnums.insert(pair<const string, const EKey>("P", EKey::P));
	m_StringToEnums.insert(pair<const string, const EKey>("Q", EKey::Q));
	m_StringToEnums.insert(pair<const string, const EKey>("R", EKey::R));
	m_StringToEnums.insert(pair<const string, const EKey>("S", EKey::S));
	m_StringToEnums.insert(pair<const string, const EKey>("T", EKey::T));
	m_StringToEnums.insert(pair<const string, const EKey>("U", EKey::U));
	m_StringToEnums.insert(pair<const string, const EKey>("V", EKey::V));
	m_StringToEnums.insert(pair<const string, const EKey>("W", EKey::W));
	m_StringToEnums.insert(pair<const string, const EKey>("X", EKey::X));
	m_StringToEnums.insert(pair<const string, const EKey>("Y", EKey::Y));
	m_StringToEnums.insert(pair<const string, const EKey>("Z", EKey::Z));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseXAxis", EKey::MouseXAxis));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseYAxis", EKey::MouseYAxis));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseLButton", EKey::MouseLButton));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseRButton", EKey::MouseRButton));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseWheelClick", EKey::MouseWheelClick));
	m_StringToEnums.insert(pair<const string, const EKey>("MouseWheelAxis", EKey::MouseWheelAxis));
}