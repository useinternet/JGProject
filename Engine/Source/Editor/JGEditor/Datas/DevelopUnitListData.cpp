#include "PCH/PCH.h"
#include "DevelopUnitListData.h"
#include "Datas/DevelopUnitData.h"
#include "Misc/DevelopUnit.h"

void JGDevelopUnitListData::AddUnitData(JGDevelopUnit* inDevelopUnit)
{
	if (inDevelopUnit == nullptr)
	{
		return;
	}

	RemoveUnitData(inDevelopUnit);

	PSharedPtr<JGDevelopUnitData> unitData = Allocate<JGDevelopUnitData>();
	unitData->UnitGuid = inDevelopUnit->GetGuid();
	unitData->UnitType = inDevelopUnit->GetType();

	DevelopUnitDatas.push_back(unitData);
}

void JGDevelopUnitListData::RemoveUnitData(JGDevelopUnit* inDevelopUnit)
{
	if (inDevelopUnit == nullptr)
	{
		return;
	}
	
	HGuid devGuid = inDevelopUnit->GetGuid();
	for (int32 i = 0; i < DevelopUnitDatas.size(); ++i)
	{
		if (DevelopUnitDatas[i]->UnitGuid == devGuid)
		{
			DevelopUnitDatas.erase(DevelopUnitDatas.begin() + i);
			break;
		}
	}
}