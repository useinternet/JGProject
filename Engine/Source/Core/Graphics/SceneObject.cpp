#include "pch.h"
#include "SceneObject.h"
#include "JGGraphics.h"


namespace JG
{
	namespace Graphics
	{
		bool StaticRenderObject::IsValid() const
		{
			bool result = true;

			if (Mesh == nullptr || Mesh->IsValid() == false)
			{
				result = false;
			}
			if (MaterialList.empty())
			{
				result = false;
			}
			else
			{
				for (auto& m : MaterialList)
				{
					if (m == nullptr)
					{
						result = false;
						break;
					}
				}
			}

			return result;
		}
		bool DebugRenderObject::IsValid() const
		{
			bool result = true;

			if (Mesh == nullptr || Mesh->IsValid() == false)
			{
				result = false;
			}
			if (MaterialList.empty())
			{
				result = false;
			}
			else
			{
				for (auto& m : MaterialList)
				{
					if (m == nullptr)
					{
						result = false;
						break;
					}
				}
			}

			return result;
		}
	}
}
