#pragma once


#include "Core.h"
#include <string>
namespace GE
{

	class GRAPHICSENGINE_API GObject
	{
	public:
		GObject();
		uint64_t GetID() const
		{
			return ID;
		}

		void SetName(const std::string& name) {
			Name = name;
		}
		const std::string& GetName() const {
			return Name;
		}
	private:
		uint64_t ID = 0;
		std::string Name      = "GObject";
		
	};




}