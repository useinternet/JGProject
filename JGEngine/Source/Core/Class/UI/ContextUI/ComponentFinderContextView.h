#pragma once
#include "Class/UI/UIView.h"



namespace JG
{
	class ComponentFinderInitData
	{
	public:

	};


	class ComponentFinderContextView : public ContextUIView<ComponentFinderInitData>
	{
		JGCLASS
	private:
		String            mSelectedComponentType;
		String            mFilterStr;
		HashSet<Type>     mComponentTypeSet;
		SortedSet<String> mFilteredTypeList;
	protected:
		virtual void Initialize(const ComponentFinderInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI() override;
	public:
		const String& GetSelectedComponent() const;
	private:
		void FindComponentTypeList(const String& filter);

	};

}



