#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>


struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 normal;
    glm::vec3 Colour;
    glm::vec2 TexUV;
};



class VBO
{
	public:
        // VBO();
		VBO(std::vector<Vertex>& vertices);

        void Bind();
        void Unbind();
        void Delete();

        GLuint Get_id();

	protected:
        GLuint ID;

};


class HudVbo
{
public:

		HudVbo(std::vector<float>& vertices);

        void Bind();
        void Unbind();
        void Delete();

        GLuint Get_id();

protected:
        GLuint ID;
};

