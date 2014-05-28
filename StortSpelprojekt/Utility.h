#pragma once

typedef unsigned int uint;

namespace Utility
{
	/* Generate a perlin map that is stored in result based on data in map that will generally be random
	*
	* #_result, the result of the perlin map
	* #_map, values that the perlin map should be based on (standard is random values)
	* #_sizeX, the size of both maps in x
	* #_sizeY, the size of both maps in y
	* #_startFrequency, first map will be generated at this frequency lower means more change while higher results on less change
	* #_frequency, what frequency to stop at including the frequency specified
	* #_amplitude, impacts the size of the generated map. Total size will be about 1.5 * amplitude (no garanti, and may be larger)
	* #_persistence, advanced option to specifie size of octaves generated. Warning! Will impact the size values resulting from the perlin map heavily */
	template < class T, class Y >
	void perlinNoise(T& _result, Y& _map, uint _sizeX, uint _sizeY, uint _startFrequency, uint _frequency, float _amplitude, float _persistence = 0.5f)
	{
		auto t = [&](uint x, uint y)->float&{ return _result[_sizeY*y + x]; };
		auto value = [&](uint x, uint y)->float&{ return _map[_sizeY*(y + 1) + (x + 1)]; };
		auto interpolate = [](float a, float b, float x)->float
		{
			float t = (1 - cos(x * 3.1415927f)) * .5f;
			return a*(1 - t) + b*t;
		};
		float amplitude;
		float noise;
		float temp;
		uint intX;
		uint intY;
		float fracX;
		float fracY;
		float xTemp;
		float yTemp;
		float frequency;
		uint x;
		uint y;
		float floatStartFrequency = float(_startFrequency);
		for (x = 0; x < _sizeX; x++)
		{
			for (y = 0; y < _sizeY; y++)
			{
				amplitude = _amplitude;
				noise = 0;
				for (frequency = floatStartFrequency; frequency <= _frequency; frequency /= _persistence)
				{
					temp = 1 / frequency;
					xTemp = x * temp;
					yTemp = y * temp;
					intX = uint(xTemp);
					fracX = xTemp - float(intX);

					intY = uint(yTemp);
					fracY = yTemp - float(intY);

					noise += interpolate(
						interpolate(
						value(intX + 0, intY + 0),
						value(intX + 1, intY + 0),
						fracX),
						interpolate(
						value(intX + 0, intY + 1),
						value(intX + 1, intY + 1),
						fracX),
						fracY) * amplitude;

					amplitude *= _persistence;
				}
				t(x, y) += noise;
			}
		}
	}

	/* Generate a perlin map that is stored in result based on data in map that will generally be random
	*
	* #_result, the result of the perlin map
	* #_map, values that the perlin map should be based on (standard is random values)
	* #_sizeX, the size of both maps in x
	* #_sizeY, the size of both maps in y
	* #_startFrequency, first map will be generated at this frequency lower means more change while higher results on less change
	* #_frequency, what frequency to stop at including the frequency specified
	* #_amplitude, impacts the size of the generated map. Total size will be about 1.5 * amplitude (no garanti, and may be larger)
	* #_persistence, advanced option to specifie size of octaves generated. Warning! Will impact the size values resulting from the perlin map heavily */
	template < class T, class Y >
	void perlinNoiseClouds(T& _result, Y& _map, uint _sizeX, uint _sizeY, uint _startFrequency, uint _frequency, float _amplitude, float _persistence = 0.5f)
	{
		auto t = [&](uint x, uint y)->float&{ return _result[x][y]; };
		auto value = [&](uint x, uint y)->float&{ return _map[x][y]; };
		auto interpolate = [](float a, float b, float x)->float
		{
			float t = (1 - cos(x * 3.1415927f)) * .5f;
			return a*(1 - t) + b*t;
		};
		float amplitude;
		float noise;
		uint x;
		uint y;
		float frequency;
		float temp;
		uint intX;
		uint intY;
		float fracX;
		float fracY;
		float xTemp;
		float yTemp;
		float floatStartFrequency = float(_startFrequency);
		for (x = 0; x < _sizeX; x++)
		{
			for (y = 0; y < _sizeY; y++)
			{
				amplitude = _amplitude;
				noise = 0;
				for (frequency = floatStartFrequency; frequency <= _frequency; frequency /= _persistence)
				{
					temp = 1 / frequency;
					xTemp = x * temp;
					intX = uint(xTemp);
					fracX = xTemp - float(intX);
					yTemp = y * temp;
					intY = uint(yTemp);
					fracY = yTemp - float(intY);

					noise += interpolate(
						interpolate(
						value(intX + 0, intY + 0),
						value(intX + 1, intY + 0),
						fracX),
						interpolate(
						value(intX + 0, intY + 1),
						value(intX + 1, intY + 1),
						fracX),
						fracY) * amplitude;

					amplitude *= _persistence;
				}
				t(x, y) += noise;
			}
		}
	}

