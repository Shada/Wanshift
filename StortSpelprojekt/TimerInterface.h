#pragma once

/* Interface for high resolution timer implementations
	- measure elapsed time
*/
class TimerInterface
{
public:
	TimerInterface();
	~TimerInterface();

	/*	Update timer one frame
		- call this once every frame/intervall */
	virtual void	frame()		=0;
	/*	Get delta time for frame */
	virtual float	getTime()	=0;
};