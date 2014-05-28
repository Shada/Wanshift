#pragma once

#include <string>
#include <exception>
#include <sstream>

namespace INI
{
	/*

	*/
	template<typename T>
	static bool getTValue(T& _inValue, std::string& _value)
	{
		std::istringstream convert(_value);
		return convert >> _inValue ? true : false;
	}

	/*	Check whether a string.find(...) is valid or not
		
		#_elementIndex: the index retrieved from string.find(...)
		
		#returns
			true if index >= 0
			false if else */
	static bool isValidStringElement(unsigned int _elementIndex)
	{
		return _elementIndex != std::string::npos;
	}

	/*	Erase string element startings from the first found delimiter character.
		The delimiter and all following elements will be erased.
		
		#_line: string reference
		#_delimiter: breakpoint character */
	static void eraseAllAfterChar(std::string& _line, char _delimiter = '#')
	{
		int sep = _line.find_first_of(_delimiter);

		if(isValidStringElement(sep))
			_line.erase(sep, _line.size() - 1);
	}

	/*	Clean attribute string from whitespace, tab, = and #
		characters
		
		#_line: string reference */
	static void cleanString(std::string& _line)
	{

		char crap[] = { ' ', '\t', '=', '[', ']' };

		int cStart;
		for(unsigned int i = 0; i < 5; ++i)
		{
			while(isValidStringElement(_line.find(crap[i])))
			{
				cStart = _line.find_first_of(crap[i]);
				_line.erase(cStart,1);
			}
		}
		eraseAllAfterChar(_line, '#');
	}

	/*	Extract key part from attribute
		
		#_line: attribute reference 
		
		#returns
			string of key (cleaned up) */
	static std::string createKey(std::string& _line)
	{
		char delim = '=';
		int end = _line.find_first_of(delim);
		std::string temp = _line.substr(0,end);

		cleanString(temp);

		return temp;
	}

	/*	Extract value part from attribute
	
		#_line: attribute reference
		
		#returns
			string of value (cleaned up)*/
	static std::string createValue(std::string& _line)
	{
		char delim = '=';
		int start	= _line.find_first_of(delim);
		int end		= _line.size()-1;
		std::string temp = _line.substr(start+1, end);

		cleanString(temp);

		return temp;
	}

	/*	Check if a string has attribute qualities
		
		#_line: attribute candidate reference
		
		#returns
			true if "=" delimiter exist and it's not the last character
			false if else*/
	static bool isAttribute(std::string& _line)
	{
		char delim = '=';
		unsigned int sep = _line.find_first_of(delim);
		
		if(sep != _line.size() )
			return isValidStringElement(sep);

		return false;
	}

	/*	Check if a string has Node qualities
	
		#_line: node candidate reference
		
		#returns
			true if line starts with "[" and ends with "]"
			false if else*/
	static bool isNode(std::string& _line)
	{
		int start, end;
		char sDelim = '[';
		char eDelim = ']';

		start	= _line.find_first_of(sDelim) + 1;
		end		= _line.find_first_of(eDelim) - 1;

		if( !isValidStringElement(start) || 
			!isValidStringElement(end))
		{
			return false;
		}

		if(start >= end)
			return false;

		if(end - start > 0)
			_line = _line.substr(start,end);
		else return false;

		return true;
	}

	/*	Check if an entire string is a comment
	
		#_line: string reference
		
		#returns
			true if first element is "#"
			false if else */
	static bool isComment(std::string& _line)
	{
		return _line[0] == '#';
	}

	/*	Check whether an attribute stores several values
		and fetch the value next in line. this should be 
		done in a while loop with this method as a conditional. 
		- Warning! This will affect the original reference value.
		
		ex. while( isSplitValue(outSubValue, entireValueRef) )
			{ store( outSubValue ); }
			
		#_outValue: reference to temporary outValue
		#_line: reference to the original multi-value
		#_delimiter: subvalue breakpoint
		
		#returns
			true if more values still exist in _line
			false if _line is empty */
	static inline bool isSplitValue(std::string& _outValue, std::string& _line, char _delimiter = ':')
	{
		if(_line.size() <= 0)
			return false;

		int sep = _line.find_first_of(_delimiter);
		bool isLastValue = !isValidStringElement(sep);
		
		if(isLastValue)
		{
			_outValue = _line.substr(0,_line.size());
			_line.clear();
		}
		else
		{
			_outValue = _line.substr(0,sep);
			_line.erase(0,sep+1);
		}

		return true;
	}

	/*	Fetch a subValue from a multi-value attribute without
		affecting the original value reference. the return value 
		marks the start of the next value in line. Use it as 
		_valueIndex argument for grabbing subsequent subvalues;
		
		#_outValue: reference to temporary outparameter
		#_line: reference to the original multi-value
		#_valueIndex: index pointing to the next subvalue in line
		#_delimiter: subvalue breakpoint

		#returns
			index to the next subvalue in line
			-1 if last subvalue
		*/
	static inline int getValueByIndex(std::string& _outValue, const std::string& _line, int _valueIndex = 0, char _delimiter = ':')
	{
		int start	= _line.find_first_not_of(_delimiter, _valueIndex);
		int sep		= _line.find_first_of(_delimiter, start);
		_outValue	= _line.substr(start, sep - start);		// this looks like it will fail for the last value, but it won't
															// due to npos weirdness
		return sep;
	}
}