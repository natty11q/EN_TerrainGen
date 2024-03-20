#include <VBO.hpp>
#include <vector>




VBO::VBO(std::vector<Vertex>& vertices)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    /* params
        1 : type of buffer
        2 : total sise of the data in bytes
        3 : the actual data
        4 : the use of the data  // affects performance     
     */
    glBufferData(GL_ARRAY_BUFFER , vertices.size() * sizeof(Vertex)  , vertices.data() , GL_STATIC_DRAW);
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void VBO::Delete()
{
    glDeleteBuffers(1, &ID);
}

GLuint VBO::Get_id()
{
    return (ID);
}




HudVbo::HudVbo(std::vector<float>& vertices)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    /* params
        1 : type of buffer
        2 : total sise of the data in bytes
        3 : the actual data
        4 : the use of the data  // affects performance     
     */
    glBufferData(GL_ARRAY_BUFFER , vertices.size() * sizeof(float)  , vertices.data() , GL_STATIC_DRAW);
}

void HudVbo::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER,ID);
}

void HudVbo::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void HudVbo::Delete()
{
    glDeleteBuffers(1, &ID);
}

GLuint HudVbo::Get_id()
{
    return (ID);
}
