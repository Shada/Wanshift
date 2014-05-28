#pragma once

#include "ModelLoaderInterface.h"

namespace Loader
{
	enum LoaderType
	{
		MESHLOADER,
		ANIMATIONLOADER,
		PLANELOADER
	};
	/*	This is a fbx loader interface factory
	- use this for creating and deleting fbx loader instances */
	class ModelLoaderAPI
	{
	public:
		/*	Instantiate a new fbx loader by type

		#returns
		InputInterface* on success
		nullptr on invalid request
		nullptr on (target environment / type rquest) incompatability
		*/
		static __declspec(dllexport) ModelLoaderInterface* createLoader(LoaderType _type); // here I can define the type. If I want to have a fbx-loader, obj-loader or something else. Maybe the animation also in separate, if I want to fix that shit.

		/*	Shut down and delete fbx loader instance

		#_loader: instance to be deleted */
		static __declspec(dllexport) void deleteLoader(ModelLoaderInterface* _loader);
	};
}
