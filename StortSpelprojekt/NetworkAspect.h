#pragma once

#include "AspectInterface.h"

class NetworkAspect : public AspectInterface
{
public:
	NetworkAspect() : AspectInterface("network", 10), c(std::make_pair(Containers::PLAYER, MODIFY), std::make_pair(Containers::ENEMY, MODIFY), std::make_pair(Containers::BIOMS, MODIFY)),
		player(*c.get<PlayerContainer>(Containers::PLAYER)),
		enemyContainer(*c.get<EnemyContainer>(Containers::ENEMY)) {}
	virtual ~NetworkAspect() {}

	virtual void runAspect(float _dt) = 0;

	/* _type is 1 for player and 2 for energy */
	virtual void damage(uint _i, unsigned short _type, float _damage) = 0;

	/* Call when player die */
	virtual void died() = 0;

protected:
	Containers c;
	//TerrainContainer	&terrain;
	PlayerContainer		&player;
	EnemyContainer		&enemyContainer;
};