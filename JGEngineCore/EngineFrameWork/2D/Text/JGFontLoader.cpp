#include"JGFontLoader.h"
#include"../../../RenderSystem/JGTexture.h"
#include"../../../EngineStatics/JGLog.h"
using namespace std;
static JGFontLoader* Instance = nullptr;
JGFontLoader::JGFontLoader()
{
	if (Instance == nullptr)
	{
		Instance = this;
	}
	else
	{
		JGLog::Write(ELogLevel::Error, TT("JGFontLoader Construct Count is exceed one."));
	}
}
JGFontLoader::~JGFontLoader()
{

}
JGFontLoader* JGFontLoader::GetInstance()
{
	return Instance;
}
bool JGFontLoader::LoadFont(JGDeviceD* Device, const string& FontPath, const wstring& FontTexturePath)
{
	// 이미 로딩되어있다면 반환..
	auto iter = m_mFontPath.find(FontPath);
	if (iter != m_mFontPath.end())
	{
		return true;
	}
	std::unique_ptr<Font> font = make_unique<Font>();

	bool result = font->Texture->Add(Device, FontTexturePath);
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Load Fond(%s) Failed."), FontTexturePath);
		return false;
	}
	ifstream fin;
	fin.open(FontPath);
	if (fin.fail())
	{
		JGLog::Write(ELogLevel::Error, TT("Load Fond(%s) Failed."), FontTexturePath);
		return false;
	}
	// 정보들 저장..
	LoadFontInformation(fin,font.get());
	LoadFontCommonInformation(fin, font.get());
	LoadFontData(fin, font.get());
	LoadKerningInformation(fin, font.get());
	//삽입..
	m_mFonts.insert(pair<const string, shared_ptr<Font>>(FontPath, move(font)));
	m_mFontPath.insert(pair<const string, const wstring>(FontPath, FontTexturePath));
	return true;
}

