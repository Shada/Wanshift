#include "MultiplayerAspect.h"
#include "ReaderINI.h"
#include "Utility_INI.h"
#include <Shlwapi.h>
#include <Shlobj.h>
#include <iostream>
#include <fstream>
#include "Utility.h"

MultiplayerAspect::MultiplayerAspect()
{
	bufferToIPC = NULL;
	bufferFromIPC = NULL;
	bufferStatic = NULL;
	fileMapToIPC = NULL;
	fileMapFromIPC = NULL;
	fileMapStatic = NULL;
}

MultiplayerAspect::~MultiplayerAspect()
{
	if(bufferToIPC != NULL)
		UnmapViewOfFile(bufferToIPC);
	if(bufferToIPC != NULL)
		UnmapViewOfFile(bufferFromIPC);
	if(bufferToIPC != NULL)
		UnmapViewOfFile(bufferStatic);

	if(fileMapToIPC != NULL)
		CloseHandle(fileMapToIPC);
	if(fileMapFromIPC != NULL)
		CloseHandle(fileMapFromIPC);
	if(fileMapStatic != NULL)
		CloseHandle(fileMapStatic);
}

void MultiplayerAspect::init(MainState& _mainState)
{
	Containers containers(Containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, MODIFY), std::make_pair(Containers::ENEMY,MODIFY)));
	PlayerContainer& player(*containers.get<PlayerContainer>(Containers::PLAYER));
	//TerrainContainer &terrain(*containers.get<TerrainContainer>(Containers::TERRAIN));
	mainState = &_mainState;

	for(int i = 0; i < 10; i++)
	{
		player.playersAnimationState[i] = aIDLE;
		player.playersAnimationFrame[i] = 0;
		player.playersPrevAnimationFrame[i] = 0;
		player.playersAnimationInterpolation[i] = 0;
		animationTimer[i] = 0.f;
		lastFrameMoving[i] = false;
		idleFreq[i] = IDLEFREQ;
		runningFreq[i] = RUNNINGFREQ;
		extractionFreq[i] = EXTRACTFREQ;
	}
}


unsigned int identifyPointer = sizeof(bool)* 6;
unsigned int identifySize = sizeof(uint8_t)* 16 + sizeof(float)* 2 + sizeof(float)* 5 + sizeof(unsigned long);

unsigned int updatePointer = identifyPointer + identifySize;
unsigned int updateSize = sizeof(float)* 3 + sizeof(float)* 2 + sizeof(unsigned short)+sizeof(float)+sizeof(float)* 5;

unsigned int serverGameUpdatePointer = updatePointer + updateSize;
unsigned int serverGameUpdateSize = sizeof(float)+sizeof(bool)+(sizeof(uint8_t)* 16 + sizeof(float)* 3 + sizeof(float)* 2 + sizeof(unsigned short)+sizeof(float)) * 10;

unsigned int energyUpdatePointer = serverGameUpdatePointer + serverGameUpdateSize;
unsigned int energyUpdateSize = (sizeof(uint8_t)* 16 + sizeof(float)* 3) * 20;

unsigned int entityInteractionPointer = energyUpdatePointer + energyUpdateSize;
unsigned int entityInteractionSize = 0;

unsigned int entityInteractionResponsePointer = entityInteractionPointer + entityInteractionSize;
unsigned int entityInteractionResponseSize = sizeof(unsigned int)+(sizeof(uint8_t)* 16 + sizeof(float)* 3) * 20 + sizeof(unsigned int)+(sizeof(float)* 4) * 10 + sizeof(int);

unsigned int bufferStaticSize = entityInteractionResponsePointer + entityInteractionResponseSize;


