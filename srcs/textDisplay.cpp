#include "textDisplay.h"

void TextDisplay::setProjection(float width, float height) {
    textShader.Activate();
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
    textShader.setMat4("projection", projection);
}

void TextDisplay::Init(float width, float height) {
    textShader.Load("shaders/textVS.glsl", "shaders/textFS.glsl");
    textShader.Activate();
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
    textShader.setMat4("projection", projection);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    glActiveTexture(GL_TEXTURE1);
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextDisplay::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activation du rendu
    textShader.Activate();
    textShader.setVec3("textColor", color.x, color.y, color.z);
    textShader.setInt("text", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    // Boucle sur tous les caract�res
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Mise � jour du VBO
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Rendu du glyphe sur le rectangle
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // Mise � jour de la m�moire du VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Rendu du rectangle
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Avancer le curseur au glyphe suivant (noter que l�avance est calcul�e en 1/64e pixels)
        x += (ch.Advance >> 6) * scale; // D�calage � droite de 6 pour obtenir la valeur en pixels
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    //  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextDisplay::display(const char* text)
{
    RenderText(text, 0.0f, 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void TextDisplay::display(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    RenderText(text, x, y, scale, color);

}

void TextDisplay::display(const char* text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    RenderText(text, x, y, scale, color);
}

void TextDisplay::display(std::string text, glm::vec2 position, GLfloat scale, glm::vec3 color)
{
    RenderText(text, position.x, position.y, scale, color);
}
