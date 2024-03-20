#ifndef SHADERCLASS_HPP
#define SHADERCLASS_HPP

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iostream>
#include <cerrno>



std::string get_file_contents(const char* filename);

class ShaderClass
{
	public:
		ShaderClass(const char* vertexFile, const char* fragmentFile);
		

		void Activate();
		void Delete();

		std::string getVertexSource();
		std::string getFragmentSource();
		
		GLuint Get_id();

		GLuint ID;
	private:
		void compileErrors(unsigned int shader, const char* type);
		std::string vertexFile;
		std::string fragmentFile;
};

#endif