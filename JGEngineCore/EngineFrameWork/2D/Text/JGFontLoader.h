#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../../../RenderSystem/JGMaterial/JG2DMesh.h"


class JGTexture;
class JGDeviceD;
class JGBufferManager;
/*
Class : JGFontVertexInformation 
Exp : 폰트 렌더링 정점 정보 입니다..( 가로 세로길이나 텍스쳐 uv 정보, 띄어쓰기 공백 등등 정보가 
들어있습니다. 
@m float Width     : 글자 가로 길이
@m float Height    : 글자 세로 길이
@m float TexWidth  : 텍스쳐 UV 좌표 가로 길이
@m float TexHeight : 텍스쳐 UV 좌표 세로 길이
@m float TexU      : 텍스쳐 U 좌표
@m float TexV      : 텍스쳐 V 좌표
@m float XAdvance  : X 좌표 상 글자 간격
@m bool Space      : 현재 글자가 스페이스(공백) 인가 여부.    */
class ENGINE_EXPORT JGFontVertexInformation
{
public:
	float Width     = 0.0f;
	float Height    = 0.0f;
	float TexWidth  = 0.0f;
	float TexHeight = 0.0f;
	float TexU      = 0.0f;
	float TexV      = 0.0f;
	float XAdvance  = 0.0f;
	bool  Space     = false;
};
/*
Class : JGFontLoader 
Exp : 전체 폰트 데이터를 관리하는 로더 클래스 입니다. 
@m map<const string, shared_ptr<Font>> m_mFonts    : 폰트들의 정보들을 관리하는 배열
@m map<const string, const wstring>    m_mFontPath : 폰트들의 경로를 관리하는 배열       */
class ENGINE_EXPORT JGFontLoader
{
private:
	class Font;
private:
	typedef unsigned int CharID;
	typedef std::pair<const CharID, const CharID> PairID;
public:
	/// JGFontLoader들의 정보 구조체 들..
	struct SFontMesh;
	struct SFontInformation;
	struct SFontCommonType;
	struct SFontType;
	struct SKerningType;
	/*
	Struct : SFontInformation
	Exp : 폰트 정보를 담은 구조체 입니다. 
	@s	FontName : 실제 타입 폰트의 이름입니다.
	@s	FontSize : 트루 타입 폰트의 크기.
	@s	bold     : 글꼴은 굵게 표시됩니다.
	@s	italic   : 꼴 기울임 꼴입니다.
	@s	charset  : 사용 된 OEM 문자 세트의 이름 (유니 코드가 아닌 경우).
	@s	unicode  : 유니 코드 문자 세트이면 1로 설정하십시오.
	@s	stretchH : 글꼴 높이가 백분율로 늘어납니다. 100 %는 신축성이 없음을 의미합니다.
	@s	smoothing: smoothing이 켜져 있으면 1로 설정하십시오.
	@s	aa       : 사용 된 슈퍼 샘플링 레벨. 1은 슈퍼 샘플링이 사용되지 않았 음을 의미합니다.
	@s	padding[4] : 각 문자의 패딩 (위쪽, 오른쪽, 아래쪽, 왼쪽).
	@s	spacing[2] : 각 문자의 간격 (수평, 수직).
	@s	outline    : 문자의 윤곽선 두께입니다.     */
	typedef struct SFontInformation
	{
		std::string  FontName;
		size_t FontSize;
		size_t bold;
		size_t italic;
		std::string  charset;
		bool   unicode;
		int    stretchH;
		bool   smoothing;
		bool   aa;
		int    padding[4];
		int    spacing[2];
		size_t outline;
	}SFontInformation;
	/*
	Struct : SFontCommonType
	Exp : 폰트 공통 타입( bmFont에서 붙인 이름..) 보통 폰트 텍스쳐에 관련된 정보를 담고 있습니다.
	@s  lineHeight : 각 텍스트 행 사이의 거리 (픽셀 단위)입니다.
	@s	base       : 행의 절대 상단에서 문자의 밑까지의 픽셀 수입니다.
	@s	scaleW     : 텍스처의 폭. 일반적으로 문자 이미지의 x pos 크기를 조정하는 데 사용됩니다.
	@s	scaleH     : 텍스처의 높이. 일반적으로 문자 이미지의 y pos 크기를 조절하는 데 사용됩니다.
	@s	pages      : 글꼴에 포함 된 텍스쳐 페이지의 수.
	@s	packed     : 단색 문자가 각 텍스처 채널에 압축되어 있으면 1로 설정합니다. 이 경우 alphaChnl은 각 채널에 저장된 내용을 설명합니다.
	@s	alphaChnl  : 채널에 그리 후 데이터가있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 윤곽을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
	@s	redChnl    : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그 레프와 외곽선을 보관 유지하고있는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
	@s	greenChnl  : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 외곽선을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
	@s	blueChnl   : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 외곽선을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
	@s	pageID     : 페이지 ID.
	@s	FileName   : 텍스처 파일 이름.*/
	typedef struct SFontCommonType
	{
		float lineHeight;
		float base;
		float scaleW;
		float scaleH;
		float pages;
		float packed;
		float alphaChnl;
		float redChnl;
		float greenChnl;
		float blueChnl;
		float pageID;
		std::string FileName;
	}SFontCommonType;
	/*
	Struct : SFontType 
	Exp : 실질 적인 데이터 받아올떄 쓰는 폰트 타입 정보입니다. ( 문자 데이터를 받아 폰트 텍스쳐에 글자를 받아올
	데이터가 들어있습니다. 
	@s	ID      : 문자 ID입니다.
	@s	x       : 텍스쳐의 문자 이미지의 왼쪽 위치.
	@s	y       : 텍스처의 문자 이미지의 위쪽 위치입니다.
	@s	Width   : 텍스처의 문자 이미지의 너비입니다.
	@s	Height  : 텍스처의 문자 이미지의 높이입니다.
	@s	XOffset : 텍스처에서 화면으로 이미지를 복사 할 때 현재 위치가 얼마나 오프셋되어야 하는지를 지정합니다.
	@s	YOffset : 텍스처에서 화면으로 이미지를 복사 할 때 현재 위치가 얼마나 오프셋되어야 하는지를 지정합니다.
	@s	XAdvance : 캐릭터 그리기 후 현재 위치를 얼마만큼 앞당겨 야합니다.
	@s	Page    : 문자 이미지가있는 텍스쳐 페이지.
	@s	chnl    : 문자 이미지가있는 텍스처 채널 (1 = 파란색, 2 = 녹색, 4 = 빨간색, 8 = 알파, 15 = 모든 채널).  */
	typedef struct SFontType
	{
		CharID ID;
		size_t x;
		size_t y;
		size_t Width;
		size_t Height;
		float  XOffset;
		float  YOffset;
		float  XAdvance;
		float  Page;
		float  chnl;
	}SFontType;
	/*
	Struct : SKerningType
	Exp : 글자 와 글자 사이의 간격 정보가 들어있습니다. 
	@s	first  : 첫 번째 문자 ID입니다.
	@s	second : 두 번째 문자 ID입니다.
	@s	amount : 첫 번째 문자 바로 다음에 두 번째 문자를 그릴 때 x 위치를 조정해야하는 정도입니다.*/
	typedef struct SKerningType
	{
		CharID first;
		CharID second;
		float amount;
	}SKerningType;
private:
	std::map<const std::string, std::shared_ptr<Font>> m_mFonts;
	std::map< const std::string, const std::wstring>   m_mFontPath;
public:
	JGFontLoader();
	~JGFontLoader();

