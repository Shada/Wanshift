#include "ShaderClass.h"

ShaderClass::ShaderClass(std::string _vertexShader, std::string _geometryShader, std::string _hullShader,
						 std::string _domainShader, std::string _pixelShader, RenderInterface *_g, int _layoutType, bool _streamOutEnabled)
{
	graphics = _g;

	vertexShaderIndex = insertShader(_vertexShader, "vs_5_0", _layoutType);
	if(_streamOutEnabled == false)
	{
		geometryShaderIndex = insertShader(_geometryShader, "gs_5_0", -1);
	}
	else
	{
		geometryShaderIndex = graphics->createGeometryStreamOut(_geometryShader, "gs_5_0");
	}
	hullShaderIndex = insertShader(_hullShader, "hs_5_0", -1);
	domainShaderIndex = insertShader(_domainShader, "ds_5_0", -1);
	pixelShaderIndex = insertShader(_pixelShader, "ps_5_0", -1);
}

int ShaderClass::insertShader(std::string _shader, std::string _shaderModel, int _layoutType)
{
	if(_shader.empty())
	{
		return -1;
	}
	return graphics->createShader(_shader, _shaderModel, _layoutType);
}

void ShaderClass::use()
{
	graphics->setShaders(vertexShaderIndex, geometryShaderIndex, hullShaderIndex, domainShaderIndex, pixelShaderIndex);
}

ShaderClass::~ShaderClass()
{
}
