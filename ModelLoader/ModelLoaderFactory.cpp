#include "ModelLoaderFactory.h"
#include "MeshLoader.h"
#include "AnimationLoader.h"
#include "PlaneLoader.h"

namespace Loader
{
	ModelLoaderInterface* ModelLoaderAPI::createLoader(LoaderType _type)
	{
		ModelLoaderInterface* loader;
		switch(_type)
		{
			case LoaderType::MESHLOADER:
			{
				loader = new MeshLoader();
				break;
			}
			case LoaderType::ANIMATIONLOADER:
			{
				loader = new AnimationLoader();
				break;
			}
			case LoaderType::PLANELOADER:
			{
				loader = new PlaneLoader();
				break;
			}
			default:
			{
				loader = nullptr;
				break;
			}
		}
		return loader;
	}

	void ModelLoaderAPI::deleteLoader(ModelLoaderInterface* _loader)
	{
		if(_loader != nullptr)
		{
			delete _loader;
			_loader = nullptr;
		}
	}
}