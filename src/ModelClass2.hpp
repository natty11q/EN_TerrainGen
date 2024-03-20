#pragma  once


#include "TextureClass.hpp"
#include <glm/glm.hpp>
#include <vector>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate  | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices) // | aiProcess_GenSmoothNormals // | aiProcess_CalcTangentSpace

struct Mesh
{
    unsigned int VAO , VBO1, VBO2 ,VBO3 , EBO;
    // unsigned int VAO , VBO, EBO;  can combine into one buffer and send.

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    unsigned int tex_handle;
};

struct Texture_load
{
    unsigned int Tex_ID;
    std::string imageName;
};


class ModelLoader
{
public:
    ModelLoader(const char* path);
    ~ModelLoader() = default;

    void load_model();
    void load_model_cout_console();
    unsigned int m_numMeshes;
    std::vector<Mesh> m_MeshList;
    std::vector<Texture_load> m_TextureList;

private:
    Assimp::Importer m_importer;
    const aiScene* m_scene = nullptr;
    aiNode* m_rootNode = nullptr; // Only being used in the: load_model_cout_console() function
    
    std::string m_filePath;
    void set_buffer_data(unsigned int in);

    int is_image_loaded(std::string filename);
    unsigned int load_texture_image(std::string filename , bool& load_Complete);
};