bool MultiplayerAspect::connect()
{
#define OPEN_FILE_MAPPING(FILE_MAP, BUFFER, NAME, BUFFER_SIZE) \
	FILE_MAP = OpenFileMapping(\
	FILE_MAP_ALL_ACCESS,   /* read/write access */ \
	FALSE,                 /* do not inherit the name */ \
	TEXT(NAME));    /* name of mapping object */ \
	\
	if(FILE_MAP == NULL) \
	{ \
	auto t = __HRESULT_FROM_WIN32(GetLastError()); \
	return false; \
} \
	\
	BUFFER = MapViewOfFile(FILE_MAP, /* handle to map object */ \
	FILE_MAP_ALL_ACCESS,  /* read/write permission */ \
	0, \
	0, \
	BUFFER_SIZE); \
	\
	if(BUFFER == NULL) \
	{ \
	auto t = __HRESULT_FROM_WIN32(GetLastError()); \
	CloseHandle(FILE_MAP); \
	return false; \
}

	OPEN_FILE_MAPPING(fileMapToIPC, bufferToIPC, "ClientIPC - messages - ToIPC", 1024);
	OPEN_FILE_MAPPING(fileMapFromIPC, bufferFromIPC, "ClientIPC - messages - FromIPC", 1024);
	OPEN_FILE_MAPPING(fileMapStatic, bufferStatic, "ClientIPC - static", bufferStaticSize);

#undef OPEN_FILE_MAPPING


	// Identify packet
	bufferPointer = sizeof(bool);
	bool t = read<bool>(bufferStatic);
	if(t)
	{
		SingleBiom b;
		Containers containers(Containers(std::make_pair(Containers::TERRAIN, MODIFY)));
		TerrainContainer& terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN));

		// Read from buffer
		bufferPointer = identifyPointer;
		idLower = read<id>(bufferStatic);
		idUpper = read<id>(bufferStatic);
		float x = read<float>(bufferStatic); // spawn position x
		float z = read<float>(bufferStatic); // spawn position z
		b.map1.x = read<float>(bufferStatic); // Biom affinity 1
		b.map1.y = read<float>(bufferStatic); // Biom affinity 2
		b.map1.z = read<float>(bufferStatic); // Biom affinity 3
		b.map1.w = read<float>(bufferStatic); // Biom affinity 4
		b.map2.x = read<float>(bufferStatic); // Biom affinity 5
		terrainContainer.worldSeed = read<unsigned long>(bufferStatic); // World seed

		player.position = glm::vec3(x, 0, z);
		player.adaptation1 = b.map1;
		player.adaptation2 = b.map2;

		bufferPointer = sizeof(bool);
		write<bool>(bufferStatic, false);

		player.id = std::make_pair(idLower, idUpper);

		//Write ID to INI file!
		saveID(idLower, idUpper);
	}

	return true;
}

