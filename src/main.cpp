#include "ShaderClass.hpp"
#include "GLFW/glfw3.h"
// #include <__algorithm/remove_if.h>
// #include <cctype>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <glm/glm.hpp>
#include <ratio>
#include <string>
#include <vector>

// namespace fs = std::filesystem;

#include <iostream>
#include <future>
#include <noise.h>

#include "ModelClass.hpp"
#include "TextureClass.hpp"
#include "Utility.hpp"
#include "VBO.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <VAO.hpp>
// #include "VBO.hpp"
// #include "glm/fwd.hpp"
// #include "glm/geometric.hpp"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <SFML/Audio.hpp>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)





//this was more tedious that you know ...
int WIDTH   = 1920 / 3;
const unsigned int WINDOW_WIDTH  = trunc( WIDTH  );
int HEIGHT  = 1920 / 3;
const unsigned int WINDOW_HEIGHT = trunc( HEIGHT );



const float Gravity = 98.1f;

// std::vector<Vertex> Line(glm::vec3 start,glm::vec3 end, int div)
// {

// }

struct light
{
	glm::vec4 Lightcolour;
	glm::vec3 Lightpos;
	glm::mat4 LightModel;
};




class Hud
{
public:
	Hud(std::vector<Texture> Tex_in = {})
	{
		this->opacity = 1.0f;
		this->textures = Tex_in;
		GpuStuff();
	}

	~Hud(){}

	void GpuStuff(){

		VAO.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		HudVbo VBO(this->vertices);
		// Generates Element Buffer Object and links it to indices
		
		EBO EBO(this->indices);
		
		// Links VBO attributes such as coordinates and colors to VAO
		int vertexSize = sizeof(float) * 4;
		VAO.LinkAttrib(VBO, 0, 2, GL_FLOAT, vertexSize , (void*)0);
		VAO.LinkAttrib(VBO, 1, 2, GL_FLOAT, vertexSize , (void*)(2 * sizeof(float)));

		// Unbind all to prevent accidentally modifying them
		VAO.Unbind();
		VBO.Unbind();
		
		EBO.Unbind();

	}

	void Draw(
		ShaderClass& shader, 
		Camera& camera
	)
	{
		shader.Activate();
		VAO.Bind();

				// Keep track of how many of each type of textures we have
		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string num;
			std::string type = textures[i].type;
			if (type == "diffuse")
			{
				num = std::to_string(numDiffuse++);
			}
			else if (type == "specular")
			{
				num = std::to_string(numSpecular++);
			}
			textures[i].texUnit(shader, (type + num).c_str(), i);
			textures[i].Bind();
		}

		
		glUniform1f( glGetUniformLocation(shader.ID, "aspectRatio") , camera.AspectRatio);
		glUniform1f( glGetUniformLocation(shader.ID, "HudOpacity") , this->opacity);
		glUniform2f( glGetUniformLocation(shader.ID, "Dimensions") , camera.width , camera.height );

		// Draw the actual mesh
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			textures[i].Unbind();
		}
	}
private:

	std::vector<float> vertices = 
	{
		-1.0f,-1.0f , 1.0f, 1.0f,
		1.0f,-1.0f  , 1.0f, 1.0f,
		1.0f,1.0f   , 1.0f, 1.0f,
		-1.0f,1.0f, 1.0f, 1.0f,

	};

	std::vector<GLuint> indices =
	{
		0, 1, 3, 1, 2, 3
	};

	HudVao VAO;
	float opacity;
	std::vector<Texture> textures;
};


std::vector<Vertex> createPlaneVertices(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int divs, float scale = 1000.0f)
{
	int count = 0;
	std::vector<Vertex> plane;
	float div_f = (float)divs; 

	glm::vec3 vec03 = (v3 - v0) / div_f;
	glm::vec3 vec12 = (v2 - v1) / div_f;
	for (int rows = 0; rows < divs + 1; rows++) {

		float row = (float)rows;
		glm::vec3 start     = v0 + vec03 * row;
		glm::vec3 end       = v1 + vec12 * row;
		glm::vec3 vectorDiv = (end - start) / div_f;

		for (int col = 0; col < divs + 1; col++)
		{

			float col_f = (float)col;

			glm::vec3 crntVec = start + vectorDiv * col_f;
			// Position
			glm::vec3 Position(crntVec.x,crntVec.y,crntVec.z);
			// Tex UVs
			glm::vec2 TextureCoord(float(col) / div_f,float (row) / div_f);			
			// langent space vectors
			glm::vec3 tangent = glm::normalize(vectorDiv);

			glm::vec3 nextStart = start + vec03;
			glm::vec3 nextEnd = end + vec12;
			glm::vec3 nextVectorDiv = (nextEnd - nextStart) / div_f;
			glm::vec3 nextRowCrntVec = nextStart + nextVectorDiv * col_f;
			glm::vec3 possibleBitangent = glm::normalize(nextRowCrntVec - crntVec);

			// Normal
			glm::vec3 Normal = cross(tangent, possibleBitangent);

			plane.push_back(Vertex{crntVec * scale, Normal,   glm::vec3(0.2,1,0.5),TextureCoord} );

			// count++;
			// std::cout << "generated vetex no : " <<  count << "\n"; 
		}
	}
	return plane;
}

