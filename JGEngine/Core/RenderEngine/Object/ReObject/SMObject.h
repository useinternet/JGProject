#pragma once


#include "Object/ReObject.h"



namespace RE
{
	class RENDERENGINE_API SMObject : public ReObject
	{

	public:
		SMObject() : ReObject("SMObject") { }
		SMObject(const std::string& name) : ReObject(name) {}

	private:



	};

	class RENDERENGINE_API SMGraphicsObject : public SMObject
	{
	public :
		SMGraphicsObject() : SMObject("SMGraphicsObject") {}
		SMGraphicsObject(const std::string& name) : SMObject(name) {}

	private:

		// 1. Data < - > Module data Name
        //
        // Mesh, SkeletalMesh
        // PSO
        // Instance
        // 
	};

	class RENDERENGINE_API SMComputeObject : public SMObject
	{
	public:
		SMComputeObject() : SMObject("SMComputeObject") {}
		SMComputeObject(const std::string& name) : SMObject(name) {}

	};
}