void MultiplayerAspect::runAspect(float _dt)
{
	_dt /= 1000;

	// Client update packet
	bufferPointer = 0;
	bool t = read<bool>(bufferStatic);
	if(!t)
	{
		// Write to buffer
		bufferPointer = updatePointer;
		write<float>(bufferStatic, player.position.x); //x
		write<float>(bufferStatic, player.position.y); //y
		write<float>(bufferStatic, player.position.z); //z
		write<float>(bufferStatic, player.rotation.x); //rotX
		write<float>(bufferStatic, player.rotation.y); //rotZ
		write<unsigned short>(bufferStatic, (unsigned short)player.animationState); //action
		write<float>(bufferStatic, player.energyInfo.energyAmount); //energy
		write<float>(bufferStatic, 0.f); // biomAffinity 1
		write<float>(bufferStatic, 0.f); // biomAffinity 2
		write<float>(bufferStatic, 0.f); // biomAffinity 3
		write<float>(bufferStatic, 0.f); // biomAffinity 4
		write<float>(bufferStatic, 0.f); // biomAffinity 5

		bufferPointer = 0;
		write<bool>(bufferStatic, true);
	}

	// Identify packet
	bufferPointer = sizeof(bool);
	t = read<bool>(bufferStatic);
	if(t)
	{
		Containers containers(Containers(std::make_pair(Containers::TERRAIN, MODIFY)));
		TerrainContainer& terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN));
	
		// Read from buffer
		bufferPointer = identifyPointer;
		idLower = read<id>(bufferStatic);
		idUpper = read<id>(bufferStatic);
		read<float>(bufferStatic); // spawn position x
		read<float>(bufferStatic); // spawn position y
		read<float>(bufferStatic); // Biom affinity 1
		read<float>(bufferStatic); // Biom affinity 2
		read<float>(bufferStatic); // Biom affinity 3
		read<float>(bufferStatic); // Biom affinity 4
		read<float>(bufferStatic); // Biom affinity 5
		terrainContainer.worldSeed = read<unsigned long>(bufferStatic); // World seed
	
		bufferPointer = sizeof(bool);
		write<bool>(bufferStatic, false);
	
		player.id = std::make_pair(idLower, idUpper);
	
		//Write ID to INI file!
		saveID(idLower, idUpper);
	}

	// Server update packet
	bufferPointer = sizeof(bool)* 2;
	t = read<bool>(bufferStatic);
	if(t)
	{
		// Read from buffer
		bufferPointer = serverGameUpdatePointer;

		float time = read<float>(bufferStatic);
		bool increasing = read<bool>(bufferStatic);

		mainState->cbContainer->cbEveryFrame.timeofDay = time;
		mainState->cbContainer->cbEveryFrame.dayBool.x = increasing;


		int p = -1;
		for(int i = 0; i < 10; i++)
		{
			id lower = read<id>(bufferStatic);
			id upper = read<id>(bufferStatic);

			float newX, newY, newZ;
			newX = read<float>(bufferStatic);
			newY = read<float>(bufferStatic);
			newZ = read<float>(bufferStatic);
			float rotX = read<float>(bufferStatic);
			float rotY = read<float>(bufferStatic);
			unsigned short action = read<unsigned short>(bufferStatic);
			float energy = read<float>(bufferStatic);

			if(player.playersId[i] != std::make_pair(lower, upper))
				handleOponentDisconnect(i);

			player.playersId[i] = std::make_pair(lower, upper);

			if(lower != idLower && upper != idUpper && lower != 0 && upper != 0)
			{
				p++;

				player.playersEnergy[p] = energy;
				
				//lastFrameMoving[p] = abs(newX - player.playersPosition[p].x) < 50.f * _dt || abs(newX - player.playersPosition[p].z) < 50.f * _dt;
				//lastFrameMoving[p] = newX != idealPos[p].x || newZ != idealPos[p].z;
				idealPos[p] = glm::vec3(newX, newY, newZ);
				//player.playersPosition[p].y = newY;

				//if(!lastFrameMoving[p])
				//	player.playersPosition[p] = idealPos[p];
				//lastFrameFalling[p] = newY != player.playersPosition[p].y;

				animationTimer[p] += _dt;
				if(player.playersAnimationState[p] != (AnimationState)action)
				{
					player.playersAnimationState[p] = (AnimationState)action;
					player.playersAnimationFrame[p] = 0;
					player.playersPrevAnimationFrame[p] = 0;
					player.playersAnimationInterpolation[p] = 0.f;
					animationTimer[p] = 0.f;
				}
				else
					player.animateModel(player.playersId[p], player.playersAnimationState[p], player.playersAnimationFrame[p], player.playersPrevAnimationFrame[p], animationTimer[p], player.playersAnimationInterpolation[p]);

				if(player.playersRotation[p].x != rotX && rotX != 0 && player.playersRotation[p].y != rotY && rotY != 0)
					player.playersRotation[p] = glm::vec2(rotX, rotY);

				if(!rotateAndMove(p, _dt, true))
					continue;

				Containers containers(Containers(std::make_pair(Containers::TERRAIN, MODIFY)));
				TerrainContainer& terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN));
				player.playersPosition[p].y = Utility::mapToTerrain(glm::vec2(player.playersPosition[p].x, player.playersPosition[p].z), terrainContainer).y + 15;

				//float diffX = newX - player.playersPosition[p].x;
				//float diffZ = newZ - player.playersPosition[p].z;
				//
				//float diff = glm::length(glm::vec2(diffX, diffZ));
				//if(diff == 0.f)
				//	continue;
				//else if(diff > 200.f)
				//{
				//	player.playersPosition[p] = idealPos[p];
				//	continue;
				//}
				//
				//glm::vec2 rot = glm::normalize(glm::vec2(diffX, diffZ));
				//
				//if(diff < 85.f * _dt * 2.f)
				//	player.playersPosition[p] = idealPos[p];
				//else
				//{
				//	if(diff > 85.f * 1.2f * _dt)
				//		diff = 85.f * 1.2f * _dt;
				//
				//	player.playersPosition[p] += rot * diff;
				//}
				//
				//if(rotX != 0 || rotY != 0)
				//{
				//	//playersRotDegree
				//	float dot = glm::dot(glm::vec2(rotX, rotY), glm::vec2(0, 1)); // = rot.y
				//	float cosa = dot / (glm::length(glm::vec2(rotX, rotY)) * glm::length(glm::vec2(0, 1))); // = rot.y / length(rot)
				//	float t0 = std::acos(cosa);
				//
				//	t0 *= rotX / abs(rotX);
				//
				//	player.playersRotDegree[p] = t0;
				//}
			}
		}

		nPlayers = p;
		bufferPointer = sizeof(bool)* 2;
		write<bool>(bufferStatic, false);
	}
	else
	{
		int p = -1;
		for(int i = 0; i < 10; i++)
		{
			auto id = player.playersId[i];
			if(id.first == idLower && id.second == idUpper || id.first == 0 && id.second == 0)
				continue;

			p++;

			animationTimer[p] += _dt;
			player.animateModel(player.playersId[p], player.playersAnimationState[p], player.playersAnimationFrame[p], player.playersPrevAnimationFrame[p], animationTimer[p], player.playersAnimationInterpolation[p]);

			//if(lastFrameMoving[p])// && idealPos[p].x != player.playersPosition[p].x && idealPos[p].z != player.playersPosition[p].z)
			//{
			//	idealPos[p].x += player.playersRotation[p].x * 85.f * _dt;
			//	idealPos[p].z += player.playersRotation[p].y * 85.f * _dt;
			//}
			//else
			//	continue;

			if(!rotateAndMove(p, _dt))
				continue;

			Containers containers(Containers(std::make_pair(Containers::TERRAIN, MODIFY)));
			TerrainContainer& terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN));
			player.playersPosition[p].y = Utility::mapToTerrain(glm::vec2(player.playersPosition[p].x, player.playersPosition[p].z), terrainContainer).y + 15;

			//float diffX = idealPos[p].x - player.playersPosition[p].x;
			//float diffZ = idealPos[p].z - player.playersPosition[p].z;
			//
			//float diff = glm::length(glm::vec2(diffX, diffZ)) * _dt;
			//if(diff == 0.f)
			//	continue;
			//
			////if(diffZ > 75.f * 1.2f) diffZ = 1.2f;
			//
			//glm::vec2 rot = glm::normalize(glm::vec2(diffX, diffZ));
			////player.playersRotation[p] = glm::vec2(rotX, rotY);
			//
			//if(diff < 85.f * _dt * 2.f)
			//	player.playersPosition[p] = idealPos[p];
			//else
			//{
			//	if(diff > 85.f * 1.2f * _dt)
			//		diff = 85.f * 1.2f * _dt;
			//
			//	player.playersPosition[p] += rot * diff;
			//}
			//
			//if(rot.x != 0 || rot.y != 0)
			//{
			//	//playersRotDegree
			//	float t0 = std::acos(rot.y / glm::length(rot)) * rot.x / abs(rot.x);
			//
			//	player.playersRotDegree[p] = t0;
			//}
		}
	}


	// Energy update packet
	bufferPointer = sizeof(bool)* 3;
	t = read<bool>(bufferStatic);
	if(t)
	{
		// Read from buffer
		bufferPointer = energyUpdatePointer;
		for(int i(0); i < 20; i++)
		{
			id lower = read<id>(bufferStatic);
			id upper = read<id>(bufferStatic);
			player.energysId[i] = std::make_pair(lower, upper);
			player.energysPos[i].x = read<float>(bufferStatic); // pos X
			player.energysPos[i].y = read<float>(bufferStatic); // pos Y
			player.energysPos[i].z = 0.0f;
			player.energysEnergy[i] = read<float>(bufferStatic); // energy
		}

		bufferPointer = sizeof(bool)* 3;
		write<bool>(bufferStatic, false);
	}

	// Entity Interaction Response
	bufferPointer = sizeof(bool)* 5;
	t = read<bool>(bufferStatic);
	if(t)
	{
		// Read from buffer
		bufferPointer = entityInteractionResponsePointer;

		uint size = read<uint>(bufferStatic);
		for(uint i(0); i < size; i++)
		{
			id lower = read<id>(bufferStatic);
			id upper = read<id>(bufferStatic);

			float x = read<float>(bufferStatic); // pos X
			float y = read<float>(bufferStatic); // pos Y
			float e = read<float>(bufferStatic); // energy

			for(int l(0); l < 10; l++)
			{
				// test if id of energy entity is equal to lower and upper
				if(player.energysId[i] == std::make_pair(lower, upper))
				{
					player.energysPos[i].x = x;
					player.energysPos[i].y = y;
					player.energysPos[i].z = 0.0f;
					player.energysEnergy[i] = e;
					break;
				}
			}
		}
		size = read<uint>(bufferStatic);
		for(uint i(0); i < size; i++)
		{
			enemyContainer.pos[i].x = read<float>(bufferStatic); // pos x
			enemyContainer.pos[i].y = 0; // pos x
			enemyContainer.pos[i].z = read<float>(bufferStatic); // pos z

			enemyContainer.targetPos[i].x = read<float>(bufferStatic); // target pos x
			enemyContainer.targetPos[i].y = 0; // target pos x
			enemyContainer.targetPos[i].z = read<float>(bufferStatic); // target pos z

			enemyContainer.action[i] = read<int>(bufferStatic);
		}

		bufferPointer = sizeof(bool)* 5;
		write<bool>(bufferStatic, false);
	}
}

