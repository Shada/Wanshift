#include "Containers.h"


Containers::map_type Containers::map = Containers::initializeMap();

//TEST(Containers, accessContainer)
//{
//	Containers c( std::make_pair( Containers::EXAMPLE, MODIFY ) );
//	ASSERT_TRUE( c.get<ExampleContainer>( Containers::EXAMPLE ) != NULL );
//}

Containers::Containers(pair o1)
{
	std::vector<pair> v;
	v.push_back(o1);
	initializeContainers(v);
}

Containers::Containers(pair o1, pair o2)
{
	std::vector<pair> v;
	v.push_back(o1);
	v.push_back(o2);
	initializeContainers(v);
}

Containers::Containers(pair o1, pair o2, pair o3)
{
	std::vector<pair> v;
	v.push_back(o1);
	v.push_back(o2);
	v.push_back(o3);
	initializeContainers(v);
}

Containers::Containers( pair o1, pair o2, pair o3, pair o4 )
{
	std::vector<pair> v;
	v.push_back( o1 );
	v.push_back( o2 );
	v.push_back( o3 );
	v.push_back( o4 );
	initializeContainers( v );
}
Containers::Containers(pair o1, pair o2, pair o3, pair o4, pair o5)
{
	std::vector<pair> v;
	v.push_back(o1);
	v.push_back(o2);
	v.push_back(o3);
	v.push_back(o4);
	v.push_back(o5);
	initializeContainers(v);
}
Containers::Containers(pair o1, pair o2, pair o3, pair o4, pair o5, pair o6)
{
	std::vector<pair> v;
	v.push_back(o1);
	v.push_back(o2);
	v.push_back(o3);
	v.push_back(o4);
	v.push_back(o5);
	v.push_back(o6);
	initializeContainers(v);
}

void Containers::initializeContainers(std::vector< pair > list)
{
	access = list;
}

Containers::map_type Containers::initializeMap()
{
	map_type m;

	addToMap(m, OBJECT2D, new Object2DContainer());
	addToMap(m, TEXT, new TextContainer());
	addToMap(m, TERRAIN, new TerrainContainer());
	addToMap(m, FONT, new FontContainer());
	addToMap(m, PLAYER, new PlayerContainer());
	addToMap(m, CAMERA, new CameraContainer());
	addToMap(m, DETAIL_LAYER, new DetailLayerContainer());
	addToMap(m, SANDSTORM, new SandStormContainer());
	addToMap(m, WIND, new WindContainer());
	addToMap(m, RAIN_SIM, new RainSimContainer());
	addToMap(m, ATMOSPHERE_SIM, new AtmosphereContainer());
	addToMap(m, BIOMS, new BiomsContainer());
	addToMap(m, CONSTBUFFER, new ConstantBufferContainer());
	addToMap(m, ENEMY, new EnemyContainer());
	addToMap(m, FOREST, new ForestContainer());
	addToMap(m, FOG_SIM, new FogContainer());
	addToMap(m, PARTICLE_EFFECT, new ParticleEffectContainer());
	return m;
}
