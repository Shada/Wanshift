#include <glm/glm.hpp>
#include <string>

namespace POST
{
	/* Weather Types */
	enum class PWEATHER
	{
		DESERT		= 0,
		MOUNTAIN	= 1,
		MEADOW		= 2,
		SWAMP		= 3, 
		FORREST,//x1
	};

	/* Find the dominant biom */
	static int getDominantBiomId(glm::vec4 _blendBiom)
	{
		int dom = 0;
		float val = 0;

		for(int i = 0; i < 4; i++)		
		{
			if( _blendBiom[i] > val )	
			{
				val = _blendBiom[i];
				dom = i;				
			}
		}
		return dom; 
	}

	/* interpolate T */
	template<typename T>
	static T interpolate(T _a, T _b, float _percent)
	{
		//return _a + _percent * (_b - _a);
		return (1.0f - _percent) * _a + (_b * _percent);
	}

	static float interpolate(float _a, float _b, float _percent)
	{
		return (1.0f - _percent) * _a + (_b * _percent);
	}

	static glm::vec3 interpolate(glm::vec3 _a, glm::vec3 _b, float _percent)
	{
		float da = 1.0f - _percent;
		float db = _percent;

		return glm::vec3( da * _a.x + db * _b.x, da * _a.y + db * _b.y, da * _a.z + db * _b.z);
		//return (1.0f - _percent) * _a + (_b * _percent);
	}
};