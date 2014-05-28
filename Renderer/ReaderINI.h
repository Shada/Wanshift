#pragma once

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string,std::string> Attributes;

struct Node_INI
{
private:
	Attributes attributes;

public:
	std::string getValue(std::string _key)
	{
		Attributes::iterator aIt = attributes.find(_key);
		std::string value = "Invalid";
		if(aIt != attributes.end())
		{
			value = attributes[_key];
		}

		return value;
	}

	Attributes getAllAttributes()
	{
		return attributes;
	}

	friend class ReaderINI;
};

struct RootINI
{
	std::map<std::string, Node_INI> nodes;
public:
	Node_INI* getNode(const std::string& _name)
	{
		std::map<std::string,Node_INI>::iterator nIt = nodes.find(_name);
		if(nIt == nodes.end())
			return nullptr;

		Node_INI* ni = &nIt->second;

		return ni;
	}

	bool getAllNodeAttributes(const std::string& _nodeName, Attributes& _inAttributes)
	{
		std::vector<std::string> temp;

		std::map<std::string,Node_INI>::iterator nIt = nodes.find(_nodeName);
		if(nIt == nodes.end())
			return false;

		Node_INI t = nIt->second;

		_inAttributes = t.getAllAttributes();

		return true;
	}
};

class ReaderINI
{
public:
	ReaderINI(void);
	~ReaderINI(void);

	bool generateIniData();
	bool generateIniData(std::string _singleFile);

	RootINI& getInitData();

private:
	RootINI initData;
	void addAttributeToNode(std::string& _key, std::string& _value);
	void readNodeAttributes(std::string& _line);
	void createNode(std::string& _node);
	bool readSingleINI(const std::string& _file);

	std::string currentNode;
};