	/* Generate a perlin map that is stored in result based on data in map that will generally be random
	*
	* #_result, the result of the perlin map
	* #_map, values that the perlin map should be based on (standard is random values)
	* #_sizeX, the size of both maps in x
	* #_sizeY, the size of both maps in y
	* #_startFrequency, first map will be generated at this frequency lower means more change while higher results on less change
	* #_frequency, what frequency to stop at including the frequency specified
	* #_amplitude, impacts the size of the generated map. Total size will be about 1.5 * amplitude (no garanti, and may be larger)
	* #_persistence, advanced option to specifie size of octaves generated. Warning! Will impact the size values resulting from the perlin map heavily */
	template < class T, class Y >
	void perlinNoiseSeamless(T& _result, Y& _map, uint _sizeX, uint _sizeY, uint _startFrequency, uint _frequency, uint _amplitude, float _persistence = 0.5f)
	{
		auto t = [&](uint x, uint y)->float&{ return _result[x][y].w; };
		auto value = [&](uint x, uint y)->float&{ return _map[x][y]; };
		auto interpolate = [](float a, float b, float x)->float
		{
			float t = (1 - cos(x * 3.1415927f)) * .5f;
			return a*(1 - t) + b*t;
		};
		uint x;
		uint y;
		float amplitude;
		float noise;
		float frequency;
		float floatStartFrequency = float(_startFrequency);
		float float_amplitude = float(_amplitude);
		float temp;
		uint intX;
		uint intY;
		float fracX;
		float fracY;
		float xTemp;
		float yTemp;
		for (x = 0; x < _sizeX; x++)
		{
			for (y = 0; y < _sizeY; y++)
			{
				amplitude = float_amplitude;
				noise = 0;
				for (frequency = floatStartFrequency; frequency <= _frequency; frequency /= _persistence)
				{
					temp = 1 / frequency;
					xTemp = x * temp;
					intX = uint(xTemp);
					fracX = xTemp - float(intX);
					yTemp = y * temp;
					intY = uint(yTemp);
					fracY = yTemp - float(intY);

					noise += interpolate(
						interpolate(
						value(intX + 0, intY + 0),
						value(intX + 1, intY + 0),
						fracX),
						interpolate(
						value(intX + 0, intY + 1),
						value(intX + 1, intY + 1),
						fracX),
						fracY) * amplitude;

					amplitude *= _persistence;
				}
				t(x, y) = noise;
			}
		}
	}

