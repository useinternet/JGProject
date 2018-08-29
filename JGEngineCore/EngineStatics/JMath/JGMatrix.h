#pragma once
#include"../Engine.h"




class JGVector2D;
class JGAngle2D;
class JGScale2D;
class JGVector3D;
class ENGINE_EXPORT JGMatrix
{
private:
	D3DXMATRIX m_Matrix;

public:
	JGMatrix();
	JGMatrix(const D3DXMATRIX& matrix);
	JGMatrix(const JGMatrix& matrix);
	~JGMatrix();

	/*
	Exp : 이 행렬을 항등 행렬로 만든다. */
	void Identity();
	/*
	Exp : 이 행렬을 전치 행렬로 만든다. */
	void Transpose();
	/*
	Exp : 이 행렬의 역행렬을 구한다. 
	@param matrix : 이 행렬의 역행렬이 될 행렬 */
	float Inverse(JGMatrix* matrix);
	/*
	Exp : 스케일 행렬로 만든다. */
	void MakeScalingMatirx(const float x, const float y, const float z);
	/*
	Exp : 스케일 행렬로 만든다. */
	void MakeScalingMatirx(JGScale2D& Scale);

	/*
	Exp : 회전 행렬을 만든다. */
	void MakeRotationMatirx(const float x, const float y, const float z);
	/*
	Exp : 회전 행렬(2D용)을 만든다. */
	void MakeAngle2DMatrix(const float angle);
	/*
	Exp : 회전 행렬(2D용)을 만든다. */
	void MakeAngle2DMatrix(JGAngle2D& angle);
	/*
	Exp : 이동 행렬을 만든다. */
	void MakeTranslationMatrix(const float x, const float y, const float z);
	/*
	Exp : 이동 행렬(2D)을 만든다. */
	void MakeTranslation2DMatrix(const float ScreenWidth, const float ScreenHeight,const float x, const float y);
	/*
	Exp : 이동 행렬(2D)을 만든다. */
	void MakeTranslation2DMatrix(const float ScreenWidth, const float ScreenHeight,JGVector2D& vector);

	/*
	Exp : 뷰행렬을 만든다. 
	@param JGVector3D location : 물체의 위치 
	@param JGVector3D lookAt : 물체가 바라보는 벡터 
	@param JGVector3D up : 물체의 위쪽 벡터 */
	void MakeViewMatrix(JGVector3D* location, JGVector3D* lookAt, JGVector3D* up);

	/*
	Exp : 투영 행렬을 만든다.
	@param const float FOV : field of view ( 시야각 ) 
	@param const float ScreenAspect : 화면 비율 
	@param const float NearZ : 렌더링되는 최소 Z거리 
	@param const float FarZ : 렌더링 되는 최대 Z거리 */
	void MakeProjectionMatrix(const float FOV, const float ScreenAspect, const float NearZ, const float FarZ);
	/*
	Exp : 정사영 행렬을 만든다. 
	@param const float width : 스크린 가로 길이
	@param const float height : 스크린 세로 길이
	@param const float NearZ : 렌더링되는 최소 Z거리
	@param const float FarZ : 렌더링 되는 최대 Z거리 */
	void MakeOrthoMatrix(const float width, const float height, const float NearZ, const float FarZ);

	/*
	Exp : 해당 슬롯에 존재하는 값을 변경한다. ( 4x4 행렬이다. )*/
	void SetElement(const UINT slot, const float Value);
	/*
	Exp : 해당 슬롯에 존재하는 값을 변경한다. ( 4x4 행렬이다. )*/
	float GetElement(const UINT slot);

	/*
	Exp : 데이터 전용 값으로 가져온다. ( 4x4 행렬이다. )*/
	D3DXMATRIX& Get();

	/*
	Exp : 데이터 전용 값 주소로 가져온다. ( 4x4 행렬이다. )*/
	D3DXMATRIX* GetAddress();

	// 연산자 오버로딩
	JGMatrix& operator=(const JGMatrix& matrix);

	JGMatrix operator+(JGMatrix& matrix);
	JGMatrix operator-(JGMatrix& matrix);
	JGMatrix operator*(JGMatrix& matrix);

	JGMatrix& operator+=(JGMatrix& matrix);
	JGMatrix& operator-=(JGMatrix& matrix);
	JGMatrix& operator*=(JGMatrix& matrix);
	
};