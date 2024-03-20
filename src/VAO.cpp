#include "VAO.hpp"

VAO::VAO()
{
    glGenVertexArrays (1 , &ID); // must be generated BEFORE the VBO 
}



void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride , void* offset)
{
    VBO.Bind();

    /* params [a way of communicating with a vertex shader from outside]
        1 :  position of the vetex attibute
        2 :  how many values per vertex
        3 :  value type
        4  -  applies when some vetices are int
        5 : amount of data between each vertex
        6 : offset , pointer to wehre vetices begin in the array
    */
    glVertexAttribPointer(layout , numComponents , type , GL_FALSE , stride , offset );

    glEnableVertexAttribArray(layout); // 1: the position of the vertex attribute 

    VBO.Unbind();
}

void VAO::Bind()
{
    glBindVertexArray(ID);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1 , &ID);
}


void VAO::Set_id(GLuint newID)
{
    this->ID = newID;
}

GLuint VAO::Get_id()
{
    return (ID);
}

GLuint* VAO::Get_idPtr()
{
    return (&ID);
}


HudVao::HudVao(){glGenVertexArrays (1 , &ID);}
HudVao::~HudVao(){}
void HudVao::LinkAttrib(HudVbo &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset)
{
    VBO.Bind();

    glVertexAttribPointer(layout , numComponents , type , GL_FALSE , stride , offset );

    glEnableVertexAttribArray(layout); // 1: the position of the vertex attribute 

    VBO.Unbind();
}