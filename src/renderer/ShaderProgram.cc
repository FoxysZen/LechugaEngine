#include <ShaderProgram.h>

ShaderProgram::ShaderProgram() {}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(program);
}

void ShaderProgram::load(std::string vertPath, std::string fragPath)
{
    std::string vertCode = FileSystem::readFile(vertPath);
    std::string fragCode = FileSystem::readFile(fragPath);

    const char *vertSrc = vertCode.c_str();
    const char *fragSrc = fragCode.c_str();

    GLint status;
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSrc, nullptr);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        Logger::error("ShaderProgram: Could not compile Vertex Shader.");
    }

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, nullptr);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        Logger::error("ShaderProgram: Could not compile Fragment Shader.");
    }

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        Logger::error("ShaderProgram: Program could not be linked.");
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ShaderProgram::bind()
{
    glUseProgram(program);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

void ShaderProgram::setUniformMat4(const std::string &name, 
                                   const glm::mat4 &matrix)
{
    if (uniformLocs.find(name) == uniformLocs.end())
    {
        uniformLocs[name] = glGetUniformLocation(program, name.c_str());
    }
    glUniformMatrix4fv(uniformLocs[name], 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::setUniformVec3(const std::string &name, 
                                   const glm::vec3 &vec)
{
    if (uniformLocs.find(name) == uniformLocs.end())
    {
        uniformLocs[name] = glGetUniformLocation(program, name.c_str());
    }
    glUniform3fv(uniformLocs[name], 1, &vec[0]);
}

void ShaderProgram::setUniformInt(const std::string &name, int value)
{
    if (uniformLocs.find(name) == uniformLocs.end())
    {
        uniformLocs[name] = glGetUniformLocation(program, name.c_str());
    }
    glUniform1i(uniformLocs[name], value);
}

void ShaderProgram::setUniformFloat(const std::string &name, float value)
{
    if (uniformLocs.find(name) == uniformLocs.end())
    {
        uniformLocs[name] = glGetUniformLocation(program, name.c_str());
    }
    glUniform1f(uniformLocs[name], value);
}
