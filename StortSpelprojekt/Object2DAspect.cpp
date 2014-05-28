#include "Object2DAspect.h"
#include <iostream>
#include "MainState.h"

Object2DAspect::Object2DAspect()
	: AspectInterface("Object2D", 0)
{

}

Object2DAspect::~Object2DAspect() {}

void Object2DAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;
	Containers containers(std::make_pair(Containers::OBJECT2D, MODIFY), std::make_pair(Containers::TEXT, Mode::MODIFY), std::make_pair(Containers::FONT, Mode::MODIFY));

	fontContainer = containers.get<FontContainer>(Containers::FONT);
	textContainer = containers.get<TextContainer>(Containers::TEXT);
	object2DContainer = containers.get<Object2DContainer>(Containers::OBJECT2D);

	initFonts();
	initTexts();

	textContainer->textBufferIndex = graphics->createVBufferDynamic(1000, sizeof(Letter));
	//graphics->updateVBuffer(textBufferIndex, (void**)&textContainer->letters[0],textContainer->letters.size(),sizeof(Letter));

	shaderTech = new ShaderClass("fontVS", "fontGS", "", "", "fontPS", graphics, (int)LayoutTypes::LAYOUT_TEXT, false);
	cbTextIndex = graphics->getHandleIndex("cbText",ResourceType::ConstantBuffer);
}


void Object2DAspect::runAspect(float _dt)
{
	CBTextData tData;
	tData.tintAlpha = glm::vec4(0, 0, 0, 1);//glm::vec4(object2DContainer->tintAlphaData.at(0));
	tData.pos = glm::vec2(50, 50);//glm::vec2(object2DContainer->posData.at(0));
	tData.scale = glm::vec2(object2DContainer->scaleData.at(0));
	tData.imageSize = glm::vec2(fontContainer->imageWidths.at(0), fontContainer->imageHeights.at(0));
	
	graphics->updateCBuffer(cbTextIndex, &tData);

	UINT strides = sizeof(Letter);
	UINT offsets = 0;
	graphics->setVertexBuffers(0, textBufferIndex, 1, &strides, &offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_TEXT);
	shaderTech->use();
	
	graphics->setToScreenRender(-1);
	graphics->setPixelShaderResource(1, fontContainer->imageIndices.at(textContainer->fontIndices.at(0)), 1);
	graphics->setBlendState(1);

	graphics->setCBuffers(0,1,cbTextIndex);
	graphics->draw(textContainer->sizes.at(0), textContainer->bufferIndices.at(0));
}

uint Object2DAspect::createText(uint _maxSize, uint _index)
{
	uint bufferStartIndex = textContainer->letters.size();		// we'll place the new text at the end of the buffer
	textContainer->bufferIndices.at(_index) = bufferStartIndex; // save the buffer start index for the text
	textContainer->maxSizes.at(_index) = _maxSize;				// save the max size for the text
	textContainer->letters.resize(bufferStartIndex + _maxSize);	// the vector is resized to fit the new text at the end.
	return bufferStartIndex;
}

unsigned int Object2DAspect::loadText(std::string _text, uint _textIndex, uint _fontIndex)
{
	unsigned int usedBufferAmount = 0;
	unsigned int size = _text.size();
	Letter current;
	int posx = 0;
	textContainer->fontIndices.at(_textIndex) = _fontIndex;

	//if the size of the character set used for the selected font is empty (font has not been loaded, or was loaded incorrectly).
	if(fontContainer->characterSets.at(_fontIndex).size() == 0)
	{
		std::cout << "Exception thrown: No font loaded for text ( " << _text.c_str() << " )\n";
		throw new std::exception("Cannot create text. No font has been loaded.");
		return 0;
	}
	for(unsigned int i = 0; i < size && usedBufferAmount < textContainer->maxSizes.at(_textIndex); i++)
	{
		if(_text[i] >= 33 && _text[i] < 127)
		{
			current.x = (float)posx;
			current.texCoords.x = (float)fontContainer->characterSets.at(_fontIndex).at(_text[i] - 33).posx;		// x1
			current.texCoords.y = (float)fontContainer->characterSets.at(_fontIndex).at(_text[i] - 33).posy;		// y1
			current.texCoords.z = (float)fontContainer->characterSets.at(_fontIndex).at(_text[i] - 33).width;		// width
			current.texCoords.w = (float)fontContainer->letterHeights.at(_fontIndex);								// height

			textContainer->letters.at(textContainer->bufferIndices.at(_textIndex) + usedBufferAmount) = current;

			posx += fontContainer->characterSets.at(_fontIndex).at(_text[i] - 33).width + fontContainer->spacings.at(_fontIndex);
			usedBufferAmount++;
		}
		else if(_text[i] == 32) //space
		{
			posx += fontContainer->spacings.at(_fontIndex) * 5;
		}
		else
		{
			std::cout << "Warning: unrecognizable character \"" << _text[i] << "\" will be skipped.\n";
		}
	}
	textContainer->sizes.at(_textIndex) = usedBufferAmount;
	return usedBufferAmount;
}
void Object2DAspect::initTexts()
{
	/*object2DContainer->allocate(0,2);
	textContainer->allocate(0,3);
	int a = createText(100,0);
	int i = loadText("LALALALAL",0,0);

	int c = createText(100,1);
	int b = loadText("CECE",1,0);

	int g = createText(100,2);
	int h = loadText("DADA",2,0);*/
	//loadText("1234567890:;_+´`-'¨\"\\/*§><|^åäöÅÄÖabcdefghijk lmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!?", 0, 0);
}
void Object2DAspect::initFonts()
{
	//allocate room for new fonts.
	fontContainer->allocate(0, 1);
	//load a font
	uint fontIndex = 0;
	int imageIndex = createFont("Fonts/Myriad-Pro", fontIndex);
	if(imageIndex < 0)
		std::cout << "Error: Could not load font";
	fontContainer->imageIndices.at(fontIndex) = imageIndex;
	//more fonts
}

int Object2DAspect::createFont(std::string _filePath, uint _index)
{
	std::ifstream myfile(_filePath + ".txt");
	if(myfile.is_open())
	{
		fontContainer->characterSets.push_back(std::vector< Character >());
		myfile >> fontContainer->imageWidths.at(_index);
		myfile >> fontContainer->imageHeights.at(_index);
		myfile >> fontContainer->letterHeights.at(_index);
		myfile >> fontContainer->spacings.at(_index);

		unsigned int number;
		while(myfile >> number)
		{
			Character c;
			c.posx = number;

			myfile >> number;
			c.posy = number;

			myfile >> number;
			c.width = number;

			fontContainer->characterSets.at(_index).push_back(c);
		}

	}
	else
	{
		std::cout << "Exception thrown: Font file " << _filePath.c_str() << " not found.\n";
		throw new std::exception("Font file not found");
	}
	myfile.close();
	return graphics->createShaderResourceViewFromFile(_filePath + ".png");
}