	template < class T >
	void erosion(T& _map, uint _sizeX, uint _sizeY, float _rainAmount, uint _iterations)
	{
		auto map = [&](uint x, uint y)->float&{ return _map[_sizeX*(y % _sizeY) + x % _sizeX]; };

		std::vector< float > water(_sizeX * _sizeY), waterMovement(_sizeX * _sizeY);
		for (uint i(0); i < water.size(); i++)
			water[i] = waterMovement[i] = 0;
		auto w = [&](uint x, uint y)->float&{ return water[(y * _sizeX) % water.size() + x % _sizeX]; };
		auto wMovement = [&](uint x, uint y)->float&{ return waterMovement[(y * _sizeX) % (water.size() - 1) + x % (_sizeX - 1)]; };

		for (uint i(0); i < _iterations; i++)
		{
			// Add water
			for (auto& f : water)
				f += _rainAmount;
			// Move water
			for (uint x(0); x < _sizeX; x++)
			{
				for (uint y(0); y < _sizeY; y++)
				{
					// Heights
					float h =			map(x - 0, y - 0) + w(x - 0, y - 0);
					float hLeft =		map(x - 1, y - 0) + w(x - 1, y - 0);
					float hRight =		map(x + 1, y - 0) + w(x + 1, y - 0);
					float hBelow =		map(x - 0, y - 1) + w(x - 0, y - 1);
					float hAbove =		map(x - 0, y + 1) + w(x - 0, y + 1);

					// Differences
					auto aboveZero = [](float x) { return x > 0 ? x : 0; };
					float dLeft = aboveZero( h - hLeft );
					float dRight = aboveZero( h - hRight );
					float dBelow = aboveZero( h - hBelow );
					float dAbove = aboveZero( h - hAbove );

					// Water movement
					float procentWaterMovement = 0.1f;
					float wLeft = dLeft * procentWaterMovement;
					float wRight = dLeft * procentWaterMovement;
					float wBelow = dBelow * procentWaterMovement;
					float wAbove = dAbove * procentWaterMovement;

					if (wLeft + wRight + wBelow + hAbove < w(x, y))
					{
						wMovement(x - 0, y - 0) -= wLeft + wRight + wBelow + hAbove;
						wMovement(x - 1, y - 0) += wLeft;
						wMovement(x + 1, y - 0) += wRight;
						wMovement(x - 0, y - 1) += wBelow;
						wMovement(x - 0, y + 1) += wAbove;
					}
					else
					{
						float total = wLeft + wRight + wBelow + hAbove;
						wMovement(x - 1, y - 0) += wLeft / total * w(x, y);
						wMovement(x + 1, y - 0) += wRight / total * w(x, y);
						wMovement(x - 0, y - 1) += wBelow / total * w(x, y);
						wMovement(x - 0, y + 1) += wAbove / total * w(x, y);
						wMovement(x - 0, y - 0) -= w(x, y);
					}
				}
			}
			// Apply erosion
			for (uint x(0); x < _sizeX; x++)
			{
				for (uint y(0); y < _sizeY; y++)
				{
					w(x, y) += wMovement(x, y);
					map(x, y) -= wMovement(x, y) * 0.1f;
					wMovement(x, y) = 0;
				}
			}
		}
	}

	template < class T >
	glm::vec3 mapToTerrain(glm::vec2 _v, T& terrain)
	{
		// follow the terrain
		bool visible = false;
		glm::vec3 terrainpos;
		float indexX = -1.f, indexZ = -1.f;
		int heightID = -1;
		int width = 0;
		int qSize = 0; // the QUADSIZE for the level where the Enemy is found
		// should flip these.. because if not in large, it cannot be in the others..
		for (int i = 0; !visible && i < 9; i++)
		{
			terrainpos = terrain.tiny[i].position;
			if(terrainpos.x <= _v.x && terrainpos.x + terrain.tinySize > _v.x &&
			   terrainpos.z <= _v.y && terrainpos.z + terrain.tinySize > _v.y)
			{
				width = terrain.tiny[i].size_x;
				visible = true;
				qSize = QUADSIZE;
				indexX = ((_v.x - terrainpos.x) / qSize);
				indexZ = ((_v.y - terrainpos.z) / qSize);
				heightID = terrain.tiny[i].heightsID;
				break;
			}
		}
		for (int i = 0; !visible && i < 9; i++)
		{
			terrainpos = terrain.medium[i].position;
			if(terrainpos.x <= _v.x && terrainpos.x + terrain.mediumSize > _v.x &&
			   terrainpos.z <= _v.y && terrainpos.z + terrain.mediumSize > _v.y)
			{
				width = terrain.medium[i].size_x;
				visible = true;
				qSize = QUADSIZE * 3;
				indexX = ((_v.x - terrainpos.x) / qSize);
				indexZ = ((_v.y - terrainpos.z) / qSize);
				heightID = terrain.medium[i].heightsID;
				break;
			}
		}
		for (int i = 0; !visible && i < 9; i++)
		{
			terrainpos = terrain.large[i].position;
			if(terrainpos.x <= _v.x && terrainpos.x + terrain.largeSize > _v.x &&
			   terrainpos.z <= _v.y && terrainpos.z + terrain.largeSize > _v.y)
			{
				width = terrain.large[i].size_x;
				visible = true;
				qSize = QUADSIZE * 9;
				indexX = ((_v.x - terrainpos.x) / qSize);
				indexZ = ((_v.y - terrainpos.z) / qSize);
				heightID = terrain.large[i].heightsID;
				break;
			}
		}

		int indexBottomLeft = (int)indexX + (int)indexZ * width;
		if (indexBottomLeft < 0 && !visible)
			return glm::vec3(_v.x, 0, _v.y);

		int indexTopLeft = indexBottomLeft + width;

		float h0 = terrain.heights[heightID].at(indexBottomLeft % (CHUNKSIZE * CHUNKSIZE));
		float h1 = terrain.heights[heightID].at((indexBottomLeft + 1) % (CHUNKSIZE * CHUNKSIZE));
		float h2 = terrain.heights[heightID].at(indexTopLeft % (CHUNKSIZE * CHUNKSIZE));
		float h3 = terrain.heights[heightID].at((indexTopLeft + 1) % (CHUNKSIZE * CHUNKSIZE));

		float height = 0.f;
		float sqX = indexX - (int)indexX;
		float sqZ = indexZ - (int)indexZ;
		if (sqX + sqZ < 1)
		{
			height = h0;
			height += (h1 - h0)*sqX;
			height += (h2 - h0)*sqZ;
		}
		else
		{
			height = h3;
			height += (h1 - h3)*(1.f - sqZ);
			height += (h2 - h3)*(1.f - sqX);
		}

		return glm::vec3(_v.x, height, _v.y);
	}

