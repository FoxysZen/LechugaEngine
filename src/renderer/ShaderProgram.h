#pragma once
#include <FileSystem.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <Logger.h>
#include <unordered_map>

class ShaderProgram
{
    public:
        ShaderProgram();
        ~ShaderProgram();

        void load(std::string vertPath, std::string fragPath);
        void bind();
        void unbind();
        void setUniformMat4(const std::string &name, const glm::mat4 &matrix);
        void setUniformVec3(const std::string &name, const glm::vec3 &vec);
        void setUniformInt(const std::string &name, int value);

    private:
        GLuint program;
        std::unordered_map<std::string, GLint> uniformLocs;
};