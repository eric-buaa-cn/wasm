#include "glslprogram.h"

#include <fstream>
#include <iostream>

using std::ifstream;
using std::ios;
using std::string;

#include <sstream>
#include <sys/stat.h>

namespace GLSLShaderInfo {
    struct ShaderFileExtension {
        const char *ext;
        GLSLShader::GLSLShaderType type;
    };

    struct ShaderFileExtension extensions[] =
    {
        {".vs",   GLSLShader::VERTEX},
        {".vert", GLSLShader::VERTEX},
        //{".gs",   GLSLShader::GEOMETRY},
        //{".geom", GLSLShader::GEOMETRY},
        //{".tcs",  GLSLShader::TESS_CONTROL},
        //{".tes",  GLSLShader::TESS_EVALUATION},
        {".fs",   GLSLShader::FRAGMENT},
        {".frag", GLSLShader::FRAGMENT},
        //{".cs",   GLSLShader::COMPUTE}
    };
}

GLSLProgram::GLSLProgram() : handle(0), linked(false) {}

void GLSLProgram::deleteShaders() {
    // Query the number of attached shaders
    GLint numShaders = 0;
    glGetProgramiv(handle, GL_ATTACHED_SHADERS, &numShaders);

    if (numShaders <= 0) {
        return;
    }

    // Get the shader names
    GLuint *shaderIDs = new GLuint[numShaders];
    glGetAttachedShaders(handle, numShaders, NULL, shaderIDs);

    // Delete the shaders
    for (int i = 0; i < numShaders; i++)
        glDeleteShader(shaderIDs[i]);

    delete[] shaderIDs;
}

GLSLProgram::~GLSLProgram() {
    if (handle == 0) return;

    // Delete the attached shaders of the program
    deleteShaders();

    // Delete the program
    glDeleteProgram(handle);
}

bool GLSLProgram::compileShader(const char *fileName) {
    std::cout << "shader: " << fileName << std::endl;
    int numberOfExtension = sizeof(GLSLShaderInfo::extensions) / sizeof(GLSLShaderInfo::ShaderFileExtension);

    // Check the file name's extension to determine the shader type
    string ext = getExtension(fileName);
    GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
    bool matchFound = false;
    for (int i = 0; i < numberOfExtension; i++) {
        if (ext == GLSLShaderInfo::extensions[i].ext) {
            matchFound = true;
            type = GLSLShaderInfo::extensions[i].type;
            break;
        }
    }

    // If we didn't find a match, throw an exception
    if (!matchFound) {
        string msg = "Unrecognized extension: " + ext;
        std::cout << msg << std::endl;
    }

    // Pass the discovered shader type along
    return compileShader(fileName, type);
}

string GLSLProgram::getExtension(const char *name) {
    string nameStr(name);

    size_t loc = nameStr.find_last_of('.');
    if (loc != string::npos) {
        return nameStr.substr(loc, string::npos);
    }
    return "";
}

bool GLSLProgram::compileShader(const char *fileName,
        GLSLShader::GLSLShaderType type) {
    if (!fileExists(fileName)) {
        string message = string("Shader: ") + fileName + " not found.";
        std::cout << "filename: " << fileName << " " << message << std::endl;
        return false;
    }

    if (handle <= 0) {
        handle = glCreateProgram();
        if (handle == 0) {
            std::cout << "filename: " << fileName << " Unable to create shader program." << std::endl;
            return false;
        }
    }

    ifstream inFile(fileName, ios::in);
    if (!inFile) {
        string message = string("Unable to open: ") + fileName;
        std::cout << "filename: " << fileName << " " << message << std::endl;
        return false;
    }

    // Get file contents
    std::stringstream code;
    code << inFile.rdbuf();
    inFile.close();

    std::cout << "shader content: " << code.str() << std::endl;

    return compileShader(code.str(), type, fileName);
}

bool GLSLProgram::compileShader(const string &source,
        GLSLShader::GLSLShaderType type,
        const char *fileName) {
    if (handle <= 0) {
        handle = glCreateProgram();
        if (handle == 0) {
            std::cout << "filename: " << fileName << " Unable to create shader program. " << std::endl;
            return false;
        }
    }

    GLuint shaderHandle = glCreateShader(type);

    const char *c_code = source.c_str();
    glShaderSource(shaderHandle, 1, &c_code, NULL);

    // Compile the shader
    glCompileShader(shaderHandle);

    // Check for errors
    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if (GL_FALSE == result) {
        // Compile failed, get log
        int length = 0;
        string logString;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetShaderInfoLog(shaderHandle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }
        string msg;
        if (fileName) {
            msg = string(fileName) + ": shader compliation failed\n";
        } else {
            msg = "Shader compilation failed.\n";
        }
        msg += logString;
        std::cout << "filename: " << fileName << " " << msg << std::endl;
        return false;

    } else {
        // Compile succeeded, attach shader
        glAttachShader(handle, shaderHandle);
    }

    return true;
}