	/* Forces the function to look in only the specified type of chunk (tiny, medium or large). Will return input value if position is outside chunks */
	template < class T >
	float mapToTerrainForced(glm::vec2 _v, T& terrain, int _level)
	{
		auto &chunks = _level == 1 ? terrain.tiny : _level == 2 ? terrain.medium : terrain.large;

		glm::vec3 terrainpos;
		float indexX = -1.f, indexZ = -1.f;
		int qSize = (int)(QUADSIZE * pow(3, _level - 1)), heightID;
		int terrainSize = (int)(terrain.tinySize * pow(3, _level - 1));
		
		for(int i = 0; i < 9; i++)
		{
			terrainpos = chunks[i].position;
			if(terrainpos.x <= _v.x && terrainpos.x + terrainSize > _v.x && terrainpos.z <= _v.y && terrainpos.z + terrainSize > _v.y)
			{
				indexX = ((_v.x - terrainpos.x) / qSize);
				indexZ = ((_v.y - terrainpos.z) / qSize);
				heightID = chunks[i].heightsID;
				break;
			}
		}

		if(indexX == -1.f)
			return 0.f;

		int indexBottomLeft = (int)indexX + (int)indexZ * chunks[0].size_x;

		int indexTopLeft = indexBottomLeft + chunks[0].size_x;

		float h0 = terrain.heights[heightID].at(indexBottomLeft % (CHUNKSIZE * CHUNKSIZE));
		float h1 = terrain.heights[heightID].at((indexBottomLeft + 1) % (CHUNKSIZE * CHUNKSIZE));
		float h2 = terrain.heights[heightID].at(indexTopLeft % (CHUNKSIZE * CHUNKSIZE));
		float h3 = terrain.heights[heightID].at((indexTopLeft + 1) % (CHUNKSIZE * CHUNKSIZE));

		float height = 0.f;
		float sqX = indexX - (int)indexX;
		float sqZ = indexZ - (int)indexZ;
		if(sqX + sqZ < 1)
		{
			height = h0;
			height += (h1 - h0) * sqX;
			height += (h2 - h0) * sqZ;
		}
		else
		{
			height = h3;
			height += (h1 - h3) * (1.f - sqZ);
			height += (h2 - h3) * (1.f - sqX);
		}

		return height + chunks[0].position.y;
	}

