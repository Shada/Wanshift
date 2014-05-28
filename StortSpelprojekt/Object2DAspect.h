#pragma once
#include "aspectinterface.h"
#include "RenderInterface.h"
#include <fstream>
#include "ShaderClass.h"
class Object2DAspect :
	public AspectInterface
{
public:
	Object2DAspect();
	~Object2DAspect();
	void runAspect(float _dt)override;
	void init(MainState& _mainState)override;
	/*-------------------------------------------------------------------------------------------------
												BILLBOARDS
												---------------------------------------------------------------------------------------------------*/

	/*-------------------------------------------------------------------------------------------------
												TEXT
												---------------------------------------------------------------------------------------------------*/
	/* initalize text */
	void initTexts();
	/*allocates space in the letter buffer for text.
	# _maxSize is the amount of letters you want to allocate for your text.
	# _index is the index to the text in the container
	returns the start position of the text in the letters array.*/
	uint createText(uint _maxSize, uint _index);
	/*adds a set of characters that you specify, to a buffer.
	# _text is a text string with the text to be converted to drawable text format and
		be inserted into _outData, character by character.
	# _textIndex is the index of the text in the container.
	# _fontIndex is the index of the font in the container used for the text.
	returns the amount of characters written into the buffer (space excluded)
	throws exception if no font settings have been loaded through loadFontSettings() or if incorrect settings are use.
		Writes a harmless warning message through cout if an unrecognizable character is handled.
		If your text requires more than the allocated space in the letters vector, then the text will be cut off at the limit.
		Do note that spaces do not take up vectorspace, so a 14 letter word where 10 of the letters are spaces will only require 4 allocated spaces in the vector.*/
	unsigned int loadText(std::string _text, uint _textIndex, uint _fontIndex);
	/*-------------------------------------------------------------------------------------------------
												FONTS
												---------------------------------------------------------------------------------------------------*/
	/* initalize all fonts */
	void initFonts();
	/* creates an array of pre-calculated measurements of all available characters. (DO NOT INCLUDE FILE EXTENSION. IT IS ASSUMED THAT THE EXTENSIONS ARE .TXT AND .PNG)
	#_filepath is the path (including the font folder) to the font file and image file representing the font.
	#throws exception if no files were found.
	returns the index of the image in the shared graphics container*/
	int createFont(std::string _filePath, uint _index);

private:
	RenderInterface		*graphics;
	FontContainer		*fontContainer;
	TextContainer		*textContainer;
	Object2DContainer	*object2DContainer;
	ShaderClass			*shaderTech;

	unsigned int textBufferIndex;

	int cbTextIndex;

	friend class Console;
};