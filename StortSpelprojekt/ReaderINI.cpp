#include "ReaderINI.h"
#include "Utility_INI.h"
#include <iostream>
#include <fstream>

using namespace INI;

ReaderINI::ReaderINI()
{
}

ReaderINI::~ReaderINI()
{
}

void ReaderINI::createNode(std::string& _node)
{
	std::map<std::string, Node_INI>::iterator iIt = initData.nodes.find(currentNode);

	if(iIt == initData.nodes.end())
	{
		Node_INI n;
		initData.nodes.insert(std::make_pair(_node, n));
	}

	currentNode = _node;
}

void ReaderINI::addAttributeToNode(std::string& _key, std::string& _value)
{
	std::pair<std::string, std::string> inner = std::make_pair(_key, _value);

	initData.nodes[currentNode].attributes[_key] = _value;
}

void ReaderINI::readNodeAttributes(std::string& _line)
{
	if(isAttribute(_line))
	{
		std::string key = createKey(_line);
		std::string val = createValue(_line);

		if(initData.nodes.begin() != initData.nodes.end())
		{
			addAttributeToNode(key, val);
		}
	}
}

bool ReaderINI::generateIniData(std::string _singleFile)
{
	currentNode.clear();

	std::map<std::string, Node_INI>::iterator iIt;// = initData.nodes.begin();

	for(iIt = initData.nodes.begin(); iIt != initData.nodes.end(); iIt++)
	{
		iIt->second.attributes.clear();
	}

	initData.nodes.clear();

	int numFailures = 0;

	bool r = readSingleINI(_singleFile);

	if(!r)
		numFailures++;

	return true;
}

bool ReaderINI::generateIniData()
{
	currentNode.clear();

	std::map<std::string, Node_INI>::iterator iIt;// = initData.nodes.begin();

	for(iIt = initData.nodes.begin(); iIt != initData.nodes.end(); iIt++)
	{
		iIt->second.attributes.clear();
	}

	initData.nodes.clear();

	std::string t[3] = { "default", "user", "debug" };
	int numFailures = 0;

	for(unsigned int i = 0; i < 3; ++i)
	{
		bool r = readSingleINI(t[i]);

		if(!r)
			numFailures++;
	}

	return true;
}

bool ReaderINI::readSingleINI(const std::string& _fileName)
{
	std::ifstream in;
	std::string path = "Init/";
	std::string filename = _fileName;
	std::string extension = ".ini";
	std::string file = path + filename + extension;
	in.open(file, std::ios::in);

	if(!in.is_open())
		return false;

	std::string line;

	int lineCount = 0;

	while(!in.eof())
	{
		line.clear();
		std::getline(in, line);
		lineCount++;

		if(isComment(line))
		{
			continue;
		}
		else if(isNode(line))
		{
			createNode(line);
		}
		else if(isAttribute(line))
		{
			readNodeAttributes(line);
		}
	}

	in.close();

	return true;
}

RootINI ReaderINI::getInitData()
{
	return initData;
}