	//template < class T >
	//SingleBiom getBlendMapValue(T *_terrain, glm::vec3 _position)
	//{
	//	SingleBiom out;
	//	int blendSize = BLENDTEXSIZE * 3 + 1;
	//	float biomStep = 9 * (CHUNKSIZE / BLENDTEXSIZE) * QUADSIZE;
	//
	//	//glm::vec3 startPos = _position - glm::vec3(abs(_terrain->largeActive[0].position.x), 0, abs(_terrain->largeActive[0].position.z));
	//	glm::vec3 startPos = _position - _terrain->largeActive[0].position;
	//
	//	int x = (int)(startPos.x / biomStep + 0.5f);
	//	int y = (int)(startPos.z / biomStep + 0.5f);
	//
	//	if (x < 0)
	//		x = 0;
	//	if (y < 0)
	//		y = 0;
	//
	//	out.map1 = _terrain->biomMap.map1.at(x + y * blendSize);
	//	out.map2 = _terrain->biomMap.map2.at(x + y * blendSize);
	//	return out;
	//}

	template < typename T >
	float getClosestBiomPoint(T *_biomsPoints, glm::vec3 _position)
	{
		glm::vec2 pos = glm::vec2(_position.x, _position.z);
		float minLen = D3D11_FLOAT32_MAX;

		for(uint i = 0; i < _biomsPoints->size(); i++)
		{
			float temp = glm::length(pos - _biomsPoints->at(i));
			if(temp < minLen)
				minLen = temp;
		}
		


		return minLen;
	}

	template < class T >
	SingleBiom getBlendMapValue(T *_bioms, glm::vec3 _position)
	{
		SingleBiom out;
		std::vector<float> lengths(NBIOMS);

		lengths.at(0) = getClosestBiomPoint(&_bioms->desertPoints, _position);
		lengths.at(1) = getClosestBiomPoint(&_bioms->mountainPoints, _position);
		lengths.at(2) = getClosestBiomPoint(&_bioms->meadowPoints, _position);
		lengths.at(3) = getClosestBiomPoint(&_bioms->swampPoints, _position);
		lengths.at(4) = getClosestBiomPoint(&_bioms->forestPoints, _position);

		for(int i = 0; i < NBIOMS; i++)
		{
			bool b = true;
			std::vector<float> lens;
			for(int j = 0; j < NBIOMS; j++)
			{
				if(j == i)
					continue;
				if(lengths.at(j) < lengths.at(i) - 5000)
				{
					i > 3 ? out.map2[i - 4] = 0.f : out.map1[i] = 0.f;
					b = false;
					break;
				}
				if(abs(lengths.at(i) - lengths.at(j)) < 5000)
					lens.push_back(lengths.at(j));
			}
			if(!b)
				continue;
			if(lens.size() == 0)
			{
				i > 3 ? out.map2[i - 4] = 1.f : out.map1[i] = 1.f;
				continue;
			}

			float min = lens.at(0);
			for(uint k = 1; k < lens.size(); k++)
			{
				if(lens.at(k) < min)
					min = lens.at(k);
			}

			float val = 0.5f + (abs(lengths.at(i) - min) / 10000) * (lengths.at(i) < min ? 1 : -1);

			i > 3 ? out.map2[i - 4] = val : out.map1[i] = val;
		}

		float len = 1.f / (glm::length(out.map1 + out.map2));
		out.map1 *= len;
		out.map2 *= len;

		return out;
	}

	/* Returns the length of x */
	template < class T > T length(T x) { return x > 0 ? x : -x; }

	/* */
	template < class T, class Function > void map(T& t, Function f)
	{
		map(t.begin(), t.end(), f);
	}
	template < class Iterator, class Function > void map(Iterator first, Iterator last, Function f)
	{
		Iterator i(first);
		while(i != last)
			*(i++) = f(i - first);
	}

	/* */
	template < class T, class Function > typename T::value_type fold(T& t, Function f)
	{
		return fold< T >(t.begin(), t.end(), f);
	}
	template < class T, class Iterator, class Function > typename T::value_type fold(Iterator first, Iterator last, Function f)
	{
		typename T::value_type result(0);
		while(first != last)
			result = f(result, *(first++));
		return result;
	}

	/* */
	template < class T, class Function > void func(T& t, Function f)
	{
		func(t.begin(), t.end(), f);
	}
	template < class Iterator, class Function > void func(Iterator first, Iterator last, Function f)
	{
		Iterator i(first);
		while(i != last)
			f(i++ - first);
	}
}