// std::vector<Vertex> createSphere(int divs, float scale = 1000.0f)
// {
// 	std::vector<Vertex> sphere;
// 	float pi = acos(-1.0f);
// 	float radius = 1.0f / (divs / 2); //radius of the circle that makes up one half of the sphere

// 	for (int i = 0; i <= divs; ++i) 
// 	{
// 		float a1 = i * ((2 * pi) / divs); // theta
// 		float r1 = radius * cos(a1);     // x
// 		float y1 = radius * sin(a1);     // y

// 		float a2 = (i + 1 == divs ? 0 : (i + 1)) * ((2 * pi	) / divs); // phi
// 		float r2 = radius * sin(a2);     // z
// 		float x2 = r1 *  cos(a2);        // x
// 	}

// }

std::vector<Vertex> SimpleVertGen(int divs, float scale = 1000.0f,float height = 1.0f)
{
	/*Height parameter is in reference to the Y axis and is a scalar multiple of the width so a height of 1.5 and scale 2 gives a 2 x 3 plane*/ 
	if(!(divs > 0))	return  std::vector<Vertex>(); // return empty if the number of divisions is not greater than zero

	std::vector<Vertex> Vertices;
	std::vector<GLuint> indices;


	glm::vec3 normal(0.0f,1.0f,0.0f);
	glm::vec3 colour(1.0f,1.0f,1.0f);
	
	for (float col = 0.0f; col < divs + 1; col += 1)
	{
		for(float row = 0.0f; row < ( divs + 1 ) * height ; row += 1)
		{
			float x;
			float z;
			x = (col / divs) * scale;
			z = (row / divs) * scale;
			glm::vec3 position(x , 0.0f , z);

			// std::cout  << "Position: \n"<<position.x<<"\t"<<position.y<<"\t"<<position.z<<"\n";
			glm::vec2 TextureCoord(float(col) / divs,float (row) / divs );

			Vertices.push_back(Vertex{position,normal,colour,TextureCoord});

			// unsigned int index = row * (divs + 1) + col;
			// // Top triangle
			// indices. push_back(index);
			// indices. push_back(index + (divs + 1) + 1);
			// indices. push_back(index + (divs + 1));
			// // Bot triangle
			// indices. push_back(index);
			// indices. push_back(index + 1);
			// indices. push_back(index + (divs + 1) + 1);

		}
	}
	return Vertices;
}


std::vector<GLuint> generatePlaneIndices(int div)
{
	std::vector<GLuint> indices;
	for (int row = 0; row < div; row++){
		for (int col = 0; col < div; col++){
			unsigned int index = row * (div + 1) + col;
			// Top triangle
			indices. push_back(index);
			indices. push_back(index + (div + 1) + 1);
			indices. push_back(index + (div + 1));
			// Bot triangle
			indices. push_back(index);
			indices. push_back(index + 1);
			indices. push_back(index + (div + 1) + 1);
		}
	}

	std::cout << "indices Size : " << indices.size() << std::endl;
	return indices;
	
}


class PlayerPhys
{
	public:
	PlayerPhys(Camera* cam)
	{
		this->Camera = cam;
		this->speed = this->BaseAccel;
		this->current_Velocity = glm::vec3(0.0f,0.0f,0.0f);
	}
	~PlayerPhys(){}

	void jump()
	{
		if(this->grounded)
		{
			this->current_Velocity += this->jumpHeight * this->Up;
		}
	}

	void capVelo()
	{
		// glm::vec3 tempVelo = this->current_Velocity;
		this->current_Velocity[0] = std::max( abs(this->current_Velocity[0]) - this->decel ,  0.0f ) * (0.5 - std::signbit(this->current_Velocity[0])) * 2 ;
		// this->current_Velocity[0] = fmin(abs(this->current_Velocity[0]) , this->MaxVelosity ) * (this->current_Velocity[0] / abs(this->current_Velocity[0]) );
		this->current_Velocity[2] = std::max( abs(this->current_Velocity[2]) - this->decel ,  0.0f ) * (0.5 - std::signbit(this->current_Velocity[0])) * 2 ;
		// this->current_Velocity[2] = fmin(abs(this->current_Velocity[2]) , this->MaxVelosity ) * (this->current_Velocity[2] / abs(this->current_Velocity[2]) );
		
		// this->current_Velocity[2] = std::min(abs(this->current_Velocity[1]) , 2.0f);
	}
	void UpdatePositions(float deltatime)
	{
		// std::cout << "deltatime : " << deltatime << "\n";
		if(!this->fly)
		{
			if (!grounded )
			{
				this->current_Velocity += Gravity * - this->Up;
			}
			this->capVelo();
			this->Camera->Position  += this->current_Velocity * deltatime;
		}
		if (this->fly)
		{
			this->Camera->Position  += this->current_Velocity * deltatime * 100.0f;
			this->current_Velocity = glm::vec3(0.0f,0.0f,0.0f);
		}
		#if 0
			std::cout << "campos = ";
			std::cout << this->Camera->Position[0] << " : x , "; 
			std::cout << this->Camera->Position[1] << " : y , "; 
			std::cout << this->Camera->Position[2] << " : z , "; 
			std::cout << "\n"; 
		#endif
	}

	void floorColision(std::vector<glm::vec3> floorV,std::vector<glm::vec3> floorN)
	{
		this->grounded = false;
		for (int i = 0 ; i < floorV.size();i++)
		{
			if  ((this->Camera->Position - floorV[i]).length() > this->PlayerRadius)
			{
				continue;
			}
			glm::vec3 vertex = floorV[i];
			glm::vec3 normal = glm::normalize(floorN[i]);
			float len = glm::length(this->Camera->Position - vertex);
			if ( len <= this->PlayerRadius + 0.5)
			{
				this->grounded = true;
				this->decel = 10.0f;
				
				if (len <= this->PlayerRadius)
				{
					this->Camera->Position +=  this->Up * (this->PlayerRadius - len) *  this->PlayerRadius * abs(cosf(glm::angle(normal,this->Up))); // 0.f mustbe specified float
					this->current_Velocity[1] = 0.0f;

				}
			}
			this->decel = 1.0f;
		}
	}


	void handleInputs(GLFWwindow* window,float deltatime)
	{
		this->direction = this->Camera->Orientation;
		
		// Handles key inputs

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			this->current_Velocity += this->speed * this->direction;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			this->current_Velocity += this->speed * -glm::normalize(glm::cross(this->direction, this->Up));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			this->current_Velocity += this->speed * -this->direction;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			this->current_Velocity += this->speed * glm::normalize(glm::cross(this->direction, this->Up));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (!this->fly)
			{
				this->jump();
			}
			else
			{
				this->current_Velocity += this->speed * this->Up ;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			if (!this->fly)
			{
				this->jump();
			}
			
			else
			{
				this->current_Velocity += this->speed * -this->Up ;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = topAccel;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = BaseAccel;
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			this->fly =  !this->fly;
			std::cout << "fly : " << this->fly << "\n";
		}
	}
	private:
	glm::vec3 current_Velocity;
	Camera* Camera;
	glm::vec3 Up = glm::vec3(0.0f,1.0f,0.0f);
	glm::vec3 direction;
	// float MaxVelosity = 2.0f;
	float speed;
	float BaseAccel = 100.0f;
	float topAccel = 5000.0f;
	bool grounded = false;
	bool fly = true;
	float PlayerRadius = 4.0f;
	float jumpHeight = 1000.0f;
	float decel = 1.0f;
};

class Plane
{
public:

	Plane(int SqrtDiv , float scale , float scaley = 40.0f ,glm::vec3 translate = glm::vec3(0.0f,0.0f,0.0f))
	{
		this->width  = SqrtDiv;
		this->height = SqrtDiv;
		this->divisions = (SqrtDiv + 1) * (SqrtDiv + 1);
		this->scale = scale;
		this->scaley = scaley;



		noise = new Noise(width,height, 1 * SqrtDiv, 4.5,0.3,30,4);
		
		
		std::vector<Texture> new_textures;
		// this->UpdateVeritces(createPlaneVertices(glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(1,0,1), glm::vec3(1,0,0), SqrtDiv,scale), generatePlaneIndices(SqrtDiv), new_textures);
		this->UpdateVeritces(SimpleVertGen( SqrtDiv, scale), generatePlaneIndices(SqrtDiv), new_textures);
		
		for (int i = 0; i < this->divisions; ++i)
		{
			this->vertices[i].Position += translate;
		}
		
		std::cout << "NUM VERTICES = " << this->vertices.size() << std::endl;

		this->displaceVertices();

		this->UpdateNormals(true);
		
		this->UpdateVeritces(this->vertices, this->indices );
	};

	void UpdateNormals(bool smooth = false , float MaxTheta = 80.0f)
	{ 
		// int testCount = this->vertices.size(); // not to be used in the for
		std::vector<Vertex> newVerts;
		std::vector<GLuint> newInds;
		// glm::vec3 green(0.0f,1.0f,0.0f);
		unsigned int crnt;
		for (unsigned int i = 0; i < this->width - 1; i++ )
		{
			for (unsigned int j = 0; j < this->height -1 ; j++ )
			{
				Vertex Cvert  = this->vertices[j + (i * this->width)];
				Vertex NvertHorisontal = this->vertices[(j+1) + (i*this->width)];
				Vertex NvertVertical   = this->vertices[j + ((i+1)*this->width + 1)];
				Vertex NvertDiagonal   = this->vertices[(j+1) + ((i+1)*this->width + 1)];

				// glm::vec3 CH = glm::normalize(NvertHorisontal.Position - Cvert.Position) ;
				// std::cout <<  "C H " << CH.x << " , " << CH.y << " , " << CH.z << "\n";
				// glm::vec3 CV = glm::normalize(NvertVertical.Position - Cvert.Position) ;
				// std::cout <<  "C V " << CV.x << " , " << CV.y << " , " << CV.z << "\n";
				// glm::vec3 CD = glm::normalize(NvertDiagonal.Position - Cvert.Position) ;
				// std::cout <<  "C D " << CD.x << " , " << CD.y << " , " << CD.z << "\n\n";
				glm::vec3 Normal1 = glm::normalize(glm::cross(NvertHorisontal.Position - Cvert.Position ,NvertVertical.Position - Cvert.Position));
				glm::vec3 Normal2 = glm::normalize(glm::cross(NvertVertical.Position - NvertDiagonal.Position,NvertHorisontal.Position - NvertDiagonal.Position)); // the horisiontal and vertical have swapped to keep the cross direction
				
				Cvert.normal = Normal1;
				newVerts.push_back(Cvert);
				newVerts.push_back(Vertex{NvertHorisontal.Position,Normal1,NvertHorisontal.Colour,NvertHorisontal.TexUV});
				newVerts.push_back(Vertex{NvertVertical.Position,Normal1,NvertVertical.Colour,NvertVertical.TexUV});
				newVerts.push_back(Vertex{NvertHorisontal.Position,Normal2,NvertHorisontal.Colour,NvertHorisontal.TexUV});
				newVerts.push_back(Vertex{NvertDiagonal.Position,Normal2,NvertDiagonal.Colour,NvertDiagonal.TexUV});
				newVerts.push_back(Vertex{NvertVertical.Position,Normal2,NvertVertical.Colour,NvertVertical.TexUV});

				crnt = (i * (this->width )) + j - 1;
				for(int k = 0 ; k < 6 ; ++k)
				{
					newInds.push_back((crnt * 6) + k);
					// std::cout << "k : " << crnt << "\n";
				}
			}
		}

		if (smooth)
		{
			glm::vec3 convNormal;
			
			std::cout << "Vertex number actual    :\t" << this->vertices.size() << "\n";
			std::cout << "Vertex number Estimated :\t" << (3 * (this->height - 1) * 3 * (this->width - 1)) << std::endl;

			for (int i = 1; i < 3 * (this->height - 1) - 1; ++i)
			{
				
			}
		}
		this->Set_Vertices(newVerts);
		this->Set_Indices(newInds);

	}

	void displaceVertices()
	{
		
		std::future<void> m_futures;
		for (int i = 0 ; i < this->divisions; ++i)
		{
			m_futures = std::async(std::launch::async, this->updateVertxPositions,&this->vertices, i,&this->noise->NoiseMap1D , this->scaley) ;
		}
	}



	void Set_Vertices(std::vector<Vertex> new_vertices)
	{
		this->vertices = new_vertices;
	};
	void Set_Indices(std::vector<GLuint> new_indices)
	{
		this->indices = new_indices;
	};

	void EditTextures(std::vector <Texture> new_textures)
	{
		this->textures = new_textures;
	};

	void UpdateVeritces(std::vector<Vertex> new_vertices , std::vector<GLuint> new_indices, std::vector<Texture> new_textures = {})
	{
		this->vertices  = new_vertices;
		this->indices 	= new_indices;
		this->textures  = new_textures;

		VAO.Bind();
		// Generates Vertex Buffer Object and links it to vertices
		VBO VBO(this->vertices);
		// Generates Element Buffer Object and links it to indices
		
		EBO EBO(this->indices);
		
		// Links VBO attributes such as coordinates and colors to VAO
		int vertexSize = sizeof(Vertex);
		VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, vertexSize , (void*)0);
		VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, vertexSize, (void*)(3 * sizeof(float)));
		VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, vertexSize, (void*)(6 * sizeof(float)));
		VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, vertexSize, (void*)(9 * sizeof(float)));
		// Unbind all to prevent accidentally modifying them
		VAO.Unbind();
		VBO.Unbind();
		
		EBO.Unbind();

		this->positions = {};		
		this->normals   = {};		
		for (int i = 0; i < this->vertices.size() ; ++i)
		{
			this->positions.push_back(this->vertices[i].Position);
			this->normals.push_back(this->vertices[i].normal);
		}
	};

	void Draw(

		ShaderClass& shader, 
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
		)

	{
		// Bind shader to be able to access uniforms
		shader.Activate();
		
		VAO.Bind();

		// Keep track of how many of each type of textures we have
		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string num;
			std::string type = textures[i].type;
			if (type == "diffuse")
			{
				num = std::to_string(numDiffuse++);
			}
			else if (type == "specular")
			{
				num = std::to_string(numSpecular++);
			}
			textures[i].texUnit(shader, (type + num).c_str(), i);
			textures[i].Bind();
		}


		// Take care of the camera Matrix
		glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(shader, "camMatrix");

		// Initialize matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		// Transform the matrices to their correct form
		trans = glm::translate(trans, translation);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);

		// Push the matrices to the vertex shader
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

		// Draw the actual mesh
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


		for (unsigned int i = 0; i < textures.size(); i++)
		{
			textures[i].Unbind();
		}
	};

	std::vector<glm::vec3> getVertexPositions()
	{
		return this->positions;
	};
	std::vector<glm::vec3> getVertexNormals()
	{
		return this->normals;
	};

	void DebugFunk(int x)
	{
		width = width + x;
		width = width - x;
	}
private:

	static void updateVertxPositions(std::vector<Vertex>* vertices,int index , std::vector<float>* value , float yscale)
	{
		// size = sizeof(&vertices);
		(*vertices)[index].Position.y += (*value)[index] * yscale;
	}


	int width;
	int height;
	int divisions;
	float scale;
	float scaley;

	VAO VAO;
	Noise* noise;
	std::vector<Vertex> vertices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<GLuint>  indices;
	std::vector<Texture> textures;
	int floatsize = sizeof(float);

};


void RefreshShader(ShaderClass* shader, std::string vertexPath ,std::string fragmentPath)
{

	ShaderClass new_shader( vertexPath.c_str() , fragmentPath.c_str() );
	*shader = new_shader;


}

void window_size_callback(GLFWwindow* window, int* width, int* height)
{
	
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Check if the key has just been pressed (not held down)
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    
    }
}

void handleSongs(std::vector<std::string>* songlist , sf::Music* music)
{
	if (music->getStatus() == music->Playing || music->getStatus() == music->Paused) return;

	int randint = GetRandomIntieger(0, songlist->size() - 1);
	std::string chosenSong = (*songlist)[randint];
	if (!music->openFromFile(chosenSong))
	{
		std::cerr << " Failed to load sound buffer: " << chosenSong << "\n";
		// sf::Music audio;
	}
	music->play();
} 

int main(int argc, char* argv[])
{


	// std::cout << "no vertices = " << noVetices;
	// std::cout << "no vertices = " << noLightVetices;

    glfwInit();  // you know what this does

    // tell mr gl man what version of opengl im using
	glfwWindowHint(GLFW_SAMPLES, 4);  // anti aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // gotta stay apple friendly
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());




    // std::cout << "W : " << WINDOW_WIDTH << "\nH : " << WINDOW_HEIGHT << "\n";
    GLFWwindow *  window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT , "Enumeral Output", NULL , NULL );  /* the window where all is displayedt o  */

    // throw actuall errors if you screw things up instead of praying and regretting life later
    if (window == NULL)
    {
        std::cout << "FAT Fail no window :(" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window); // Makes the context of the specified window current for the calling thread.
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    gladLoadGL();  // loads configs for opengl


	// glfwSetWindowSizeCallback(window, window_size_callback);
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT); // specify the span of the screen

    std::cout << "argv[0]    : " << argv[0] << std::endl;
	// argv 0  is the path to the exe in build so root will be 2 folders up
	std::string parentDir = ((std::filesystem::path(std::string(argv[0])).std::filesystem::path::parent_path()).std::filesystem::path::parent_path()).string() + "/";
    std::cout << "Parent Dir : " << parentDir << std::endl;

	
	std::string recoursePath = (parentDir + "resources/").c_str();
	std::string shaderPath   = (parentDir + "resources/shaders/").c_str();
	std::string texturePath  = (parentDir + "resources/textures").c_str();
	std::string ModelPath    = (parentDir + "resources/assets/models/").c_str();
     
	std::vector<ShaderClass*> ShaderList; 

    ShaderClass shaderProgram1((shaderPath + "default.vert").c_str(),(shaderPath + "default.frag").c_str());
	ShaderList.push_back(&shaderProgram1);
    
	ShaderClass shaderProgram2((shaderPath + "default_2.vert").c_str(),(shaderPath + "default_2.frag").c_str());
	ShaderList.push_back(&shaderProgram2);
    
	ShaderClass TerrainShaderProgram1((shaderPath + "Terrain.vert").c_str(),(shaderPath + "Terrain.frag").c_str());
	ShaderList.push_back(&TerrainShaderProgram1);
    
	// ShaderClass TerrainShaderProgram2((shaderPath + "Terrain.vert_2").c_str(),(shaderPath + "Terrain_2.frag").c_str());
	// ShaderList.push_back(&TerrainShaderProgram2);
    
	ShaderClass grassProgram((shaderPath + "default.vert").c_str(),(shaderPath + "grass.frag").c_str());
	ShaderList.push_back(&grassProgram);
    
	ShaderClass HudShader((shaderPath + "HudShader.vert").c_str(),(shaderPath + "HudShader.frag").c_str());
	ShaderList.push_back(&HudShader);

	ShaderClass TerrainShader = TerrainShaderProgram1;

	ShaderClass shaderProgram = shaderProgram1;


	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(0.8f, 0.9f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	light sun;
	sun.Lightcolour = lightColor;
	sun.Lightpos    = lightPos;
	sun.LightModel  = lightModel;

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	TerrainShader.Activate();
	glUniform4f(glGetUniformLocation(TerrainShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(TerrainShader.ID, "lightPos"), lightPos.x + 50, lightPos.y + 100, lightPos.z + 50);
	

	float translate = 0.87f;
	glUniform1f(glGetUniformLocation(TerrainShader.ID, "translate"), translate);

	grassProgram.Activate();
	glUniform4f(glGetUniformLocation(grassProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);



    // GLuint uniID = glGetUniformLocation(shaderProgram.Get_id(), "scale");
   

    


	// float rotation = 0.0f;
	// double prevTime = glfwGetTime();




	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_BACK);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);
	// Configures the blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Creates camera object
	Camera camera(WINDOW_WIDTH, WINDOW_WIDTH, glm::vec3(0.0f, 0.0f, 2.0f ), 45.0f, 0.1f, 10000.0f);



	std::string modelLoad = (ModelPath + "imported/world_earth/scene.gltf").c_str();

	
	// std::cout << "ParentDir = " << parentDir << std::endl;
	std::string groundPath = "resources/assets/models/defaults/ground/scene.gltf";
	std::string treesPath = "resources/assets/models/defaults/grass/scene.gltf";

	Model Earth((parentDir + groundPath).c_str());
	Model Grass((parentDir + treesPath).c_str());


	// int divs;
	// if (argc > 1) {
	// 	divs = 	std::stoi(argv[1]);
	// }
	// else {
	// 	divs = 500; // Number of divisions for each axis
	// }
	// std::string divstr = get_file_contents((recoursePath + "/dat.txt").c_str());


	// set control variables to their default values
	int divs    = 200;
	float scale = 300.0f;
	float scaley = 40.0f;
	glm::vec3 planeTranslation = glm::vec3(0.0f);

	std::ifstream read( (recoursePath + "/dat.txt").c_str() );
	bool found = false;
	for (std::string line; std::getline(read, line); ) {
		// removing punctuation like paranthesis, commas, etc.
		std::string checkstring = "";
		std::string checkstringPost = "";
		found = false; 
		for (char j : line)
		{
			if (j == ':') found = true;

			if (!found)
			{
				checkstring += j;
			}
			else
			{
				checkstringPost += j;
			}
		}
		// remove any spaces at the end
		while(checkstring[ checkstring.length() - 1 ] == ' ')
		{
			checkstring.pop_back();
		}
		std::replace_if(std::begin(checkstring), std::end(checkstring), [] (char x) { return std::ispunct(x); }, ' ');
		// std::remove_if(std::begin(checkstringPost), std::end(checkstringPost), [] (char x) { return std::ispunct(x); });
		// std::replace_if(std::begin(checkstringPost), std::end(checkstringPost), [] (char x) { return std::ispunct(x); }, ' ');
		checkstringPost.erase(remove(checkstringPost.begin(), checkstringPost.end(), ' '), checkstringPost.end());
		checkstringPost.erase(remove(checkstringPost.begin(), checkstringPost.end(), ':'), checkstringPost.end());

		std::cout<< "\n\n READING DATA :" << "chechkstring = [" << checkstring << "]\tpost = [" << checkstringPost << "]" << " found :" << found <<  "\n\n";

		if (checkstring == "Divisions")
		{
			divs = std::stoi(checkstringPost);
		}
		else if (checkstring == "ScaleXZ")
		{
			scale = std::stof(checkstringPost);
		}
		else if (checkstring == "ScaleY")
		{
			scaley = std::stof(checkstringPost);
		}
		else if (checkstring == "Plane Translation")
		{

			// assign the correct values to the plane translate.
			int index = 0;
			std::string StringValue;
			std::vector<float> Value;
			char j = ' ';
			StringValue = "";

			while(index < checkstring.length() ) // keep going until we hit a comma or the end of the string
			{
				j = checkstringPost[index];
				if (j == ',')
				{
		
					Value.push_back(std::stof(StringValue));
					
					if(Value.size() >= 3){break;} // may turn this into a variable that i increment inside the if block
					
					StringValue = "";
					index ++;
					continue;
				}

				StringValue += j;
				index ++;
				
				// very paper mache way of doing things, but it works for now
				if (index  == checkstring.length())
				{
			
					Value.push_back(std::stof(StringValue));
				}
			}
			
			if (Value.size() < 3)
			{
				std::cerr << "invalid Plane translate , initialising to [0,0,0]" << std::endl;
				// set value arr to be {0.0f,0.0f,0.0f}
				Value = {0.0f ,0.0f ,0.0f};

			}

			
			for (float i  : Value)
			{
				std::cout << checkstring;

			}
			planeTranslation = glm::vec3(Value[0],Value[1],Value[2]);
		}
	}


	Plane Ground(divs,scale, scaley , planeTranslation);


	TerrainShader.Activate();
	glUniform1f(glGetUniformLocation(TerrainShader.ID, "heightscale"), scaley);

	Hud HeadSup;

	// float size = 4.0f / static_cast<float>(divs);
	
	// Earth.setScale(size);
	// Grass.setScale(size * .75f);

	// Shader shader("src/shaders/vertex.shader", "	src/shaders/fragment.shader");
	// Shader skyboxShader("src/shaders/skyboxVertex.shader", "src/shaders/skyboxFragment.shader");
	// Mesh* skyboxMesh = new Mesh("resources/assets/meshes/skybox.obj");
	// Texture skyboxTex("resources/assets/textures/skybox.png", true	);
	// Skybox skybox(&skyboxShader, &skyboxMesh, &skyboxTex);

	// // Game loop timer
	// Timer t;
	// while (!glfwWindowShouldClose(	

	// sf::Sound audio;
	sf::Music audio;
	sf::SoundBuffer audioBuffer;


	std::vector<std::string> songList = GetFolderContents((parentDir + "resources/audio/music").c_str());
	for (int i = 0 ; i < songList.size() ; ++i)
	{
		if (songList[i] == std::string(parentDir + "resources/audio/music/.DS_Store"))
		{
			songList.erase(songList.begin() + i);
		}
	}
	// for (int i = 0 ; i < songList.size() ; ++i)
	// {
	// 	if (songList[i].substr(songList[i].length() -  4) == ".ogg")
	// 	{

	// 	}
	// }


	// for  (int i=0 ; i < songList.size() ; ++i )
	// {
	// 	std::cout << songList[i] <<"\n";
 	// }

	int randint = GetRandomIntieger(0, songList.size() - 1);

	std::string chosenSong = songList[randint];
	// std::string chosenSong = "/Users/nathanielfrimpong-santeng/codestuffs/EnumeralTest/TerrainGen/resources/audio/music/Enumeral_Neo_2.mp3";
	std::cout << "SONGPLAYING = " << chosenSong << std::endl;

	float currentVolume = 100.0f;
	bool isPlaying = false;

	// if (!audioBuffer.loadFromFile(chosenSong))
	if (!audio.openFromFile(chosenSong))
	{
		std::cerr << " Failed to load sound buffer: " << chosenSong << "\n";
		// sf::Music audio;
	}
	audio.setLoop(false);
	audio.setPosition(0.0f,0.0f,0.0f);
	audio.setVolume(currentVolume);
	audio.play();
	isPlaying = true;

	// else
	// {
	// 	sf::Sound audio;
	// 	audio.setPitch(1.0f);
	// 	audio.setVolume(currentVolume);
	// 	audio.setBuffer(audioBuffer);
	// 	audio.setLoop(true);
	// 	audio.play();
	// 	isPlaying = true;
	// 	SONGAUD = 2;

	// }
	




	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	// Use this to disable VSync (not advized)
	//glfwSwapInterval(0);

	float dt;

	bool wired;
	bool grass;
	bool earth;
	bool Trippy;
	wired = false; // Change to true if you want wireframe mode
	grass = true; // Change to true if you want wireframe mode
	earth = true; // Change to true if you want wireframe mode
	Trippy = false; // Change to true if you want wireframe mode
    /* MAIN LOOP */
	PlayerPhys physics(&camera);

	glEnable(GL_DEPTH_TEST); // Enabling depth testing allows rear faces of 3D objects to be hidden behind front faces.
	glEnable(GL_MULTISAMPLE); // Anti-aliasing
	// glEnable(GL_BLEND); // GL_BLEND for OpenGL transparency which is further set within the fragment shader. 
    while (!glfwWindowShouldClose(window))
    {

		
		// glfwGetWindowFrameSize(window, &WINDOW_WIDTH, &WINDOW_WIDTH);
		// glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

		// Camera camera(WINDOW_WIDTH,WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));



		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		dt = (timeDiff) / 1000;
		counter++;

		// Creates new title
		std::string FPS = std::to_string((1.0 / timeDiff));
		std::string ms = std::to_string(timeDiff * 1000);
		std::string newTitle = "Enumeral Terrain Gen - " + FPS + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		
		if (timeDiff >= 1.0 / 30.0)
		{
			// Resets times and counter
			counter = 0;
			// Use this if you have disabled VSync
			//camera.Inputs(window);
		}
		camera.Inputs(window, dt);
		prevTime = crntTime;

        glClearColor(0.09f, 0.051f, 0.098f, 1.0f);  // BLUE!!!
        // glClear(GL_COLOR_BUFFER_BIT);3
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		handleSongs(&songList, &audio);




		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)	{ wired = !wired; }
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)	{ grass = !grass; }
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)	{ earth = !earth; }
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)	{ Trippy = !Trippy; }
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			for (ShaderClass* shader : ShaderList)
			{
				RefreshShader(shader, shader->getVertexSource(),shader->getFragmentSource());
				shader->Activate();

				glUniform4f(glGetUniformLocation(shader->ID, "lightColor"), sun.Lightcolour.x, sun.Lightcolour.y, sun.Lightcolour.z, sun.Lightcolour.w);
				glUniform3f(glGetUniformLocation(shader->ID, "lightPos"), sun.Lightpos.x, sun.Lightpos.y, sun.Lightpos.z);
			}

			// RefreshShader(&grassProgram, grassProgram.getVertexSource(), grassProgram.getFragmentSource(), &sun );
			// RefreshShader(&TerrainShader, TerrainShader.getVertexSource(), TerrainShader.getFragmentSource(), &sun );
			// RefreshShader(&HudShader, HudShader.getVertexSource(), HudShader.getFragmentSource(), &sun );
			// RefreshShader(&shaderProgram, shaderProgram.getVertexSource(), shaderProgram.getFragmentSource(), &sun );
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			currentVolume += 100.0f * timeDiff;
			currentVolume = clamp(currentVolume,0.0f,100.0f);
			audio.setVolume(currentVolume);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			currentVolume -= 100.0f * timeDiff;
			currentVolume = clamp(currentVolume,0.0f,100.0f);
			audio.setVolume(currentVolume);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			currentVolume -= 100.0f * timeDiff;
			currentVolume = clamp(currentVolume,0.0f,100.0f);
			audio.setVolume(currentVolume);
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			std::cout << "is it playing : " << audio.getStatus() << std::endl;
		}


		if(wired)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			shaderProgram = shaderProgram2;
			// Ground.DebugFunk(6);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			shaderProgram = shaderProgram1;
		}

		// if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){specIntensity += 0.5f * !OPress;


		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix();





		physics.floorColision(Earth.getVertexPositions(),Earth.getVertexNormals());
		// physics.floorColision(Ground.getVertexPositions(),Ground.getVertexNormals());

		physics.handleInputs(window,dt);
		physics.UpdatePositions(dt);
        // initialising th ematricies to the identity matrix

		if (earth)
		{
			Earth.Draw(shaderProgram, camera);
		}







		Ground.Draw(TerrainShader, camera);

		glDisable(GL_CULL_FACE);

		
		for (int i = 0; i < Trippy; i++)
		{
			HeadSup.Draw(HudShader, camera);
			glUniform1f(glGetUniformLocation(HudShader.ID, "Time"), crntTime);
		}


		for (int i = 0; i < grass; i++)
		{
			Grass.Draw(grassProgram, camera);
		}

		glEnable(GL_CULL_FACE);

        
		glfwSwapBuffers(window);
        glfwPollEvents();
		
    }

    shaderProgram.Delete();
	grassProgram.Delete();
	TerrainShader.Delete();
	shaderProgram1.Delete();
	shaderProgram2.Delete();
	TerrainShaderProgram1.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}