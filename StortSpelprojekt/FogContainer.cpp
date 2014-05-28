#include "FogContainer.h"
#include "ReaderINI.h"
#include "Utility_INI.h"

FogSim initFogSim( Node_INI& _node )
{
	FogSim fs;

	std::string col		= _node.getValue("Color");
	std::string sdep	= _node.getValue("StartDepth");
	std::string hlcol	= _node.getValue("HighlightColor");
	std::string gden	= _node.getValue("GlobalDensity");
	std::string hoff	= _node.getValue("HeightFalloff");

	std::string sr,sg,sb;
	float fr, fg, fb;

	int vIndex = 0;
	vIndex = INI::getValueByIndex( sr, col );
	vIndex = INI::getValueByIndex( sg, col, vIndex );
	vIndex = INI::getValueByIndex( sb, col, vIndex );

	INI::getTValue<float>( fr, sr );
	INI::getTValue<float>( fg, sg );
	INI::getTValue<float>( fb, sb );

	fs.color	= glm::vec3( fr, fg, fb );

	vIndex = 0;
	vIndex = INI::getValueByIndex( sr, hlcol );
	vIndex = INI::getValueByIndex( sg, hlcol, vIndex );
	vIndex = INI::getValueByIndex( sb, hlcol, vIndex );

	INI::getTValue<float>( fr, sr );
	INI::getTValue<float>( fg, sg );
	INI::getTValue<float>( fb, sb );

	fs.highlightColor	= glm::vec3( fr, fg, fb );

	INI::getTValue<float>( fs.globalDensity, gden );
	INI::getTValue<float>( fs.heightFalloff, hoff );
	INI::getTValue<float>( fs.startDepth, sdep );

	return fs;
}

FogContainer::FogContainer(void)
{
	setData("WorldVariables");
}

void FogContainer::setData(std::string _file)
{
	ReaderINI reader;
	reader.generateIniData(_file);
	RootINI data = reader.getInitData();

	Node_INI node = data.getNode("Fog_Type_List");
	Attributes types = node.getAllAttributes();

	for(Attributes::iterator aIt = types.begin(); aIt != types.end(); aIt++)
	{
		node = data.getNode( aIt->second );
		fogTypes[ aIt->second ] = initFogSim( node );
	}
}

FogContainer::~FogContainer(void)
{
}

FogSim FogContainer::getFogType(std::string _name)
{
	if( fogTypes.count(_name) == 0)
	{
		FogSim empty;
		return empty;
	}

	return fogTypes[_name];
}