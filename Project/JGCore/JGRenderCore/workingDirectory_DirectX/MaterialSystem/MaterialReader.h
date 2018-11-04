#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"Shader/ShaderTool/ShaderObject.h"
// 머터리얼 형식의 파일 해석 완료
// 이걸 토대로 머터리얼시스템 완료후
// 텍스쳐(노멀 맵핑) <- 괜찬은 모델 구하기 해서 모델링 해보고
// 그림자 맵핑까지 함 해보자
// 그후 MRT 구현

namespace JGRC
{
	class CORE_EXPORT MaterialReader
	{
	private:
		static class MaterialSystem* m_MtSys;
		static class ShaderObjectManager* m_ObjManager;
		
	public:
		MaterialReader();
		~MaterialReader();

		class Material* Read(const std::string& materialPath);

	private:
		bool ReadShaderType(const std::string& buffer, std::fstream& fin, EShaderType* type);
		bool ReadHlslPath(const std::string& buffer, std::fstream& fin, std::string* path);
		bool ReadInputLayout(const std::string& buffer, std::fstream& fin, class Material* mt);
		bool ReadCBuffer(const std::string& buffer, std::fstream& fin, class Material* mt, EShaderType type);
		bool ReadSamplerState(const std::string& buffer, std::fstream& fin, class Material* mt);
		bool ReadTexture(const std::string& buffer, std::fstream& fin, class Material* mt);
		bool Move(const std::string& buffer, const std::string& keyword, std::fstream& fin);
	};
}
