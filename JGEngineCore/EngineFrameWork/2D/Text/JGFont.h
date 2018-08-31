#pragma once
#include"../../../EngineStatics/Engine.h"



class JGTexture;
class JG2DMesh;
class JGDeviceD;
class JGBufferManager;

class ENGINE_EXPORT JGFontLoader
{
private:
	struct FontMesh;
	struct FontInformation;
	struct FontCommonType;
	struct FontType;
	struct KerningType;
	typedef unsigned int CharID;
	typedef std::pair<const CharID, const CharID> PairID;
	struct FontVertex
	{
		D3DXVECTOR3 position = { 0.0f,0.0f,0.0f };
		D3DXVECTOR2 texture = { 0.0f,0.0f };
	};
	struct FontInformation
	{
		/*
		FontName : 실제 타입 폰트의 이름입니다.
		FontSize : 트루 타입 폰트의 크기.
		bold     : 글꼴은 굵게 표시됩니다.
		italic   : 꼴 기울임 꼴입니다.
		charset  : 사용 된 OEM 문자 세트의 이름 (유니 코드가 아닌 경우).
		unicode  : 유니 코드 문자 세트이면 1로 설정하십시오.
		stretchH : 글꼴 높이가 백분율로 늘어납니다. 100 %는 신축성이 없음을 의미합니다.
		smoothing: smoothing이 켜져 있으면 1로 설정하십시오.
		aa       : 사용 된 슈퍼 샘플링 레벨. 1은 슈퍼 샘플링이 사용되지 않았 음을 의미합니다.
		padding[4] : 각 문자의 패딩 (위쪽, 오른쪽, 아래쪽, 왼쪽).
		spacing[2] : 각 문자의 간격 (수평, 수직).
		outline    : 문자의 윤곽선 두께입니다.
		*/
		std::string  FontName;
		size_t FontSize;
		size_t bold;
		size_t italic;
		std::string  charset;
		bool  unicode;
		int   stretchH;
		bool  smoothing;
		bool  aa;
		int padding[4];
		int spacing[2];
		size_t outline;
	};
	struct FontCommonType
	{
		/*
		lineHeight : 각 텍스트 행 사이의 거리 (픽셀 단위)입니다.
		base       : 행의 절대 상단에서 문자의 밑까지의 픽셀 수입니다.
		scaleW     : 텍스처의 폭. 일반적으로 문자 이미지의 x pos 크기를 조정하는 데 사용됩니다.
		scaleH     : 텍스처의 높이. 일반적으로 문자 이미지의 y pos 크기를 조절하는 데 사용됩니다.
		pages      : 글꼴에 포함 된 텍스쳐 페이지의 수.
		packed     : 단색 문자가 각 텍스처 채널에 압축되어 있으면 1로 설정합니다. 이 경우 alphaChnl은 각 채널에 저장된 내용을 설명합니다.
		alphaChnl  : 채널에 그리 후 데이터가있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 윤곽을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
		redChnl    : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그 레프와 외곽선을 보관 유지하고있는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
		greenChnl  : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 외곽선을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
		blueChnl   : 채널에 그리 후 데이터가 보관 유지되고있는 경우는 0으로, 윤곽을 보관 유지하고있는 경우는 1, 그리 후와 외곽선을 보관 유지하는 경우는 2, 그 값이 제로의 경우는 3, 그 외의 경우는 4가됩니다.
		pageID     : 페이지 ID.
		FileName   : 텍스처 파일 이름.
		*/
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
	};
	struct FontType
	{
		/*
		ID      : 문자 ID입니다.
		x       : 텍스쳐의 문자 이미지의 왼쪽 위치.
		y       : 텍스처의 문자 이미지의 위쪽 위치입니다.
		Width   : 텍스처의 문자 이미지의 너비입니다.
		Height  : 텍스처의 문자 이미지의 높이입니다.
		XOffset : 텍스처에서 화면으로 이미지를 복사 할 때 현재 위치가 얼마나 오프셋되어야 하는지를 지정합니다.
		YOffset : 텍스처에서 화면으로 이미지를 복사 할 때 현재 위치가 얼마나 오프셋되어야 하는지를 지정합니다.
		XAdvance : 캐릭터 그리기 후 현재 위치를 얼마만큼 앞당겨 야합니다.
		Page    : 문자 이미지가있는 텍스쳐 페이지.
		chnl    : 문자 이미지가있는 텍스처 채널 (1 = 파란색, 2 = 녹색, 4 = 빨간색, 8 = 알파, 15 = 모든 채널).

		*/
		CharID ID;
		size_t x;
		size_t y;
		size_t Width;
		size_t Height;
		float  XOffset;
		float  YOffset;
		float XAdvance;
		float Page;
		float chnl;
		std::vector<FontVertex> Mesh;
	};
	struct KerningType
	{
		/*
		first  : 첫 번째 문자 ID입니다.
		second : 두 번째 문자 ID입니다.
		amount : 첫 번째 문자 바로 다음에 두 번째 문자를 그릴 때 x 위치를 조정해야하는 정도입니다.
		*/
		CharID first;
		CharID second;
		float amount;
	};
	std::unique_ptr<FontInformation> m_FontInformation;
	std::unique_ptr<FontCommonType>  m_FontCommonInformation;
	std::map<const CharID, FontType> m_Font;
	std::map<PairID, const float> m_KerningInformation;
	size_t CharCount;
	size_t KerningCount;
	float m_Size;
public:
	JGFontLoader();
	~JGFontLoader();

	bool LoadFont(JGDeviceD* Device, JGBufferManager* BufferManager, const std::string& FontPath,
		const std::string& FontTexturePath, size_t FontSize);
	// 폰트만드는데 필요한거.( 폰트 정보 경로, 폰트 텍스쳐 경로, 폰트 크기)
};