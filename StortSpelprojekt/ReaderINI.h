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
	Node_INI getNode(const std::string& _name)
	{
		std::map<std::string,Node_INI>::iterator nIt = nodes.find(_name);

		//if(nIt == nodes.end())
		//	return nodes.begin();

		return nIt->second;
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
	ReaderINI();
	~ReaderINI();

	/*	create data graph from master .inis 
	
		#returns 
			true if all files exists
			false if else */
	bool generateIniData();

	/*	create data graph from single .ini 
	
		#_singleFile: name of the .ini 
		
		#returns
			true if file exist
			false if else */
	bool generateIniData(std::string _singleFile);

	/*	get data graph 
	
		returns
			root of data graph */
	RootINI getInitData();

private:
	
	/*	add attribute to the currently worked on node
	
		#_key: attribute key
		#_value: attribute value */
	void addAttributeToNode(std::string& _key, std::string& _value);

	/*	read currently worked on line and add it to the currently 
		worked on node if it is an attribute 
	
		#_line: raw string line from .ini */
	void readNodeAttributes(std::string& _line);

	/*	create a node from the cleaned up raw string line */
	void createNode(std::string& _node);

	/*	read and create data graph from single .ini 
	
		#_singleFile: name of the .ini 
		
		#returns
			true if file exist
			false if else */
	bool readSingleINI(const std::string& _file);

	RootINI initData;
	std::string currentNode;
};
