#include "pch.h"
#include "CommandPrompt.h"
#include "Application.h"

#include "Event/GameEvent.h"
#include "Class/Asset/AssetImporter.h"
#include "Class/Asset/AssetHelper.h"

#define COMMAND_BIND_FUNC(x) std::bind(&CommandPrompt::##x, this, std::placeholders::_1, std::placeholders::_2)
namespace JG
{
	CommandPrompt::CommandPrompt()
	{
		LoadCommand();
	}
	void CommandPrompt::Execute(const String& command)
	{
		JG_LOG_TRACE(">> {0}", command);

		List<String> commandAndArgs = StringHelper::Split(command, ' ');
		if (commandAndArgs.empty())
		{
			return;
		}
		if (mRegisteredCommand.find(commandAndArgs[0]) == mRegisteredCommand.end())
		{
			JG_LOG_ERROR(">> Does Not Exist Command ..");
			return;
		}


		List<String> args;
		if (commandAndArgs.size() > 1)
		{
			args.insert(args.end(), commandAndArgs.begin() + 1, commandAndArgs.end());
		}
		
		Execute(commandAndArgs[0], args);
	}
	void CommandPrompt::Execute(const String& command, const List<String>& args)
	{
		mRegisteredCommand[command](command, args);
	}
	void CommandPrompt::LoadCommand()
	{
		RegisterCommand("savegameworld", COMMAND_BIND_FUNC(Command_SaveGameWorld));
		RegisterCommand("assetimport", COMMAND_BIND_FUNC(Command_AssetImport));
	}
	void CommandPrompt::RegisterCommand(const String& command, const std::function<void(const String&, const List<String>&)>& func)
	{

		mRegisteredCommand[StringHelper::ToLower(command)] = func;
	}
	void CommandPrompt::Command_SaveGameWorld(const String& command, const List<String>& args)
	{
		RequestSaveGameWorldEvent e;
		Application::GetInstance().SendEvent(e);
	}
	void CommandPrompt::Command_AssetImport(const String& command, const List<String>& args)
	{
		if (args.empty() == true)
		{
			JG_LOG_TRACE(">> Try AssetImport -help");
			return;
		}
		if (args.size() == 1)
		{
			if (args[0] == "help")
			{
				JG_LOG_TRACE(R"(AssetImport [ImprotAssetType] [Import Args...]
ImportAssetType 
	[Texture] 
		Supported File Format : .png, .jpg, .tga
			DestPath   : file path to import 
			OutputPath : path to export
	[CubeMap] : 
		Supported File Format : .png, .jpg, .tga
		Arguments : [DestPath] [OutputPath] [Flags]
			DestPath : If it is a directory, numbers [_1,_2,_3] must be attached to the end of the file name. , If it is a file, you must enter six file paths.
			OutputPath : path to export
	[FBX] 
		Supported File Format : .fbx
		Arguments : [DestPath] [OutputPath] [Flags]
			DestPath   : file path to import 
			OutputPath : path to export
)");
			}
		}
		else if (args.size() >= 3)
		{
			String importAssetType = args[0];
			if (importAssetType == "texture" && args.size() == 3)
			{
				String srcPath    = args[1];
				String outputPath = args[2];

				TextureAssetImportSettings importSettings;
				AssetHelper::GetResourcePath(srcPath, &srcPath, nullptr);
				AssetHelper::GetResourcePath(outputPath, &outputPath, nullptr);
				importSettings.AssetPath  = srcPath;
				importSettings.OutputPath = outputPath;
				importSettings.Flags = ETextureAssetImportFlags::None;

				if (fs::exists(srcPath) == false)
				{
					JG_LOG_ERROR(">> not exist file : {0}", srcPath);
				}

				
				JG_LOG_TRACE(">> Import Texture {0} -> {1}", srcPath, outputPath);

				if (AssetImporter::Import(importSettings) == EAssetImportResult::Fail)
				{
					JG_LOG_ERROR(">> Fail Import Texture");
				}
				else
				{
					JG_LOG_ERROR(">> Scuccess Import Texture");
				}
				
			}
			else if (importAssetType == "cubemap")
			{

			}
			else if (importAssetType == "fbx")
			{
				String srcPath    = args[1];
				String outputPath = args[2];

				JG_LOG_TRACE(">> Import FBX {0} -> {1}", srcPath, outputPath);
			}
			else
			{
				JG_LOG_ERROR(">> Not Supported Import Asset");
			}
		}
		else
		{
			JG_LOG_ERROR(">> Not Enough Arguments");
		}


	}
}