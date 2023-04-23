#pragma once
#include "Widget.h"


class WText;
class WButton;
class JGDevelopUnit;
class WDevelopUnitExecuter;
class WDevelopUnitList;
class WDevelopUnitContent;

// ���� ����
// dll�����ؼ� ������, �����ֵ��� ����
/*
�ʿ��� ����

DetailWidget ( ������ )
PreviewSceneWidget ( ������ )
ConsoleWidget ( ��ɾ� )

*/
class WDevelopWidget : public WWidget
{
	// ������ Develop �� dll �̸� �ۼ� �� ���� ��ư
	// ���ʿ� ���μ��� ����Ʈ
	// �����ʿ� �׽�Ʈ ȭ��

	// ������ ���μ��� �̸� (dll �̸� )
	// OnOpen�� dll ����

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