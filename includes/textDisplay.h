#ifndef TEXT_DISPLAY_CLASS_H
#define TEXT_DISPLAY_CLASS_H

#include "minecraft.h"
#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include <ft2build.h>


class TextDisplay {
	struct Character {
		unsigned int textureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};

	std::map<char, Character> Characters;

	GLuint VAO, VBO;
	Shader  textShader;

	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	public:


		void display(const char* text);
		void display(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
		void display(const char* text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
		void display(std::string text, glm::vec2 position, GLfloat scale, glm::vec3 color);
		void Init(float width, float height);
		void setProjection(float width, float height);
};

#endif