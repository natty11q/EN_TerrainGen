#include "ShaderClass.hpp"
#include "Utility.hpp"
#include <string>


ShaderClass::ShaderClass(const char* vertexFile, const char* fragmentFile)
{
    std::string vertexCode = "";
    std::string fragmentCode = "";

    std::cout << "fetching Vertex Code ... " << std::endl;
    vertexCode   = get_file_contents(vertexFile);
    if  (vertexCode == "") {std::cerr << "Error: Could not open/find the file " << vertexFile << std::endl;  exit(-1);}
    std::cout << "got vertex shader Code : SUCSESS" << std::endl;

    std::cout << "fetching Fragment Code ... " << std::endl;
    fragmentCode = get_file_contents(fragmentFile);
    if  (fragmentCode == "") {std::cerr << "Error: Could not open/find the file " << vertexFile << std::endl;  exit(-1);}
    std::cout << "got fragment shader Code : SUCSESS" << std::endl;

    const char* vertexSource   = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    this->fragmentFile = fragmentFile;
    this->vertexFile = vertexFile;


    // create the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    /* params:
        1 : reference Value
        2 : number of strings used
        3 : pointer to the source coude
        4 : ???????? */

    glShaderSource(vertexShader , 1 , &vertexSource , NULL);
    glCompileShader(vertexShader); // compile it now so that it can be sent to the gpu 

    compileErrors(vertexShader, "VERTEX");

    // do the same for the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader , 1 , &fragmentSource , NULL);
    glCompileShader(fragmentShader);

    compileErrors(fragmentShader, "FRAGMENT");


    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    glLinkProgram(ID);

    compileErrors(ID, "PROGRAM");


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderClass::Activate()
{
    glUseProgram(ID);
}


void ShaderClass::Delete()
{
    glDeleteProgram(ID);
}


GLuint ShaderClass::Get_id()
{
    return (ID);
}

// Checks if the different Shaders have compiled properly
void ShaderClass::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];

    std::string program = "PROGRAM"; // was gettign a warning
	if (type != program)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		} else
        {
            std::cout << "SHADERS COMPILED SUCSESSFULLY" << std::endl;
        }
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}


std::string ShaderClass::getFragmentSource(){
    return this->fragmentFile;
}
std::string ShaderClass::getVertexSource(){
    return this->vertexFile;
}