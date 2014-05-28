#include "RenderFactory.h"
#include "RenderDX11.h"
#include <iostream>

namespace Render
{
	void RenderAPI::print()
	{
		printf("Greetings from Renderer\n");
	}

	RenderInterface* RenderAPI::createRenderer(Version _dxVersion)
	{
		switch(_dxVersion)
		{
		case Version::DX10:
			return nullptr;
			break;
		case Version::DX11:
			{
			RenderInterface *r = new RenderDX11();
			return r;
			break;
			}
		case Version::UNDEFINED:
		default:
			return nullptr;
		}
	}

	void RenderAPI::deleteRenderer(RenderInterface* _renderer)
	{
		if(_renderer != nullptr)
		{
			_renderer->release();
			delete _renderer;
		}
	}
}