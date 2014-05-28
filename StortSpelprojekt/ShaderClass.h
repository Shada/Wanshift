#pragma once
#include <string>
#include "RenderFactory.h"

class ShaderClass
{
private:
	// indices to compiled shaders stored in shared container
	int vertexShaderIndex;
	int geometryShaderIndex;
	int hullShaderIndex;
	int domainShaderIndex;
	int pixelShaderIndex;

	RenderInterface *graphics;
	/*	will tell the library to compile this shader and put it
		in the shared container.
		
		#_shader: path to the shader
		#_shaderModel: accepted arguments are: vs_5_0, gs_5_0, hs_5_0, ds_5_0, ps_5_0

		#returns:
			int that points to where it was put in the container. */
	int insertShader(std::string _shader, std::string _shaderModel,int _layoutType);
public:
	/*	Constructor, creates intance that will hold pointers to shaders, used as a technique.
			#all arguments are paths to the respective shaders, leave empty if unused */
	ShaderClass(std::string _vertexShader, std::string _geometryShader, std::string _hullShader, 
				std::string _domainShader, std::string _pixelShader,RenderInterface *_g,int _layoutType, bool _streamOutEnabled);

	/* applies these shaders to the pipeline */
	void use();
	~ShaderClass();
};