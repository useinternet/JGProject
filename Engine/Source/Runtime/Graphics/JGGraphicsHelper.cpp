#include "PCH/PCH.h"
#include "JGGraphicsHelper.h"

uint64 HJGGraphicsHelper::GetShaderDataTypeSize(EShaderDataType dataType)
{
	PSharedPtr<JGEnum> Enum = StaticEnum<EShaderDataType>();
	if (Enum == nullptr)
	{
		return 0;
	}

	PSharedPtr<JGMeta> Meta = Enum->GetMetaDataByIndex((int32)dataType);
	if (Meta == nullptr)
	{
		return 0;
	}

	HHashSet<PName> Values;
	Meta->GetMetaValues(PName("DataSize"), Values);

	PString sizeStr = (*Values.begin()).ToString();
	int32 sizeInt   = sizeStr.ToInt();

	return sizeInt;
}

PName HJGGraphicsHelper::GetShaderDataTypeHLSLName(EShaderDataType dataType)
{
	PSharedPtr<JGEnum> Enum = StaticEnum<EShaderDataType>();
	if (Enum == nullptr)
	{
		return NAME_NONE;
	}

	PSharedPtr<JGMeta> Meta = Enum->GetMetaDataByIndex((int32)dataType);
	if (Meta == nullptr)
	{
		return NAME_NONE;
	}

	HHashSet<PName> Values;
	Meta->GetMetaValues(PName("HLSLName"), Values);

	return (*Values.begin());
}
