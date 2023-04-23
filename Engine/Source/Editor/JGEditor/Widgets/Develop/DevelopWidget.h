#pragma once
#include "Widget.h"


class WText;
class WButton;
class JGDevelopUnit;
class WDevelopUnitExecuter;
class WDevelopUnitList;
class WDevelopUnitContent;

// 개발 위젯
// dll빌드해서 붙히고, 뗄수있도록 제작
/*
필요한 위젯

DetailWidget ( 디테일 )
PreviewSceneWidget ( 프리뷰 )
ConsoleWidget ( 명령어 )

*/
class WDevelopWidget : public WWidget
{
	// 맨위에 Develop 열 dll 이름 작성 후 열기 버튼
	// 왼쪽엔 프로세스 리스트
	// 오른쪽엔 테스트 화면

	// 연결할 프로세스 이름 (dll 이름 )
	// OnOpen시 dll 열기

	PSharedPtr<WDevelopUnitExecuter> _developUnitExecuter;
	PSharedPtr<WDevelopUnitList>     _developUnitList;
	PSharedPtr<WDevelopUnitContent>  _developUnitContent;

	//JGDevelopUnit* _developUnit = nullptr;
	//HJInstance _dllInstance;
public:
	WDevelopWidget() = default;
	virtual ~WDevelopWidget() = default;

	virtual void OnOpen() override;
	virtual void OnClose() override;
	virtual void OnUpdate() override;
	virtual void OnGUIBuild(HGUIBuilder& inBuilder) override;
	virtual void OnMenuBuild(HMenuBuilder& inBuilder) override;
	virtual void OnContextMenuBuild(HContextMenuBuilder& inBuilder) override;

protected:
	//virtual void OnCreateDevelopUnit() {}
	//virtual void OnDestoryDevelopUnit() {}

	//void SetDevelopUnit(const PString& inDevelopUnitName);

//private:
//	void CreateDevelopUnit(const PString& inDevelopUnitName);
//	void Reset();
};