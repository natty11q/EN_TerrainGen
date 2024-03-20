#ifndef EBO_HPP
#define EBO_HPP

#include <glad/glad.h>
#include <vector>

class EBO
{
	public:
		EBO(std::vector<GLuint> & indices);

        void Bind();
        void Unbind();
        void Delete();

        GLuint Get_id();

	private:
        GLuint ID;

};


#endif