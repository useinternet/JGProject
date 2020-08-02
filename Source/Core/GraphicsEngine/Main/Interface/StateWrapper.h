#pragma once

namespace GE
{

	enum class RendererMode
	{
		_3D,
		Paper,
		GUI
	};


	enum class BlendMode
	{
		Opaque,
		Transparent
	};




	struct MaterialProperty
	{
		BlendMode blendMode       = BlendMode::Opaque;
		RendererMode rendererMode = RendererMode::_3D;


	};
}
