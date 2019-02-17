#pragma once

#include"Location.h"
#include"Rotation.h"
#include"Scale.h"


namespace JGLibrary
{
	class CoordinationController
	{
		Transform _Transform;
	public:
		Location _Location;
		Rotation _Rotation;
		Scale    _Scale;
	public:
		CoordinationController()  = default;
		~CoordinationController() = default;
	public:
		Transform& GetTransform()
		{
			UpdateTransform();
			return _Transform;
		}
		xmfloat4x4 GetData()
		{
			UpdateTransform();
			_Transform.mData;
		}
		SimMatrix GetSimData()
		{
			UpdateTransform();
			_Transform.GetSIMD();
		}
	private:
		void UpdateTransform()
		{
			if (_Location.IsUpdate() || _Rotation.IsUpdate() || _Scale.IsUpdate())
			{
				_Transform = (_Scale.GetTransform()    *
					          _Rotation.GetTransform() *
					          _Location.GetTransform());
			}
		}
	};
}