bool MultiplayerAspect::rotateAndMove(int _i, float _dt, bool _isServerPackage)
{
	float diffX = idealPos[_i].x - player.playersPosition[_i].x;
	float diffZ = idealPos[_i].z - player.playersPosition[_i].z;

	float diff = glm::length(glm::vec2(diffX, diffZ)) * _dt;

	if(diff == 0.f)
		return false;
	if(_isServerPackage && diff > 200.f)
	{
		player.playersPosition[_i] = idealPos[_i];
		return false;
	}

	glm::vec2 rot = glm::normalize(glm::vec2(diffX, diffZ));

	//if(diff < 85.f * _dt * 2.f)
	//	player.playersPosition[_i] = idealPos[_i];
	//else
	{
		if(diff > 85.f * 1.2f * _dt)
			diff = 85.f * 1.2f * _dt;

		player.playersPosition[_i].x += rot.x * diff;
		player.playersPosition[_i].z += rot.y * diff;
		float diffY = idealPos[_i].y - player.playersPosition[_i].y;
		player.playersPosition[_i].y += diffY * diff;
	}

	if(rot.x != 0 || rot.y != 0)
	{
		//playersRotDegree
		player.playersRotDegree[_i] = std::acos(rot.y / glm::length(rot)) * rot.x / abs(rot.x);
	}

	return true;
}

