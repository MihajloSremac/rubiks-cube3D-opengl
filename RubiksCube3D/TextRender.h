#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Struktura za čuvanje informacija o svakom karakteru
struct Character {
    GLuint TextureID;   // ID teksture za karakter
    glm::ivec2 Size;    // Dimenzije karaktera
    glm::ivec2 Bearing; // Offset od baze linije
    GLuint Advance;     // Offset do sledećeg karaktera
};

class TextRender {
public:
    // Konstruktor koji učitava font i postavlja šejdere
    TextRender(const std::string& fontPath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, int fontSize);

    // Renderovanje teksta na datoj poziciji
    void RenderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    ~TextRender(); // Destruktor za čišćenje resursa

private:
    GLuint VAO, VBO;                          // OpenGL objekti za renderovanje
    GLuint shaderProgram;                     // Šejder program specifičan za ovaj renderer
    std::map<char, Character> Characters;     // Mapa ASCII karaktera

    // Privatne funkcije
    unsigned int compileShader(GLenum type, const std::string& source);
    unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void loadCharacters(const std::string& fontPath, int fontSize);
};

#endif