bool GLSLProgram::link() {
    if (linked) return true;
    if (handle <= 0) {
        std::cout << "Program has not been compiled." << std::endl;
        return false;
    }

    glLinkProgram(handle);

    int status = 0;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
        // Store log and return false
        int length = 0;
        string logString;

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }

        std::cout << string("Program link failed:\n") << logString << std::endl;
        return false;
    }

    findUniformLocations();
    linked = true;
    deleteShaders();

    return true;
}

void GLSLProgram::findUniformLocations() {
    uniformLocations.clear();

    GLint numUniforms = 0;
//#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveUniform
    GLint maxLen;
    GLchar *name;

    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numUniforms);

    name = new GLchar[maxLen];
    for (GLuint i = 0; i < numUniforms; ++i) {
        GLint size;
        GLenum type;
        GLsizei written;
        glGetActiveUniform(handle, i, maxLen, &written, &size, &type, name);
        GLint location = glGetUniformLocation(handle, name);
        uniformLocations[name] = glGetUniformLocation(handle, name);
    }
    delete[] name;
//#else
//    // For OpenGL 4.3 and above, use glGetProgramResource
//    glGetProgramInterfaceiv( handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
//
//    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};
//
//    for( GLint i = 0; i < numUniforms; ++i ) {
//        GLint results[4];
//        glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);
//
//        if( results[3] != -1 ) continue;  // Skip uniforms in blocks
//        GLint nameBufSize = results[0] + 1;
//        char * name = new char[nameBufSize];
//        glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name);
//        uniformLocations[name] = results[2];
//        delete [] name;
//    }
//#endif
}

void GLSLProgram::use() {
    if (handle <= 0 || (!linked))
        throw GLSLProgramException("Shader has not been linked");
    glUseProgram(handle);
}

int GLSLProgram::getHandle() {
    return handle;
}

bool GLSLProgram::isLinked() {
    return linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char *name) {
    glBindAttribLocation(handle, location, name);
}

//void GLSLProgram::bindFragDataLocation(GLuint location, const char *name) {
//    glBindFragDataLocation(handle, location, name);
//}

void GLSLProgram::setUniform(const char *name, float x, float y, float z) {
    GLint loc = getUniformLocation(name);
    glUniform3f(loc, x, y, z);
}

void GLSLProgram::setUniform(const char *name, const glm::vec3 &v) {
    this->setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char *name, const glm::vec4 &v) {
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void GLSLProgram::setUniform(const char *name, const glm::vec2 &v) {
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}

void GLSLProgram::setUniform(const char *name, const glm::mat4 &m) {
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char *name, const glm::mat3 &m) {
    GLint loc = getUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char *name, float val) {
    GLint loc = getUniformLocation(name);
    glUniform1f(loc, val);
}

void GLSLProgram::setUniform(const char *name, int val) {
    GLint loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void GLSLProgram::setUniform(const char *name, GLuint val) {
    GLint loc = getUniformLocation(name);
    glUniform1ui(loc, val);
}

void GLSLProgram::setUniform(const char *name, bool val) {
    int loc = getUniformLocation(name);
    glUniform1i(loc, val);
}

void GLSLProgram::printActiveUniforms() {
//#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveUniform
    GLint nUniforms, size, location, maxLen;
    GLchar *name;
    GLsizei written;
    GLenum type;

    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &nUniforms);

    name = new GLchar[maxLen];

    printf("Active uniforms:\n");
    printf("------------------------------------------------\n");
    for (GLuint i = 0; i < nUniforms; ++i) {
        glGetActiveUniform(handle, i, maxLen, &written, &size, &type, name);
        location = glGetUniformLocation(handle, name);
        printf(" %-5d %s (%s)\n", location, name, getTypeString(type));
    }

    delete[] name;
//#else
//    // For OpenGL 4.3 and above, use glGetProgramResource
//    GLint numUniforms = 0;
//    glGetProgramInterfaceiv( handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
//
//    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};
//
//    printf("Active uniforms:\n");
//    for( int i = 0; i < numUniforms; ++i ) {
//        GLint results[4];
//        glGetProgramResourceiv(handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);
//
//        if( results[3] != -1 ) continue;  // Skip uniforms in blocks
//        GLint nameBufSize = results[0] + 1;
//        char * name = new char[nameBufSize];
//        glGetProgramResourceName(handle, GL_UNIFORM, i, nameBufSize, NULL, name);
//        printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
//        delete [] name;
//    }
//#endif
}

void GLSLProgram::printActiveUniformBlocks() {
//#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveUniformBlockiv
    GLint written, maxLength, maxUniLen, nBlocks, binding;
    GLchar *name;

    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCKS, &nBlocks);
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniLen);
    GLchar *uniName = new GLchar[maxUniLen];
    name = new GLchar[maxLength];

    printf("Active Uniform blocks: \n");
    printf("------------------------------------------------\n");
    for (GLuint i = 0; i < nBlocks; i++) {
        glGetActiveUniformBlockName(handle, i, maxLength, &written, name);
        glGetActiveUniformBlockiv(handle, i, GL_UNIFORM_BLOCK_BINDING, &binding);
        printf("Uniform block \"%s\" (%d):\n", name, binding);

        GLint nUnis;
        glGetActiveUniformBlockiv(handle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &nUnis);
        GLint *unifIndexes = new GLint[nUnis];
        glGetActiveUniformBlockiv(handle, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, unifIndexes);

        for (int unif = 0; unif < nUnis; ++unif) {
            GLuint uniIndex = unifIndexes[unif];
            GLint size;
            GLenum type;

            glGetActiveUniform(handle, uniIndex, maxUniLen, &written, &size, &type, uniName);
            printf("    %s (%s)\n", name, getTypeString(type));
        }

        delete[] unifIndexes;
    }
    delete[] name;
    delete[] uniName;
