#pragma once

// Includes
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

/**
 * @class Shader,
 * @brief Imports and maintains program shaders for opengl.
 */
class Shader
{
public:
    // ---------- Global Variables --------------

     // the program ID
    unsigned int ID;

private:
    // ---------- Local Variables --------------

public:
    // ---------- Global functions --------------

    // constructor, reads the files found at the two input paths
    // and creates and links gl shader with the given source code.
    Shader(const std::string& _vertexPath, const std::string& _fragmentPath);

    // Setts this shader to be used for gl context.
    void use() const;

private:
    // ---------- Local functions --------------


public:
    // ---------- Getters / setters / Adders --------------

    // Adders

    // Setters
     // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setBool(const std::string& _name, bool _value) const
    {
        glUniform1i(glGetUniformLocation(ID, _name.c_str()), (int)_value);
    }
    // ------------------------------------------------------------------------

	// Sets uniform by name with value inputs.
    void setInt(const std::string& _name, int _value) const
    {
        glUniform1i(glGetUniformLocation(ID, _name.c_str()), _value);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setFloat(const std::string& _name, float _value) const
    {
        glUniform1f(glGetUniformLocation(ID, _name.c_str()), _value);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setVec2(const std::string& _name, const glm::vec2& _value) const
    {
        glUniform2fv(glGetUniformLocation(ID, _name.c_str()), 1, &_value[0]);
    }

    // Sets uniform by name with value inputs.
    void setVec2(const std::string& _name, float _x, float _y) const
    {
        glUniform2f(glGetUniformLocation(ID, _name.c_str()), _x, _y);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setVec3(const std::string& _name, const glm::vec3& _value) const
    {
        glUniform3fv(glGetUniformLocation(ID, _name.c_str()), 1, &_value[0]);
    }

    // Sets uniform by name with value inputs.
    void setVec3(const std::string& _name, float _x, float _y, float _z) const
    {
        glUniform3f(glGetUniformLocation(ID, _name.c_str()), _x, _y, _z);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setVec4(const std::string& _name, const glm::vec4& _value) const
    {
        glUniform4fv(glGetUniformLocation(ID, _name.c_str()), 1, &_value[0]);
    }

    // Sets uniform by name with value inputs.
    void setVec4(const std::string& _name, float _x, float _y, float _z, float _w) const
    {
        glUniform4f(glGetUniformLocation(ID, _name.c_str()), _x, _y, _z, _w);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setMat2(const std::string& _name, const glm::mat2& _mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setMat3(const std::string& _name, const glm::mat3& _mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }
    // ------------------------------------------------------------------------

    // Sets uniform by name with value inputs.
    void setMat4(const std::string& _name, const glm::mat4& _mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, _name.c_str()), 1, GL_FALSE, &_mat[0][0]);
    }
    
    // Getters
};