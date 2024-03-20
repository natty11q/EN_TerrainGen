#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include <VBO.hpp>

class VAO
{
public:
	VAO();
        void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride , void* offset);

        void Bind();
        void Unbind();
        void Delete();

        GLuint Get_id();
        GLuint* Get_idPtr();

        void Set_id(GLuint newID);

protected:
        GLuint ID;

};




class HudVao : public VAO 
{
public:
        HudVao();
        ~HudVao();
        void LinkAttrib(HudVbo& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride , void* offset);
};


#endif