//#else
//    GLint numBlocks = 0;
//
//    glGetProgramInterfaceiv(handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
//    GLenum blockProps[] = {GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH};
//    GLenum blockIndex[] = {GL_ACTIVE_VARIABLES};
//    GLenum props[] = {GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX};
//
//    for(int block = 0; block < numBlocks; ++block) {
//        GLint blockInfo[2];
//        glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
//        GLint numUnis = blockInfo[0];
//
//        char * blockName = new char[blockInfo[1]+1];
//        glGetProgramResourceName(handle, GL_UNIFORM_BLOCK, block, blockInfo[1]+1, NULL, blockName);
//        printf("Uniform block \"%s\":\n", blockName);
//        delete [] blockName;
//
//        GLint * unifIndexes = new GLint[numUnis];
//        glGetProgramResourceiv(handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);
//
//        for( int unif = 0; unif < numUnis; ++unif ) {
//            GLint uniIndex = unifIndexes[unif];
//            GLint results[3];
//            glGetProgramResourceiv(handle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);
//
//            GLint nameBufSize = results[0] + 1;
//            char * name = new char[nameBufSize];
//            glGetProgramResourceName(handle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
//            printf("    %s (%s)\n", name, getTypeString(results[1]));
//            delete [] name;
//        }
//
//        delete [] unifIndexes;
//    }
//#endif
}

void GLSLProgram::printActiveAttribs() {
//#ifdef __APPLE__
    // For OpenGL 4.1, use glGetActiveAttrib
    GLint written, size, location, maxLength, nAttribs;
    GLenum type;
    GLchar *name;

    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
    glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &nAttribs);

    name = new GLchar[maxLength];
    printf("Active Attributes: \n");
    printf("------------------------------------------------\n");
    for (int i = 0; i < nAttribs; i++) {
        glGetActiveAttrib(handle, i, maxLength, &written, &size, &type, name);
        location = glGetAttribLocation(handle, name);
        printf(" %-5d %s (%s)\n", location, name, getTypeString(type));
    }
    delete[] name;
//#else
//    // >= OpenGL 4.3, use glGetProgramResource
//    GLint numAttribs;
//    glGetProgramInterfaceiv( handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
//
//    GLenum properties[] = {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION};
//
//    printf("Active attributes:\n");
//    for( int i = 0; i < numAttribs; ++i ) {
//        GLint results[3];
//        glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);
//
//        GLint nameBufSize = results[0] + 1;
//        char * name = new char[nameBufSize];
//        glGetProgramResourceName(handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
//        printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
//        delete [] name;
//    }
//#endif
}

const char *GLSLProgram::getTypeString(GLenum type) {
    // There are many more types than are covered here, but
    // these are the most common in these examples.
    switch (type) {
        case GL_FLOAT:
            return "float";
        case GL_FLOAT_VEC2:
            return "vec2";
        case GL_FLOAT_VEC3:
            return "vec3";
        case GL_FLOAT_VEC4:
            return "vec4";
        case GL_DOUBLE:
            return "double";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_INT:
            return "unsigned int";
        case GL_BOOL:
            return "bool";
        case GL_FLOAT_MAT2:
            return "mat2";
        case GL_FLOAT_MAT3:
            return "mat3";
        case GL_FLOAT_MAT4:
            return "mat4";
        default:
            return "?";
    }
}

bool GLSLProgram::validate() {
    if (!isLinked()) {
        std::cout << "Program is not linked" << std::endl;
        return false;
    }

    GLint status;
    glValidateProgram(handle);
    glGetProgramiv(handle, GL_VALIDATE_STATUS, &status);

    if (GL_FALSE == status) {
        // Store log and return false
        int length = 0;
        string logString;

        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
            char *c_log = new char[length];
            int written = 0;
            glGetProgramInfoLog(handle, length, &written, c_log);
            logString = c_log;
            delete[] c_log;
        }

        std::cout << string("Program failed to validate\n") + logString << std::endl;
        return false;
    }
    return true;
}

int GLSLProgram::getUniformLocation(const char *name) {
    auto pos = uniformLocations.find(name);

    if (pos == uniformLocations.end()) {
        uniformLocations[name] = glGetUniformLocation(handle, name);
    }

    return uniformLocations[name];
}

bool GLSLProgram::fileExists(const string &fileName) {
    struct stat info;
    int ret = -1;

    ret = stat(fileName.c_str(), &info);
    return 0 == ret;
}
