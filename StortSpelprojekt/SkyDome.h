#pragma once

#include "RenderInterface.h"

class SkyDome
{
public:
	SkyDome(/*float _radius = 1.0f*/);
	~SkyDome();

	/*	create skydome buffer
	
		#_rings: number of rings that comprise the dome
		#_segments: number of vertices that comprise each ring
		
		#returns
			true if vertex- & index-buffer are created
			false if buffer creation fails */
	bool init(int _rings, int _segments, RenderInterface* _graphics);

	/*	draw the skydome
	
		#_graphics: renderInterface */
	void draw(RenderInterface* _graphics);

	/*	get vertex index */
	int getVIndex();

	/*	get indices index */
	int getBIndex();

	/*	get amount of indices */
	int getNumIndices();

private:
	int		nIndex;
	int		vIndex;
	int		bIndex;
	float	radius;
};
