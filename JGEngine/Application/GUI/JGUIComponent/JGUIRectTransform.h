#pragma once

#include "GUI/JGUIObject/JGUIComponent.h"

class JGUIRectTransform : public JGUIComponent
{
public:
	JGUIRectTransform(
		const JVector2& pos = { 0.0f,0.0f },
		float angle = 0.0f, 
		const JVector2 & scale = { 1.0f,1.0f });
protected:

public:
	virtual void SetPosition(const JVector2& pos);
	virtual void SetPosition(float x, float y);
	virtual void OffsetPosition(const JVector2& offset);
	virtual void OffsetPosition(float x, float y);



	void SetAngle(float angle);
	void OffsetAngle(float angle);


	void SetScale(const JVector2& scale);
	void SetScale(float x, float y);
	void OffsetScale(const JVector2& scale);
	void OffsetScale(float x, float y);


	void SetSize(const JVector2& size);
	void SetSize(float x, float y);
	void OffsetSize(const JVector2& offset);
	void OffsetSize(float x, float y);

	void SetPivot(const JVector2& pivot);
	void SetPivot(float x, float y);
	void OffsetPivot(const JVector2& pivot);
	void OffsetPivot(float x, float y);

	bool IsDirty() const;
	void Flush();
public:
	const JVector2& GetLocalPosition() const;
	const JVector2& GetPosition() const;
	float    GetLocalAngle() const;
	float    GetAngle() const;
	const JVector2& GetScale() const;
	const JVector2& GetSize()  const;
	const JVector2& GetPivot() const;
	const JVector2& GetLocalPivot() const;
	JGUIRect GetRect()  const;
	JGUIRect GetLocalRect() const;
private:
	void SendDirty(int n);

protected:
	JVector2    m_LocalPosition;
	float       m_LocalAngle;
	JVector2    m_LocalPivot;
	JVector2    m_Scale;
	JVector2    m_Size;
	mutable JVector2 m_Pivot;
	mutable JVector2 m_Position;
	mutable float    m_Angle = 0.0f;

	mutable bool m_IsDirty[3];
	enum
	{
		PosDirty = 0,
		AngleDirty = 1,
		PivotDirty = 2
	};
};

class JGUIWinRectTransform : public JGUIRectTransform
{

public:
	virtual void SetPosition(const JVector2& pos);
	virtual void SetPosition(float x, float y);
	virtual void OffsetPosition(const JVector2& offset);
	virtual void OffsetPosition(float x, float y);
private:
	void SendPosToWin();


};