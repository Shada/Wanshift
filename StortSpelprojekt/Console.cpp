#include "Console.h"
#include "TextContainer.h"
#include "FontContainer.h"
#include "Object2DAspect.h"
#include "Containers.h"
#include <iostream>

struct OVERLAYSPRITE
{
	glm::vec2	topleft;
	glm::vec2	dimensions;
	float		opacity;
};


Console::Console() : textContainer(*Containers(std::make_pair(Containers::TEXT, MODIFY)).get<TextContainer>(Containers::TEXT)),
					fontContainer(*Containers(std::make_pair(Containers::FONT, READ)).get<FontContainer>(Containers::FONT))
{
}

Console::~Console()
{
}

bool Console::init(RenderInterface* _graphics)
{
	if(_graphics == nullptr)
		return false;

	graphics = _graphics;
	cbTextIndex = graphics->getHandleIndex("cbText",ResourceType::ConstantBuffer);
	shaderTech		= new ShaderClass("fontVS", "fontGS", "", "", "fontPS", graphics, (int)LayoutTypes::LAYOUT_TEXT, false);
	overlayShader	= new ShaderClass("overlayVS", "overlayGS", "", "", "overlayPS", graphics, (int)LayoutTypes::LAYOUT_OVERLAY, false);

	OVERLAYSPRITE overlay;
	overlay.topleft = glm::vec2(-1, 1);
	overlay.dimensions = glm::vec2(2, 0.3);
	overlay.opacity = 0.15f;
	overlayBufferIndex = graphics->createVBufferImmutable((void**)&overlay, 1, sizeof(OVERLAYSPRITE));

	textContainer.allocate(0, 40);	//I'm sorry!
	textIndexBuffer = textContainer.textBufferIndex;

	backBufferRTV = graphics->getHandleIndex("backBufferRTV",ResourceType::RenderTargetView);
	return true;
}

void Console::drawOverlay()
{
	UINT strides = sizeof(OVERLAYSPRITE);
	UINT offsets = 0;
	graphics->setTopology(1);
	graphics->setVertexBuffers(0, overlayBufferIndex, 1, &strides, &offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_OVERLAY);
	overlayShader->use();
	//graphics->setToScreenRender(-1);
	graphics->setRenderTargetViews(backBufferRTV,1,-1);
	graphics->setBlendState(1);
	graphics->draw(1, 0);
}

void Console::draw(const int _msgIndex, glm::vec2 _pos, glm::vec4 _color, float _scale)
{
	graphics->updateVBuffer(textIndexBuffer, (void**)&textContainer.letters[0], textContainer.letters.size(), sizeof(Letter));

	CBTextData tData;
	tData.tintAlpha = _color;
	tData.pos = _pos;
	tData.scale = glm::vec2(_scale, _scale);
	tData.imageSize = glm::vec2(fontContainer.imageWidths.at(0), fontContainer.imageHeights.at(0));
	graphics->updateCBuffer(cbTextIndex, &tData);
	graphics->setCBuffers(0,1,cbTextIndex);
	UINT strides = sizeof(Letter);
	UINT offsets = 0;

	graphics->setVertexBuffers(0, textContainer.textBufferIndex, 1, &strides, &offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_TEXT);
	shaderTech->use();
	//graphics->setToScreenRender(-1);
	graphics->setRenderTargetViews(backBufferRTV,1,-1);
	graphics->setPixelShaderResource(1, fontContainer.imageIndices.at(textContainer.fontIndices.at(0)), 1);
	graphics->setBlendState(1);
	graphics->draw(textContainer.sizes.at(_msgIndex), textContainer.bufferIndices.at(_msgIndex));
}

uint Console::createText(uint _maxSize, uint _index)
{
	//textContainer.allocate(0,1);
	//_index = textContainer.sizes.size()-1;
	uint bufferStartIndex = textContainer.letters.size();		// we'll place the new text at the end of the buffer
	textContainer.bufferIndices.at(_index) = bufferStartIndex; // save the buffer start index for the text
	textContainer.maxSizes.at(_index) = _maxSize;				// save the max size for the text
	textContainer.letters.resize(bufferStartIndex + _maxSize);	// the vector is resized to fit the new text at the end.

	return bufferStartIndex;
}

unsigned int Console::loadText(std::string _text, uint _textIndex, uint _fontIndex)
{
	unsigned int usedBufferAmount = 0;
	unsigned int size = _text.size();
	Letter current;
	int posx = 0;
	textContainer.fontIndices.at(_textIndex) = _fontIndex;

	//if the size of the character set used for the selected font is empty (font has not been loaded, or was loaded incorrectly).
	if(fontContainer.characterSets.at(_fontIndex).size() == 0)
	{
		std::cout << "Exception thrown: No font loaded for text ( " << _text.c_str() << " )\n";
		throw new std::exception("Cannot create text. No font has been loaded.");
		return 0;
	}
	for(unsigned int i = 0; i < size && usedBufferAmount < textContainer.maxSizes.at(_textIndex); i++)
	{
		if(_text[i] >= 33 && _text[i] < 127)
		{
			current.x = (float)posx;
			current.texCoords.x = (float)fontContainer.characterSets.at(_fontIndex).at(_text[i] - 33).posx;		// x1
			current.texCoords.y = (float)fontContainer.characterSets.at(_fontIndex).at(_text[i] - 33).posy;		// y1
			current.texCoords.z = (float)fontContainer.characterSets.at(_fontIndex).at(_text[i] - 33).width;		// width
			current.texCoords.w = (float)fontContainer.letterHeights.at(_fontIndex);								// height

			textContainer.letters.at(textContainer.bufferIndices.at(_textIndex) + usedBufferAmount) = current;

			posx += fontContainer.characterSets.at(_fontIndex).at(_text[i] - 33).width + fontContainer.spacings.at(_fontIndex);
			usedBufferAmount++;
		}
		else if(_text[i] == 32) //space
		{
			posx += fontContainer.spacings.at(_fontIndex) * 5;
		}
		else
		{
			//std::cout << "Warning: unrecognizable character \"" << _text[i] << "\" will be skipped.\n";
		}
	}
	textContainer.sizes.at(_textIndex) = usedBufferAmount;
	return usedBufferAmount;
}