void MultiplayerAspect::handleOponentDisconnect(int _outdatedID)
{
	Containers containers(Containers(std::make_pair(Containers::PLAYER, READ)));
	PlayerContainer& player(*containers.get<PlayerContainer>(Containers::PLAYER));

	for(int j = _outdatedID; j < 9; j++)
	{
		player.playersId[j] = player.playersId[j + 1];
		player.playersEnergy[j] = player.playersEnergy[j + 1];
		player.playersRotation[j] = player.playersRotation[j + 1];
		player.playersPosition[j] = player.playersPosition[j + 1];
		player.playersRotDegree[j] = player.playersRotDegree[j + 1];
		player.playersAnimationState[j] = player.playersAnimationState[j + 1];
		player.playersAnimationFrame[j] = player.playersAnimationFrame[j + 1];
		player.playersPrevAnimationFrame[j] = player.playersPrevAnimationFrame[j + 1];
		player.playersAnimationInterpolation[j] = player.playersAnimationInterpolation[j + 1];

		idealPos[j] = idealPos[j + 1];
		animationTimer[j] = animationTimer[j + 1];
		lastFrameMoving[j] = lastFrameMoving[j + 1];
	}

	player.playersEnergy[9] = 0.f;
	player.playersRotation[9] = glm::vec2();
	player.playersPosition[9] = glm::vec3();
	player.playersRotDegree[9] = 0.f;
	player.playersAnimationState[9] = aIDLE;
	player.playersAnimationFrame[9] = 0;
	player.playersPrevAnimationFrame[9] = 0;
	player.playersAnimationInterpolation[9] = 0.f;
	player.playersId[9] = std::make_pair(0, 0);

	idealPos[9] = glm::vec3();
	animationTimer[9] = 0.f;
	lastFrameMoving[9] = false;
}

