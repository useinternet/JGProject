#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/Core/Input/Key.h"


PSharedPtr<JGEnum> Module_Core_Code_Generation_Static_Create_Enum_EKeyCode()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;
	HHashMap<int32, int32> EnumRedirectMap;

	MetaList.resize(99);
	EnumStringList.resize(99);

    EnumStringList[0] = "Unknown";
    EnumRedirectMap[0] = 0;


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[1] = "A";
    EnumRedirectMap[1] = 1;


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[2] = "B";
    EnumRedirectMap[2] = 2;


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[3] = "C";
    EnumRedirectMap[3] = 3;


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[4] = "D";
    EnumRedirectMap[4] = 4;


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[5] = "E";
    EnumRedirectMap[5] = 5;


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[6] = "F";
    EnumRedirectMap[6] = 6;


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[7] = "G";
    EnumRedirectMap[7] = 7;


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[8] = "H";
    EnumRedirectMap[8] = 8;


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[9] = "I";
    EnumRedirectMap[9] = 9;


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[10] = "J";
    EnumRedirectMap[10] = 10;


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[11] = "K";
    EnumRedirectMap[11] = 11;


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[12] = "L";
    EnumRedirectMap[12] = 12;


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[13] = "M";
    EnumRedirectMap[13] = 13;


	MetaList[13] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[14] = "N";
    EnumRedirectMap[14] = 14;


	MetaList[14] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[15] = "O";
    EnumRedirectMap[15] = 15;


	MetaList[15] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[16] = "P";
    EnumRedirectMap[16] = 16;


	MetaList[16] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[17] = "Q";
    EnumRedirectMap[17] = 17;


	MetaList[17] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[18] = "R";
    EnumRedirectMap[18] = 18;


	MetaList[18] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[19] = "S";
    EnumRedirectMap[19] = 19;


	MetaList[19] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[20] = "T";
    EnumRedirectMap[20] = 20;


	MetaList[20] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[21] = "U";
    EnumRedirectMap[21] = 21;


	MetaList[21] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[22] = "V";
    EnumRedirectMap[22] = 22;


	MetaList[22] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[23] = "W";
    EnumRedirectMap[23] = 23;


	MetaList[23] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[24] = "X";
    EnumRedirectMap[24] = 24;


	MetaList[24] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[25] = "Y";
    EnumRedirectMap[25] = 25;


	MetaList[25] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[26] = "Z";
    EnumRedirectMap[26] = 26;


	MetaList[26] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[27] = "Num0";
    EnumRedirectMap[27] = 27;


	MetaList[27] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[28] = "Num1";
    EnumRedirectMap[28] = 28;


	MetaList[28] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[29] = "Num2";
    EnumRedirectMap[29] = 29;


	MetaList[29] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[30] = "Num3";
    EnumRedirectMap[30] = 30;


	MetaList[30] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[31] = "Num4";
    EnumRedirectMap[31] = 31;


	MetaList[31] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[32] = "Num5";
    EnumRedirectMap[32] = 32;


	MetaList[32] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[33] = "Num6";
    EnumRedirectMap[33] = 33;


	MetaList[33] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[34] = "Num7";
    EnumRedirectMap[34] = 34;


	MetaList[34] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[35] = "Num8";
    EnumRedirectMap[35] = 35;


	MetaList[35] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[36] = "Num9";
    EnumRedirectMap[36] = 36;


	MetaList[36] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[37] = "NumPad0";
    EnumRedirectMap[37] = 37;


	MetaList[37] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[38] = "NumPad1";
    EnumRedirectMap[38] = 38;


	MetaList[38] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[39] = "NumPad2";
    EnumRedirectMap[39] = 39;


	MetaList[39] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[40] = "NumPad3";
    EnumRedirectMap[40] = 40;


	MetaList[40] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[41] = "NumPad4";
    EnumRedirectMap[41] = 41;


	MetaList[41] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[42] = "NumPad5";
    EnumRedirectMap[42] = 42;


	MetaList[42] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[43] = "NumPad6";
    EnumRedirectMap[43] = 43;


	MetaList[43] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[44] = "NumPad7";
    EnumRedirectMap[44] = 44;


	MetaList[44] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[45] = "NumPad8";
    EnumRedirectMap[45] = 45;


	MetaList[45] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[46] = "NumPad9";
    EnumRedirectMap[46] = 46;


	MetaList[46] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[47] = "NumPadAdd";
    EnumRedirectMap[47] = 47;


	MetaList[47] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[48] = "NumPadMultiply";
    EnumRedirectMap[48] = 48;


	MetaList[48] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[49] = "NumPadSubtract";
    EnumRedirectMap[49] = 49;


	MetaList[49] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[50] = "NumPadDemical";
    EnumRedirectMap[50] = 50;


	MetaList[50] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[51] = "NumPadDivide";
    EnumRedirectMap[51] = 51;


	MetaList[51] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[52] = "NumLock";
    EnumRedirectMap[52] = 52;


	MetaList[52] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[53] = "F0";
    EnumRedirectMap[53] = 53;


	MetaList[53] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[54] = "F1";
    EnumRedirectMap[54] = 54;


	MetaList[54] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[55] = "F2";
    EnumRedirectMap[55] = 55;


	MetaList[55] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[56] = "F3";
    EnumRedirectMap[56] = 56;


	MetaList[56] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[57] = "F4";
    EnumRedirectMap[57] = 57;


	MetaList[57] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[58] = "F5";
    EnumRedirectMap[58] = 58;


	MetaList[58] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[59] = "F6";
    EnumRedirectMap[59] = 59;


	MetaList[59] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[60] = "F7";
    EnumRedirectMap[60] = 60;


	MetaList[60] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[61] = "F8";
    EnumRedirectMap[61] = 61;


	MetaList[61] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[62] = "F9";
    EnumRedirectMap[62] = 62;


	MetaList[62] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[63] = "F10";
    EnumRedirectMap[63] = 63;


	MetaList[63] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[64] = "F11";
    EnumRedirectMap[64] = 64;


	MetaList[64] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[65] = "F12";
    EnumRedirectMap[65] = 65;


	MetaList[65] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[66] = "Left";
    EnumRedirectMap[66] = 66;


	MetaList[66] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[67] = "Right";
    EnumRedirectMap[67] = 67;


	MetaList[67] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[68] = "Up";
    EnumRedirectMap[68] = 68;


	MetaList[68] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[69] = "Down";
    EnumRedirectMap[69] = 69;


	MetaList[69] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[70] = "LeftBracket";
    EnumRedirectMap[70] = 70;


	MetaList[70] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[71] = "RightBracket";
    EnumRedirectMap[71] = 71;


	MetaList[71] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[72] = "Semicolon";
    EnumRedirectMap[72] = 72;


	MetaList[72] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[73] = "QuotationMark";
    EnumRedirectMap[73] = 73;


	MetaList[73] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[74] = "Comma";
    EnumRedirectMap[74] = 74;


	MetaList[74] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[75] = "Period";
    EnumRedirectMap[75] = 75;


	MetaList[75] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[76] = "Slash";
    EnumRedirectMap[76] = 76;


	MetaList[76] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[77] = "Del";
    EnumRedirectMap[77] = 77;


	MetaList[77] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[78] = "PgUp";
    EnumRedirectMap[78] = 78;


	MetaList[78] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[79] = "PgDown";
    EnumRedirectMap[79] = 79;


	MetaList[79] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[80] = "End";
    EnumRedirectMap[80] = 80;


	MetaList[80] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[81] = "Home";
    EnumRedirectMap[81] = 81;


	MetaList[81] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[82] = "Insert";
    EnumRedirectMap[82] = 82;


	MetaList[82] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[83] = "Esc";
    EnumRedirectMap[83] = 83;


	MetaList[83] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[84] = "GraveAccent";
    EnumRedirectMap[84] = 84;


	MetaList[84] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[85] = "Tab";
    EnumRedirectMap[85] = 85;


	MetaList[85] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[86] = "CapsLock";
    EnumRedirectMap[86] = 86;


	MetaList[86] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[87] = "Plus";
    EnumRedirectMap[87] = 87;


	MetaList[87] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[88] = "Minus";
    EnumRedirectMap[88] = 88;


	MetaList[88] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[89] = "BackSpace";
    EnumRedirectMap[89] = 89;


	MetaList[89] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[90] = "BackSlash";
    EnumRedirectMap[90] = 90;


	MetaList[90] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[91] = "Enter";
    EnumRedirectMap[91] = 91;


	MetaList[91] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[92] = "Pause";
    EnumRedirectMap[92] = 92;


	MetaList[92] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[93] = "ScrollLock";
    EnumRedirectMap[93] = 93;


	MetaList[93] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[94] = "PrintScreen";
    EnumRedirectMap[94] = 94;


	MetaList[94] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[95] = "Space";
    EnumRedirectMap[95] = 95;


	MetaList[95] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[96] = "MouseLeftButton";
    EnumRedirectMap[96] = 96;


	MetaList[96] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[97] = "MouseRightButton";
    EnumRedirectMap[97] = 97;


	MetaList[97] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[98] = "MouseMidButton";
    EnumRedirectMap[98] = 98;


	MetaList[98] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGType::GenerateType<EKeyCode>(), "EKeyCode", EnumRedirectMap, EnumStringList , MetaList);
}


