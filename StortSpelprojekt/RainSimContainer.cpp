#include "RainSimContainer.h"
#include "ReaderINI.h"
#include "Utility_INI.h"

/*				Rain	Fog		Snow	Haze	Starfall
	A|Meadow	V		V		X		X		V			
	B|Desert	X		X		X		V		V
	C|Mountain	X		X		V		X		V
	D|Swamp		V		V		X		X		V
	E|Forrest	X		V		X		X		V
				AD		ADE		C		B		ABCDE

	Desert		= 0
	Meadow		= 1
	Mountain	= 2
	Swamp		= 3
	Forrest		= 4				
*/
/*				Rain	Fog		Snow	Haze	Starfall
	Rain		-		V		X		X		X
	Fog			V		-		X		X		V
	Snow		X		X		-		X		X	
	Haze		X		X		X		-		V
	Starfall	X		V		X		V		-
*/

RainSimSettings initSettings(Node_INI& _node)
{
	RainSimSettings rss;

	std::string sspeed	= _node.getValue("simulationSpeed");
	std::string texture	= _node.getValue("texture");
	std::string noise	= _node.getValue("noiseMap");
	std::string grpSize	= _node.getValue("groupSize");
	std::string grdSize	= _node.getValue("gridSize");
	

	INI::getTValue<float>(rss.simulationSpeed, sspeed);
	INI::getTValue<UINT>(rss.groupSize, grpSize);
	INI::getTValue<UINT>(rss.gridSize, grdSize);
	rss.textureName	= texture;
	rss.noiseMap	= noise;

	return rss;
}

RainSimConstants initConstants(Node_INI& _node)
{
	RainSimConstants rsC;

	std::string parS	= _node.getValue("particleScale");
	std::string col		= _node.getValue("color");

	std::string sr, sg, sb, sa;
	float r, g, b, a;

	int vIndex = 0;
	vIndex = INI::getValueByIndex(sr, col);
	vIndex = INI::getValueByIndex(sg, col, vIndex);
	vIndex = INI::getValueByIndex(sb, col, vIndex);
	vIndex = INI::getValueByIndex(sa, col, vIndex);

	INI::getTValue<float>(r, sr);
	INI::getTValue<float>(g, sg);
	INI::getTValue<float>(b, sb);
	INI::getTValue<float>(a, sa);

	rsC.color	= glm::vec4(r,g,b,a);
	INI::getTValue<float>(rsC.particleScale, parS);

	return rsC;
}

RainSimVariables initVariables(Node_INI& _node)
{
	RainSimVariables rsV;

	std::string verS	= _node.getValue("verticalSpeed");
	std::string mWE		= _node.getValue("maxWindEffect");
	std::string mWV		= _node.getValue("maxWindVariance");
	std::string rD		= _node.getValue("rainDensity");
	std::string bhs		= _node.getValue("boundHalfSize");
	std::string hmS		= _node.getValue("heightMapSize");
	std::string eoy		= _node.getValue("stretch");
	std::string cof		= _node.getValue("centerOffset");

	int vIndex = 0;
	std::string sx, sy, sz, sw;
	vIndex = INI::getValueByIndex(sx, bhs);
	vIndex = INI::getValueByIndex(sy, bhs, vIndex);
	vIndex = INI::getValueByIndex(sz, bhs, vIndex);

	float bx, by, bz, cx, cy, cz;

	INI::getTValue<float>(bx, sx);
	INI::getTValue<float>(by, sy);
	INI::getTValue<float>(bz, sz);

	vIndex = 0;
	vIndex = INI::getValueByIndex(sx, cof);
	vIndex = INI::getValueByIndex(sy, cof, vIndex);
	vIndex = INI::getValueByIndex(sz, cof, vIndex);

	INI::getTValue<float>(cx, sx);
	INI::getTValue<float>(cy, sy);
	INI::getTValue<float>(cz, sz);

	rsV.centerOffset	= glm::vec3(cx,cy,cz);
	rsV.boundHalfSize	= glm::vec3(bx,by,bz);
	INI::getTValue<float>(rsV.maxWindEffect,	mWE);
	INI::getTValue<float>(rsV.maxWindVariance,	mWV);
	INI::getTValue<float>(rsV.rainDensity,		rD);
	INI::getTValue<float>(rsV.verticalSpeed,	verS);
	INI::getTValue<UINT>(rsV.heightMapSize,		hmS);
	INI::getTValue<float>(rsV.stretch,			eoy);
	
	return rsV;
}

RainSimContainer::RainSimContainer(void)
{
	ReaderINI reader;

	bool result = reader.generateIniData("WorldVariables");
	RootINI data = reader.getInitData();
	Node_INI node = data.getNode("Rain_Constants");

	Attributes constants = data.getNode("Constants").getAllAttributes();
	for(Attributes::iterator aIt = constants.begin(); aIt != constants.end(); aIt++)
	{
		node = data.getNode( aIt->second );
		RainSimConstants rsc = initConstants(node);
		weatherConstants[aIt->second] = rsc;
	}

	Attributes settings = data.getNode("Simulation_Settings").getAllAttributes();
	for(Attributes::iterator aIt = settings.begin(); aIt != settings.end(); aIt++)
	{
		node = data.getNode( aIt->second );
		RainSimSettings rss = initSettings(node);
		weatherSettings[aIt->second] = rss;
	}

	Attributes types = data.getNode("Rain_Type_List").getAllAttributes();
	for(Attributes::iterator aIt = types.begin(); aIt != types.end(); aIt++)
	{
		node = data.getNode( aIt->second );
		RainSimVariables rsv = initVariables(node);
		weatherTypes[aIt->second] = rsv;
	}
}



RainSimContainer::~RainSimContainer(void)
{
}

RainSimVariables RainSimContainer::getRainVariables(const std::string& _name)
{
	if(weatherTypes.count(_name) == 0)
	{
		RainSimVariables empty;
		return empty;
	}

	return weatherTypes.find(_name)->second;
}

RainSimConstants RainSimContainer::getRainConstants(const std::string& _name)
{
	if(weatherConstants.count(_name) == 0)
	{
		RainSimConstants empty;
		return empty;
	}

	return weatherConstants.find(_name)->second;
}

RainSimSettings RainSimContainer::getSettings(const std::string& _name)
{
	if(weatherSettings.count(_name) == 0)
	{
		RainSimSettings empty;
		return empty;
	}

	return weatherSettings.find(_name)->second;
}