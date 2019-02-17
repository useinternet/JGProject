#pragma once
#include"GeoMath.h"
using namespace std;
namespace JGLibrary
{
	void JVector2Test()
	{
		JVector2 v1(1.0f,1.0f);
		JVector2 v2(2.0f,-5.0f);
		JVector2 v3(1.0f, -1.4f);
		JVector2 v4(-3.0f, 2.4f);
		JVector2 temp;
		cout << "JVector2 Test =========" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "=======================" << endl << endl;

		cout << " *** v1 + v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 + v2 : " << (v1 + v2).ToString() << endl;
		cout << endl;

		cout << " *** v3 - v4 ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "v3 - v4 : " << (v3 - v4).ToString() << endl;
		cout << endl;

		cout << " *** v1 * 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1 * 2.0f : " << (v1 * 2.0f).ToString() << endl;
		cout << "2.0f * v1 : " << (2.0f * v1).ToString() << endl;
		cout << endl;


		cout << " *** v1 / 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1 / 2.0f : " << (v1 / 2.0f).ToString() << endl;
		cout << endl;


		cout << " *** v1 += v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		v1 += v2;
		cout << "v1(v1+=v2) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 -= v3 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v3 : " << v3.ToString() << endl;
		v1 -= v3;
		cout << "v1(v1-=v3) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 *= 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		v1 *= 2.0f;
		cout << "v1(v1*=2.0f) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 /= 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		v1 /= 2.0f;
		cout << "v1(v1/=2.0f) : " << v1.ToString() << endl;
		cout << endl;


		cout << " *** v1 == v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 == v2 : " << (v1 == v2) << endl;
		cout << endl;

		cout << " *** v1 != v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 != v2 : " << (v1 != v2) << endl;
		cout << endl;

		cout << " *** v1.operator[] *** "<< endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1[0] : " << v1[0] << endl;
		cout << "v1[1] : " << v1[1] << endl;
		cout << endl;


		cout << " *** v3.normalize ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		temp = v3;
		v3.Normalize();
		temp.Normalize();
		cout << "v3(member) : " << v3.ToString() << endl;
		cout << "v3(inline) : " << temp.ToString() << endl;
		cout << endl;


		cout << " *** v4.lengthsq ***" << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "lengthsq : " << v4.LengthSqrd() << endl;
		cout << endl;

		cout << " *** v3.length ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "length : " << v3.Length() << endl;
		cout << endl;

		cout << " *** v1 dot v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "dot(member) : " << v1.Dot(v2) << endl;
		cout << "dot(inline) : " << Dot(v1,v2) << endl;
		cout << endl;
	}


	void JVector3Test()
	{
		JVector3 v1(1.0f, 1.0f,1.0f);
		JVector3 v2(2.0f, -5.0f,1.2f);
		JVector3 v3(1.0f, -1.4f,2.1f);
		JVector3 v4(-3.0f, 2.4f, -2.5f);
		JVector3 temp;
		cout << "JVector3 Test =========" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "=======================" << endl << endl;

		cout << " *** v1 + v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 + v2 : " << (v1 + v2).ToString() << endl;
		cout << endl;

		cout << " *** v3 - v4 ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "v3 - v4 : " << (v3 - v4).ToString() << endl;
		cout << endl;

		cout << " *** v1 * 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1 * 2.0f : " << (v1 * 2.0f).ToString() << endl;
		cout << "2.0f * v1 : " << (2.0f * v1).ToString() << endl;
		cout << endl;


		cout << " *** v1 / 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1 / 2.0f : " << (v1 / 2.0f).ToString() << endl;
		cout << endl;


		cout << " *** v1 += v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		v1 += v2;
		cout << "v1(v1+=v2) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 -= v3 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v3 : " << v3.ToString() << endl;
		v1 -= v3;
		cout << "v1(v1-=v3) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 *= 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		v1 *= 2.0f;
		cout << "v1(v1*=2.0f) : " << v1.ToString() << endl;
		cout << endl;

		cout << " *** v1 /= 2.0f ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		v1 /= 2.0f;
		cout << "v1(v1/=2.0f) : " << v1.ToString() << endl;
		cout << endl;


		cout << " *** v1 == v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 == v2 : " << (v1 == v2) << endl;
		cout << endl;

		cout << " *** v1 != v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v1 != v2 : " << (v1 != v2) << endl;
		cout << endl;

		cout << " *** v1.operator[] *** " << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v1[0] : " << v1[0] << endl;
		cout << "v1[1] : " << v1[1] << endl;
		cout << "v1[2] : " << v1[2] << endl;
		cout << endl;


		cout << " *** v3.normalize ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		temp = v3;
		v3.Normalize();
		temp.Normalize();
		cout << "v3(member) : " << v3.ToString() << endl;
		cout << "v3(inline) : " << temp.ToString() << endl;
		cout << endl;


		cout << " *** v4.lengthsq ***" << endl;
		cout << "v4 : " << v4.ToString() << endl;
		cout << "lengthsq : " << v4.LengthSqrd() << endl;
		cout << endl;

		cout << " *** v3.length ***" << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "length : " << v3.Length() << endl;
		cout << endl;

		cout << " *** v1 Dot v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "dot(member) : " << v1.Dot(v2) << endl;
		cout << "dot(inline) : " << Dot(v1, v2) << endl;
		cout << endl;


		cout << " *** v1 Cross v2 ***" << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "v2 : " << v2.ToString() << endl;
		cout << "dot(member) : " << v1.Cross(v2).ToString() << endl;
		cout << "dot(inline) : " << Cross(v1, v2).ToString() << endl;
		cout << "Dot(v1 dot Cross of member) : " << Standard(v1.Dot(v1.Cross(v2))) << endl;
		cout << "Dot(v1 dot Cross of inline) : " << Standard(v1.Dot(Cross(v1, v2))) << endl;
		cout << endl;

	
		cout << "*** v1 CoordinateSystem *** " << endl;
		cout << "v1 : " << v1.ToString() << endl;
		cout << "Change CoordinateSystem---" << endl;
		v1.CoordinateSystem(&v2, &v3);
		cout << "v2 : " << v2.ToString() << endl;
		cout << "v3 : " << v3.ToString() << endl;
		cout << "v1 Dot v2 : " << Standard(v1.Dot(v2)) << endl;
		cout << "v1 Dot v3 : " << Standard(v1.Dot(v3)) << endl;
		cout << "v2 Dot v3 : " << Standard(v2.Dot(v3)) << endl;
		cout << endl;

	}

	void JPoint2Test()
	{

	}

	void JPoint3Test()
	{

	}
	void JRayTest()
	{

	}
	void Bound2Test()
	{

	}
	void Bound3Test()
	{

	}
	void InlineFunctionTest()
	{

	}
}


