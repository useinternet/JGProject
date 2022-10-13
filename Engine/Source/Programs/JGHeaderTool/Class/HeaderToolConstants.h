#pragma once
#include "Core.h"

namespace HHeaderToolConstants
{
	/*
		const PString codeGenHeaderFileNameTemplate = "Module.{MODULE_NAME}.{FILE_NAME}.generation.cpp";
	const PString codeGenCPPFileNameTemplate    = "Module.{MODULE_NAME}.{FILE_NAME}.generation.h";

	const PString token_module_name = "{MODULE_NAME}";
	const PString token_file_name   = "{FILE_NAME}";
	*/
	namespace Token
	{
		static const PString Class = "JGCLASS";
		static const PString Enum  = "JGENUM";
		static const PString EnumMeta = "JGENUMMETA";
		static const PString Property = "JGPROPERTY";
		static const PString Function = "JGFUNCTION";

		static const PString ModuleName = "{MODULE_NAME}";
		static const PString FileName = "{FILE_NAME}";
		static const PString ClassName = "{CLASS_NAME}";
		static const PString EnumName = "{ENUM_NAME}";
	}

	namespace Template
	{
		static const PString CodeGenHeaderFileName = "Module.{MODULE_NAME}.{FILE_NAME}.generation.h";
		static const PString CodeGenCppFileName = "Module.{MODULE_NAME}.{FILE_NAME}.generation.cpp";

		static const PString CodeGenCreateStaticClassFunction = "PSharedPtr<JGClass> Module_{MODULE_NAME}_Code_Generation_Static_Create_Class_{CLASS_NAME}()";
		static const PString CodeGenCreateClassFunction = "PSharedPtr<JGClass> Module_{MODULE_NAME}_Code_Generation_Create_Class_{CLASS_NAME}(const {CLASS_NAME}* fromThis)";

		static const PString CodeGenCreateStaticEnumFunction = "PSharedPtr<JGClass> Module_{MODULE_NAME}_Code_Generation_Static_Create_Enum_{ENUM_NAME}()";
		static const PString CodeGenCreateEnumFunction = "PSharedPtr<JGClass> Module_{MODULE_NAME}_Code_Generation_Create_Enum_{ENUM_NAME}(const {ENUM_NAME}* fromThis)";
	}


};