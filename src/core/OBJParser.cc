#include <OBJParser.h>

std::map<std::string, std::vector<float>> OBJParser::getVertices(std::string path)
{
    std::string file = FileSystem::readFile(path);
    std::istringstream stream(file);
    std::string line;
    std::map<std::string, std::vector<float>> groups;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::string currentMaterial = "default";

    while (std::getline(stream, line))
    {
        if (line.substr(0, 2) == "v ") // Vertex
        {
            std::istringstream ss(line);
            std::string prefix;
            float x, y, z;
            ss >> prefix >> x >> y >> z;
            positions.push_back(glm::vec3(x, y, z));
        }
        else if (line.substr(0, 3) == "vn ") // Normal
        {
            std::istringstream ss(line);
            std::string prefix;
            float x, y, z;
            ss >> prefix >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if (line.substr(0, 3) == "vt ") // UV
        {
            std::istringstream ss(line);
            std::string prefix;
            float u, v;
            ss >> prefix >> u >> v;
            uvs.push_back(glm::vec2(u, 1.0f - v));
        }
        else if (line.substr(0, 7) == "usemtl ") // Texture
        {
            currentMaterial = line.substr(7);
        }
        else if (line.substr(0, 2) == "f ") // Face
        {
            std::istringstream ss(line);
            std::string prefix, v1, v2, v3;
            ss >> prefix >> v1 >> v2 >> v3;

            for (const std::string& vert : {v1, v2, v3})
            {
                int slash1 = vert.find('/');
                int slash2 = vert.find('/', slash1 + 1);

                int posIdx = std::stoi(vert.substr(0, slash1)) - 1;
                int nIdx   = std::stoi(vert.substr(slash2 + 1)) - 1;
                int uvIdx  = std::stoi(vert.substr(slash1 + 1, slash2 - slash1 - 1)) - 1;

                glm::vec3 pos  = positions[posIdx];
                glm::vec3 norm = normals[nIdx];
                glm::vec2 uv   = uvs[uvIdx];

                groups[currentMaterial].push_back(pos.x);
                groups[currentMaterial].push_back(pos.y);
                groups[currentMaterial].push_back(pos.z);
                groups[currentMaterial].push_back(norm.x);
                groups[currentMaterial].push_back(norm.y);
                groups[currentMaterial].push_back(norm.z);
                groups[currentMaterial].push_back(uv.x);
                groups[currentMaterial].push_back(uv.y);
            }
        }
    }

    return groups;
}