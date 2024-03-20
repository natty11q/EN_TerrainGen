#include "EBO.hpp"


EBO::EBO(std::vector<GLuint> & indices)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ID);
    /* params
        1 : type of buffer
        2 : total sise of the data in bytes
        3 : the actual data
        4 : the use of the data  // affects performance     
     */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER , indices.size() * sizeof(GLuint) , indices.data() , GL_STATIC_DRAW);
}

void EBO::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ID);
}

void EBO::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void EBO::Delete()
{
    glDeleteBuffers(1, &ID);
}

GLuint EBO::Get_id()
{
    return (ID);
}