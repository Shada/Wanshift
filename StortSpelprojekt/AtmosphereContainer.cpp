#include "AtmosphereContainer.h"
#include "ReaderINI.h"
#include "Utility_INI.h"

AtmosphereQuality initQuality(Node_INI& _node)
{
	AtmosphereQuality aq;

	std::string sam	= _node.getValue("nSamples");
	std::string sd	= _node.getValue("scaleDepth");
	std::string sod	= _node.getValue("scaleOverDepth");

	INI::getTValue(aq.nSamples,			sam);
	INI::getTValue(aq.scaleDepth,		sd);
	INI::getTValue(aq.scaleOverDepth,	sod);

	return aq;
}

AtmosphereWorld initWorld(Node_INI& _node)
{
	AtmosphereWorld aw;

	std::string radMul	= _node.getValue("radiusMultiplier");
	std::string iR		= _node.getValue("innerRadius");
	std::string oR		= _node.getValue("outerRadius");
	std::string gr		= _node.getValue("groundReflectance");
	std::string br		= _node.getValue("brightness");

	float fiR, foR, fMul;
	INI::getTValue<float>(fiR, iR);
	INI::getTValue<float>(foR, oR);
	INI::getTValue<float>(fMul, radMul);
	
	/*INI::getTValue<float>(aw.innerRadius, iR);
	INI::getTValue<float>(aw.outerRadius, oR);*/
	aw.innerRadius	= fiR * fMul;
	aw.outerRadius	= foR * fMul;
	INI::getTValue<float>(aw.brightness, br);
	INI::getTValue<float>(aw.reflectance, gr);

	return aw;
}

ScatterType initScatter(Node_INI& _node)
{
	ScatterType st;
	std::string krpi	= _node.getValue("kr4PI");
	std::string kmpi	= _node.getValue("km4PI");
	std::string krsun	= _node.getValue("krEsun");
	std::string kmsun	= _node.getValue("kmEsun");
	std::string gM		= _node.getValue("gM");
	std::string wR		= _node.getValue("wR");
	std::string wM		= _node.getValue("wM");
	std::string exp		= _node.getValue("exposure");
	std::string pwArg	= _node.getValue("powArg");

	int pA;
	INI::getTValue<int>(pA, pwArg);

	std::string sx, sy, sz;
	float rx, mx, ry, my, rz, mz;

	int vIndex = 0;
	vIndex = INI::getValueByIndex(sx, wM);
	vIndex = INI::getValueByIndex(sy, wM, vIndex);
	vIndex = INI::getValueByIndex(sz, wM, vIndex);

	INI::getTValue(mx, sx);
	INI::getTValue(my, sy);
	INI::getTValue(mz, sz);

	vIndex = 0;
	vIndex = INI::getValueByIndex(sx, wR);
	vIndex = INI::getValueByIndex(sy, wR, vIndex);
	vIndex = INI::getValueByIndex(sz, wR, vIndex);

	INI::getTValue(rx, sx);
	INI::getTValue(ry, sy);
	INI::getTValue(rz, sz);

	INI::getTValue<float>(st.exposure, exp);
	INI::getTValue<float>(st.gM, gM);
	INI::getTValue<float>(st.km4PI, kmpi);
	INI::getTValue<float>(st.kmEsun, kmsun);
	INI::getTValue<float>(st.kr4PI, krpi);
	INI::getTValue<float>(st.krEsun, krsun);
	st.wR = glm::vec3( 1.0 / pow(rx, pA), 1.0 / pow(ry, pA), 1.0 / pow(rz, pA) );
	st.wM = glm::vec3( 1.0 / pow(mx, pA), 1.0 / pow(my, pA), 1.0 / pow(mz, pA) );

	return st;
}

AtmosphereContainer::AtmosphereContainer(void)
{
	setData("WorldVariables");
}


AtmosphereContainer::~AtmosphereContainer(void)
{
}

void AtmosphereContainer::setData(std::string _file)
{
	ReaderINI reader;
	reader.generateIniData(_file);
	RootINI root = reader.getInitData();

	Node_INI node = root.getNode("Atmosphere_Quality");
	atmosphereQuality = initQuality(node);

	node = root.getNode("Atmosphere_World");
	atmosphereWorld = initWorld(node);

	node = root.getNode("Scatter_Type_List");
	Attributes types = node.getAllAttributes();

	for(Attributes::iterator sIt = types.begin(); sIt != types.end(); sIt++)
	{
		node = root.getNode( sIt->second );
		scatterTypes[ sIt->second ] = initScatter( node );
	}
}

AtmosphereQuality AtmosphereContainer::getAtmosphereQuality()
{
	return atmosphereQuality;
}

AtmosphereWorld AtmosphereContainer::getAtmosphereWorld()
{
	return atmosphereWorld;
}

ScatterType AtmosphereContainer::getScatter(UINT _index)
{
	if(_index >= scatterTypes.size() || scatterTypes.size() == 0)
	{
		ScatterType st;
		return st;
	}

	return scatterTypes.at(0);
}

ScatterType AtmosphereContainer::getScatter(std::string _name)
{
	if( scatterTypes.count(_name) <= 0 )
	{
		ScatterType st;
		return st;
	}

	return scatterTypes[_name];
}