	/*
	Exp : FontLoader의 인스턴스를 가져옵니다. (남용 금지) */
	static JGFontLoader* GetInstance();
	/*
	Exp : 폰트를 불러옵니다. 
	@param JGDeviceD* Device              : JGDevice 포인터
	@param const string& FontPath         : 폰트 경로
	@param const wstring& FontTexturePath : 폰트 텍스쳐 경로*/
	bool LoadFont(JGDeviceD* Device, const std::string& FontPath, const std::wstring& FontTexturePath);
	/*
	Exp : 해당 문장에 해당하는 정점 정보를 출력합니다. 
	@param const string& FontPath : 쓸 폰트 경로
	@param const wstring& Text    : 정점 정보로 바꿀 문장
	@param const float TextSize   : 폰트 크기
	@param vector<JGFontVertexInformation>* Array : 데이터 받을 정점 배열 */
	void OutputVertexInformation(const std::string& FontPath,const std::wstring& Text,const float TextSize,std::vector<JGFontVertexInformation>* Array);
private:
	/// BmFont 프로그램에서 쓸 폰트 불러오는 함수..
	void LoadFontInformation(std::ifstream& fin,Font* font);
	void LoadFontCommonInformation(std::ifstream& fin, Font* font);
	void LoadFontData(std::ifstream& fin, Font* font);
	void LoadKerningInformation(std::ifstream& fin, Font* font);
	void EqualLoop(std::ifstream& fin);
private:
	/// 폰트 불러올때 쓸 임시 클래스..
	class Font
	{
	public:
		std::unique_ptr<JGTexture>        Texture;
		std::unique_ptr<SFontInformation> FontInformation;
		std::unique_ptr<SFontCommonType>  FontCommonInformation;
		std::map<const CharID, SFontType> mFontType;
		std::map<PairID, const float>     KerningInformation;
		size_t CharCount;
		size_t KerningCount;

		Font();
		~Font();
	};
};