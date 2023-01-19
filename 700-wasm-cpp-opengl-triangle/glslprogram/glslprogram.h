#pragma once

/* ------------------------------------------------
https://github.com/daw42/glslcookbook/blob/master/ingredients/glslprogram.h
-------------------------------------------------*/
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>

//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//#include <glad/glad.h>

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <stdexcept>

class GLSLProgramException : public std::runtime_error {
    public:
        GLSLProgramException(const std::string &msg) :
            std::runtime_error(msg) {}
};

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
    };
};

class GLSLProgram {
    private:
        GLuint handle;
        bool linked;
        std::map<std::string, int> uniformLocations;

        GLint getUniformLocation(const char *name);

        bool fileExists(const std::string &fileName);

        std::string getExtension(const char *fileName);

        // Make these private in order to make the object non-copyable
        GLSLProgram(const GLSLProgram &other) {}

        GLSLProgram &operator=(const GLSLProgram &other) { return *this; }

        void deleteShaders();

    public:
        GLSLProgram();

        ~GLSLProgram();

        bool compileShader(const char *fileName);

        bool compileShader(const char *fileName, GLSLShader::GLSLShaderType type);

        bool compileShader(const std::string &source, GLSLShader::GLSLShaderType type,
                const char *fileName = NULL);

        bool link();

        bool validate();

        void use();

        int getHandle();

        bool isLinked();

        void bindAttribLocation(GLuint location, const char *name);

//        void bindFragDataLocation(GLuint location, const char *name);

        void setUniform(const char *name, float x, float y, float z);

        void setUniform(const char *name, const glm::vec2 &v);

        void setUniform(const char *name, const glm::vec3 &v);

        void setUniform(const char *name, const glm::vec4 &v);

        void setUniform(const char *name, const glm::mat4 &m);

        void setUniform(const char *name, const glm::mat3 &m);

        void setUniform(const char *name, float val);

        void setUniform(const char *name, int val);

        void setUniform(const char *name, bool val);

        void setUniform(const char *name, GLuint val);

        void findUniformLocations();

        void printActiveUniforms();

        void printActiveUniformBlocks();

        void printActiveAttribs();

        const char *getTypeString(GLenum type);
};
