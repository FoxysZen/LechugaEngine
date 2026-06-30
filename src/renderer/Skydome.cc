#include <Skydome.h>

Skydome::Skydome()
{

}

Skydome::~Skydome()
{

}

void Skydome::init(ShaderProgram *shader, Texture *texture)
{
    this->shader = shader;
    this->texture = texture;
    
    std::vector<float> vertices = generateSphere(500.0f, 16, 32);
    vertexCount = vertices.size() / 5;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
        vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Skydome::draw(const glm::mat4 &view, const glm::mat4 &proj, float deltaTime)
{
    time += deltaTime * scrollSpeed;
    if (time > 1.0f) time -= 1.0f;
    
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    
    shader->bind();
    shader->setUniformMat4("view", view);
    shader->setUniformMat4("proj", proj);
    shader->setUniformFloat("time", time);
    shader->setUniformInt("skyTexture", 0);
    texture->bind(0);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    
    texture->unbind(0);
    shader->unbind();
    
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

std::vector<float> Skydome::generateSphere(float radius, int rings, int sectors)
{
    const float PI = glm::pi<float>();
    std::vector<float> vertices;
    vertices.reserve(rings * (sectors + 1) * 6 * 5);

    auto addVertex = [&](int i, int j) {
        float u = (float)j / (float)sectors;
        float v = (float)i / (float)rings;
        float x = sin(v * PI) * cos(u * 2 * PI);
        float y = cos(v * PI);
        float z = sin(v * PI) * sin(u * 2 * PI);
        vertices.push_back(x * radius);
        vertices.push_back(y * radius);
        vertices.push_back(z * radius);
        vertices.push_back(u);
        vertices.push_back(v);
    };

    for (int i = 0; i < rings; i++)
    {
        for (int j = 0; j < sectors; j++)
        {
            addVertex(i, j);
            addVertex(i + 1, j);
            addVertex(i, j + 1);
            addVertex(i, j + 1);
            addVertex(i + 1, j);
            addVertex(i + 1, j + 1);
        }
    }
    return vertices;
}