void MultiplayerAspect::damage(uint _i, unsigned short _type, float _damage)
{
	Containers containers(Containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, MODIFY)));
	PlayerContainer& player(*containers.get<PlayerContainer>(Containers::PLAYER));

	// Entity Interaction
	bufferPointer = sizeof(bool)* 4;
	bool t = read<bool>(bufferStatic);
	if(!t)
	{
		// Write to buffer
		bufferPointer = entityInteractionPointer;

		write<id>(bufferStatic, player.playersId[_i].first); // lower id
		write<id>(bufferStatic, player.playersId[_i].second); // upper id
		write<unsigned short>(bufferStatic, _type); // entityType
		write<float>(bufferStatic, _damage); // damage

		bufferPointer = sizeof(bool)* 4;
		write<bool>(bufferStatic, true);
	}
	else
	{
		// Read from buffer
		bufferPointer = entityInteractionPointer;
		id lower = read<id>(bufferStatic);
		id upper = read<id>(bufferStatic);
		unsigned short type = read<unsigned short>(bufferStatic);
		float damage = read<float>(bufferStatic);

		// Write to buffer
		bufferPointer = entityInteractionPointer;

		write<id>(bufferStatic, player.playersId[_i].first); // lower id
		write<id>(bufferStatic, player.playersId[_i].second); // upper id
		write<unsigned short>(bufferStatic, _type); // entityType

		if(player.playersId[_i] == std::make_pair(lower, upper))
			write<float>(bufferStatic, damage + _damage); // damage
		else
			write<float>(bufferStatic, _damage); // damage
	}
}

void MultiplayerAspect::died()
{
}

void MultiplayerAspect::saveID(unsigned long long _lower, unsigned long long _upper)
{
#ifdef _DEBUG

	std::string path = "D:\\SourceTree\\WanShift\\StortSpelprojekt\\Init\\user.ini";

#else

	TCHAR szPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	std::string path = szPath;
	path = path + "\\Wanshift\\user.ini";

#endif	

	std::ifstream in;

	in.open(path, std::ios::in);

	if (!in.is_open())
		return;

	std::string line;
	std::vector<std::string> file = std::vector<std::string>();

	while (!in.eof())
	{
		line.clear();
		std::getline(in, line);
		file.push_back(line);
	}

	in.close();

	std::string toFile = "mpID\t\t= " + std::to_string(_lower) + ":" + std::to_string(_upper);

	for (uint i = 0; i < file.size(); i++)
	{
		int index = file[i].find_first_of('\t');

		if (index != -1)
		{
			std::string tmp = file[i].substr(0, index);

			if (tmp == "mpID")
			{
				file[i] = toFile;
			}
		}
	}

	std::ofstream out;

	out.open(path, std::ios::out);

	for (uint i = 0; i < file.size(); i++)
	{
		out << file[i] << "\n";
	}

	out.close();
}

bool MultiplayerAspect::getID(std::pair<unsigned long long, unsigned long long>& _id)
{
	ReaderINI reader;
	RootINI root;
	Node_INI node;
	std::string su, sl, tmp;
	int vIndex = 0;
	unsigned long long l, u;

	reader.generateIniData("user");

	root = reader.getInitData();

	node = root.getNode("Multiplayer");

	tmp = node.getValue("mpID");

	vIndex = INI::getValueByIndex(sl, tmp, vIndex);
	vIndex = INI::getValueByIndex(su, tmp, vIndex);	

	INI::getTValue<unsigned long long>(l, sl);
	INI::getTValue<unsigned long long>(u, su);

	if (l == 0 && u == 0)
	{
		_id = std::pair<unsigned long long, unsigned long long>();
		return false;
	}

	_id = std::pair<unsigned long long, unsigned long long>(l, u);
	return true;
}