void JGFontLoader::OutputVertexInformation(const string& FontPath,const wstring& Text, float TextSize, std::vector<JGFontVertexInformation>* Array)
{
	Font* font = m_mFonts[FontPath].get();
	JGFontVertexInformation VertexInformation;
	float weight = font->FontCommonInformation->scaleW;
	float height = font->FontCommonInformation->scaleH;
	float AccX = 0.0f;
	for (size_t i = 0; i < Text.size(); ++i)
	{
		CharID id = (int)Text[i];
		SFontType type = font->mFontType[id];

		// 비율
		float WidthAspect = (float)type.Width / (float)(type.Width + type.Height);
		float HeightAspect = (float)type.Height / (float)(type.Width + type.Height);

		// 비율에따른 크기 
		VertexInformation.Width = TextSize * (1.0f + WidthAspect);
		VertexInformation.Height = TextSize * (1.0f + HeightAspect);

		// 텍스쳐 위치
		VertexInformation.TexWidth = (float)type.Width / weight;
		VertexInformation.TexHeight = (float)type.Height / height;

		// 텍스쳐 uv 구하기
		VertexInformation.TexU = type.x / weight;
		VertexInformation.TexV = type.y / height;
		
		VertexInformation.XAdvance = AccX;
		AccX += VertexInformation.Width;
		Array->push_back(VertexInformation);
	}
}
void JGFontLoader::LoadFontInformation(std::ifstream& fin, Font* font)
{
	char Temp = '.';
	size_t Count = 0;
	EqualLoop(fin);

	// 이름 받기
	fin.get(Temp);
	fin.get(Temp);
	while (Temp != '\"')
	{
		font->FontInformation->FontName.push_back(Temp);
		fin.get(Temp);
	}
	Count = 0;
	EqualLoop(fin);

	// 사이즈 받기
	fin >> font->FontInformation->FontSize;
	EqualLoop(fin);
	// Bold 값 받기
	fin >> font->FontInformation->bold;
	EqualLoop(fin);
	// iteralic값 받기
	fin >> font->FontInformation->italic;
	EqualLoop(fin);


	// charset 값 받기
	fin.get(Temp);
	fin.get(Temp);
	while (Temp != '\"')
	{
		font->FontInformation->FontName.push_back(Temp);
		fin.get(Temp);
	}
	Count = 0;
	EqualLoop(fin);

	// unicode
	fin >> font->FontInformation->unicode;
	EqualLoop(fin);

	// stretchH
	fin >> font->FontInformation->stretchH;
	EqualLoop(fin);

	// smooth
	fin >> font->FontInformation->smoothing;
	EqualLoop(fin);

	//aa
	fin >> font->FontInformation->aa;
	EqualLoop(fin);

	// padding
	for (int i = 0; i < 4; ++i)
	{
		fin >> font->FontInformation->padding[i];
		fin.get(Temp);
	}
	EqualLoop(fin);
	// spacing
	for (int i = 0; i < 2; ++i)
	{
		fin >> font->FontInformation->spacing[i];
		fin.get(Temp);
	}
	EqualLoop(fin);

	//outline
	fin >> font->FontInformation->outline;

}
void JGFontLoader::LoadFontCommonInformation(std::ifstream& fin, Font* font)
{
	char Temp = '.';
	size_t Count = 0;
	EqualLoop(fin);

	// lineHeight
	fin >> font->FontCommonInformation->lineHeight;
	EqualLoop(fin);


	// base
	fin >> font->FontCommonInformation->base;
	EqualLoop(fin);


	// scaleW
	fin >> font->FontCommonInformation->scaleW;
	EqualLoop(fin);


	// scaleH
	fin >> font->FontCommonInformation->scaleH;
	EqualLoop(fin);


	// pages
	fin >> font->FontCommonInformation->pages;
	EqualLoop(fin);

	// packed
	fin >> font->FontCommonInformation->packed;
	EqualLoop(fin);

	// alphaChnl
	fin >> font->FontCommonInformation->alphaChnl;
	EqualLoop(fin);

	// redChnl
	fin >> font->FontCommonInformation->redChnl;
	EqualLoop(fin);

	// greenChnl
	fin >> font->FontCommonInformation->greenChnl;
	EqualLoop(fin);

	// blueChnl
	fin >> font->FontCommonInformation->blueChnl;
	EqualLoop(fin);

	// pageID
	fin >> font->FontCommonInformation->pageID;
	EqualLoop(fin);

	// pages
	fin.get(Temp);
	fin.get(Temp);
	while (Temp != '\"')
	{
		font->FontCommonInformation->FileName.push_back(Temp);
		fin.get(Temp);
	}
	Count = 0;
}
void JGFontLoader::LoadFontData(std::ifstream& fin, Font* font)
{
	char Temp = '.';
	SFontType FontType;
	size_t Count = 0;
	EqualLoop(fin);

	// CharCount
	fin >> font->CharCount;
	EqualLoop(fin);

	for (size_t i = 0; i < font->CharCount; ++i)
	{
		// CharID
		fin >> FontType.ID;
		EqualLoop(fin);

		// x
		fin >> FontType.x;
		EqualLoop(fin);

		// y
		fin >> FontType.y;
		EqualLoop(fin);

		// Width
		fin >> FontType.Width;
		EqualLoop(fin);

		// Height
		fin >> FontType.Height;
		EqualLoop(fin);

		// XOffset
		fin >> FontType.XOffset;
		EqualLoop(fin);

		// YOffset
		fin >> FontType.YOffset;
		EqualLoop(fin);

		// XAdvance
		fin >> FontType.XAdvance;
		EqualLoop(fin);

		// Page
		fin >> FontType.Page;
		EqualLoop(fin);

		// chnl
		fin >> FontType.chnl;
		EqualLoop(fin);

		font->mFontType.insert(pair<const CharID, SFontType>(FontType.ID, FontType));
	}
}
void JGFontLoader::LoadKerningInformation(std::ifstream& fin,Font* font)
{
	char Temp = '.';
	SKerningType Kerning;
	size_t Count = 0;

	// KerningCount;
	fin >> font->KerningCount;
	EqualLoop(fin);

	for (size_t i = 0; i < font->KerningCount; ++i)
	{
		// first
		fin >> Kerning.first;
		EqualLoop(fin);

		// second
		fin >> Kerning.second;
		EqualLoop(fin);

		// amount
		fin >> Kerning.amount;
		if (i != font->KerningCount - 1)
		{
			EqualLoop(fin);
		}
		font->KerningInformation.insert(pair<PairID, const float>(PairID(Kerning.first, Kerning.second), Kerning.amount));
	}

}
void JGFontLoader::EqualLoop(ifstream& fin)
{
	char Temp = ' ';
	while (Temp != '=')
	{
		fin.get(Temp);
	}
}


JGFontLoader::Font::Font()
{
	Texture = make_unique<JGTexture>();
	FontInformation = make_unique<SFontInformation>();
	FontCommonInformation = make_unique<SFontCommonType>();
	CharCount = 0;
	KerningCount = 0;
}
JGFontLoader::Font::~Font()
{



}