#pragma once

#include "NetworkAspect.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <array>
#include "MainState.h"
#pragma comment(lib, "user32.lib")

class MultiplayerAspect : public NetworkAspect
{
public:
	MultiplayerAspect();
	~MultiplayerAspect();

	void runAspect(float _dt)override;
	void init(MainState& _mainState)override;

	/* Tries to connect to ClientIPC
	 * #returns true is succesfull, false otherwise */
	bool connect();

	/* See NetworkAspect for documentation for functions*/
	void damage(uint _i, unsigned short _type, float _damage)override;
	void died()override;
private:
	template < class T > T& read(LPVOID _buffer)
	{
		bufferPointer += sizeof(T);
		return *(reinterpret_cast<T*>(static_cast<char*>(_buffer)+bufferPointer - sizeof(T)));
	}
	template < class T > void write(LPVOID _buffer, T _t)
	{
		bufferPointer += sizeof(T);
		*(reinterpret_cast<T*>(static_cast<char*>(_buffer)+bufferPointer - sizeof(T))) = _t;
	}

	bool rotateAndMove(int _i, float _dt, bool _isServerPackage = false);
	void handleOponentDisconnect(int _outdatedID);
	void saveID(unsigned long long _lower, unsigned long long _upper);
	bool getID(std::pair<unsigned long long, unsigned long long>& _id);

	std::array<float, 10> animationTimer;

	typedef unsigned int long long id;

	int nPlayers;
	std::array<float, 10> idleFreq, runningFreq, extractionFreq;
	std::array<glm::vec3, 10> idealPos, lastIdealPos;
	std::array<bool, 10> lastFrameMoving;
	std::array<bool, 10> lastFrameFalling;

	HANDLE fileMapToIPC;
	HANDLE fileMapFromIPC;
	HANDLE fileMapStatic;
	LPVOID bufferToIPC;
	LPVOID bufferFromIPC;
	LPVOID bufferStatic;
	uint bufferSize;
	uint bufferPointer;

	id idLower, idUpper;

	MainState *mainState;
};