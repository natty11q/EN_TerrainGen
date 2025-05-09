#ifndef TEXTURE_CLASS_HPP
#define TEXTURE_CLASS_HPP

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "ShaderClass.hpp"

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;

	Texture(const char* image, const char* texType, GLuint slot);

	// Assigns a texture unit to a texture
	void texUnit(ShaderClass& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif
