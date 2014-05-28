#pragma once

#include "RenderInterface.h"
#include "Containers.h"
#include "ShaderClass.h"

/*	Console Component, it holds access to all
	relevant data for rendering text */
class Console
{
public:
	Console();
	~Console();

	/*	Initialize all dependecies

		#_graphics: Renderinterface instance

		returns
		false if _graphics == null
		true if else*/
	bool init(RenderInterface* _graphics);

	/*	Draw a sexy, half transparent overlay */
	void drawOverlay();

	/*	Draw text to screen

		#_msgIndex: the index for the text in the buffer
		#_pos: position to screen in pixels
		#_color: color of the text
		#_scale: scale of the text */
	void draw(const int _msgIndex, glm::vec2 _pos, glm::vec4 _color = glm::vec4(1, 1, 1, 1), float _scale = 1.0f);

	/*	Refresh already created text

		#_text: the new text-string
		#_textIndex: the original buffer index for the text
		#_fontIndex: the font index to use */
	unsigned int Console::loadText(std::string _text, uint _textIndex, uint _fontIndex);

	/*	Snag a bit of the buffer to hold the text

		#_maxSize: the max size that the can text can hold
		#_index: the index in the buffer */
	uint createText(uint _maxSize, uint _index);

	RenderInterface*	graphics;

private:

	ShaderClass*		shaderTech;
	ShaderClass*		overlayShader;
	FontContainer&		fontContainer;
	TextContainer&		textContainer;
	int					overlayBufferIndex;
	int					textIndexBuffer;
	int					cbTextIndex;
	int					backBufferRTV;

	friend class Debugger;
};
