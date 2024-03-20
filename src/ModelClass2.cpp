#include <ModelClass2.hpp>




ModelLoader::ModelLoader(const char* filepath) : m_filePath(std::string(filepath))
{
    m_scene = m_importer.ReadFile(filepath,ASSIMP_LOAD_FLAGS);
    // load_model();
    load_model_cout_console();
}

void ModelLoader::load_model_cout_console()
	{
		// Briefly looking at the node structure
		// ------------------------------------------------
		if (!m_scene || !m_scene->mRootNode || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << m_importer.GetErrorString() << "\n";
		else
		{
			m_numMeshes = m_scene->mNumMeshes;
			m_MeshList.resize(m_numMeshes);
 
			std::cout << "\n\n   Start of Assimp Loading Meshes & Analysis";
			std::cout << "\n   -----------------------------------------";
 
			m_rootNode = m_scene->mRootNode;
 
			std::cout << "\n   node->mNumMeshes: " << m_rootNode->mNumMeshes;
			std::cout << "\n   node->mName.C_Str(): " << m_rootNode->mName.C_Str();
			std::cout << "\n\n   node->mNumChildren: " << m_rootNode->mNumChildren;
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < m_rootNode->mNumChildren; ++i)
			{
				std::cout << "\n   node->mChildren[i]->mName.C_Str(): " << m_rootNode->mChildren[i]->mName.C_Str();
				std::cout << "\n   node->mChildren[i]->mNumMeshes: " << m_rootNode->mChildren[i]->mNumMeshes;
			}
			std::cout << "\n\n   scene->HasMaterials(): " << m_scene->HasMaterials();
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < m_scene->mNumMaterials; ++i)
				std::cout << "\n   scene->mMaterials[i]->GetName(): " << m_scene->mMaterials[i]->GetName().C_Str();
 
			std::cout << "\n\n   scene->HasTextures(): " << m_scene->HasTextures();
 
			aiMesh* mesh{};
 
			int total_num_indices = 0;
			int indices_offset = 0; // Not being used yet... i.e. indices_offset += mesh->mNumVertices; is commented further down.
 
			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			std::cout << "\n   scene->mNumMeshes: " << m_numMeshes;
			std::cout << "\n   ********************\n";
			// ---------------------------------------------------------
			for (unsigned int i = 0; i < m_numMeshes; ++i) // In this case... scene->mNumMeshes = node->mChildren[i]->mNumMeshes
			{
				mesh = m_scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html
 
				std::cout << "\n\n   mesh->mMaterialIndex: " << mesh->mMaterialIndex;
				std::cout << "\n   ----------------------- ";
				std::cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();
				
				aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html
 
				std::cout << "\n\n   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << material->GetTextureCount(aiTextureType_DIFFUSE);
				std::cout << "\n   material->GetTexture(aiTextureType_SPECULAR, tex_count, &string): " << material->GetTextureCount(aiTextureType_SPECULAR);
				std::cout << "\n   material->GetTexture(aiTextureType_AMBIENT, tex_count, &string): " << material->GetTextureCount(aiTextureType_AMBIENT) << "\n\n";
 
				unsigned int tex_count = 0;
				for (; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // The above std::cout reveals that only using: aiTextureType_DIFFUSE
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string); // Acquire the name of the image file to be loaded.
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << string.C_Str() << "\n\n";
 
					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.
					std::cout << "   Loading Image\n";
 
					if (already_loaded == -1) // Image not yet loaded.
					{		
						bool load_complete = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_complete);
 
						if (load_complete) // Although do nothing if the image fails to load.
						{
							Texture_load texture;
							texture.imageName = string.C_Str();
							texture.Tex_ID = texture_handle;
 
							m_TextureList.push_back(texture);
							m_MeshList[i].tex_handle = texture_handle;
						}
					}
					else // Assign existing texture handle.
					{
						std::string edited = string.C_Str();
						std::size_t position = edited.find_last_of("\\");
 
						std::cout << "   Image file: " << edited.substr(position + 1) << " (is already loaded)";
						m_MeshList[i].tex_handle = already_loaded;
					}
				}
				if (tex_count == 0)
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): No image has been applied to this mesh\n\n";
				else
					std::cout << "\n";
 
				for (unsigned int slot = 0; slot < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++slot)
					std::cout << "   mesh->HasTextureCoords(" << slot << "): " << mesh->HasTextureCoords(slot) << "\n";
 
				std::cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
				std::cout << "\n   ------------------------------------- ";
 
				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					m_MeshList[i].positions.push_back(position);
 
					std::cout << "\n   Count: " << i2;
					std::cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
					std::cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
					std::cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;
 
					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						m_MeshList[i].normals.push_back(normal);
						std::cout << "\n   mesh->mNormals[" << i2 << "] X: " <<normal.x << " Y: " << normal.y << " Z: " << normal.z;
					}
					else
						m_MeshList[i].normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
 
					if (mesh->HasTextureCoords(0)) // Above for loop: AI_MAX_NUMBER_OF_TEXTURECOORDS reveals that only slot [0] is in question.
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						m_MeshList[i].texCoords.push_back(tex_coords);
						std::cout << "\n   mesh->mTextureCoords[0][" << i2 << "] X: " << tex_coords.x << " Y: " << tex_coords.y;
					}
					else					
						m_MeshList[i].texCoords.push_back(glm::vec2(0.0f, 0.0f));
				}
				std::cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
				std::cout << "   ------------------ ";
				
				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					std::cout << "\n";
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
					{
						std::cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
						m_MeshList[i].indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);
						++total_num_indices;
					}
				}
				std::cout << "\n   Total number of indices: " << total_num_indices;
				std::cout << "\n   **************************";
				total_num_indices = 0;
 
				// indices_offset += mesh->mNumVertices; // Disabled for tutorial: Model Loading (Part 1 of 3)
 
				std::cout << "\n   Indices offset (Total 'mesh->mNumVertices' so far): " << indices_offset;
				std::cout << "\n   *****************************************************\n\n";
 
				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}
			// Look to see if each mesh's texture handle corresponds correctly to the loaded image
			// ----------------------------------------------------------------------------------------------------------------
			if (m_TextureList.size() > 0)
				for (unsigned int i = 0; i < m_TextureList.size(); ++i)
				{
					std::cout << "   image_list[" << i << "].imageID: " << m_TextureList[i].Tex_ID << "... image_list[" << i << "].image_name: " << m_TextureList[i].imageName << "\n";
 
					for (unsigned int i2 = 0; i2 < m_numMeshes; ++i2)
						if (m_TextureList[i].Tex_ID == m_MeshList[i2].tex_handle)
							std::cout << "   m_MeshList[" << i2 << "].tex_handle: " << m_MeshList[i2].tex_handle << "\n";
					std::cout << "\n";
				}
			else
				std::cout << "   ***** No images have been loaded\n";			
		}
	}

void ModelLoader::load_model()
	{
		if (!m_scene || !m_scene->mRootNode || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << m_importer.GetErrorString() << "\n";
		else
		{
			m_numMeshes = m_scene->mNumMeshes;
			m_MeshList.resize(m_numMeshes);
 
			aiMesh* mesh{};		
			int indices_offset = 0; // Not being used yet... i.e. indices_offset += mesh->mNumVertices; is commented further down.
 
			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			for (unsigned int i = 0; i < m_numMeshes; ++i)
			{
				mesh = m_scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html				
 
				aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html			
 
				// This loop will only run once (i.e. there's only 1 texture per mesh)
				for (unsigned int tex_count = 0; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // Also, only using: aiTextureType_DIFFUSE.
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string); // Acquire the name of the image file to be loaded.			
 
					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.
 
					if (already_loaded == -1) // Image not yet loaded so now attempt to load it.
					{
						bool load_success = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_success);
 
						if (load_success) // Although do nothing if the image fails to load.
						{
							Texture_load texture;
							texture.imageName = string.C_Str();
							texture.Tex_ID = texture_handle;
 
							m_TextureList.push_back(texture);
							m_MeshList[i].tex_handle = texture_handle;
						}
					}
					else	
						m_MeshList[i].tex_handle = already_loaded; // Assign existing texture handle.
				}
				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
						glm::vec3 position{};
						position.x = mesh->mVertices[i2].x;
						position.y = mesh->mVertices[i2].y;
						position.z = mesh->mVertices[i2].z;
						m_MeshList[i].positions.push_back(position);					
 
						if (mesh->HasNormals())
						{
							glm::vec3 normal{};
							normal.x = mesh->mNormals[i2].x;
							normal.y = mesh->mNormals[i2].y;
							normal.z = mesh->mNormals[i2].z;
							m_MeshList[i].normals.push_back(normal);
						}
						else
							m_MeshList[i].normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
 
						if (mesh->HasTextureCoords(0)) // Only slot [0] is in question.
						{
							glm::vec2 tex_coords{};
							tex_coords.x = mesh->mTextureCoords[0][i2].x;
							tex_coords.y = mesh->mTextureCoords[0][i2].y;							
							m_MeshList[i].texCoords.push_back(tex_coords);							
						}
						else
							m_MeshList[i].texCoords.push_back(glm::vec2(0.0f, 0.0f));
				}
				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)										
						m_MeshList[i].indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);									
				
				// indices_offset += mesh->mNumVertices; // Disabled for tutorial: Model Loading (Part 1 of 3)
 
				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}			
		}
	}	




int ModelLoader::is_image_loaded(std::string filename)
{
    for (unsigned int i = 0; i < m_TextureList.size() ;++i)
    {
        if  (filename.compare(m_TextureList[i].imageName) == 0)
        {
            return m_TextureList[i].Tex_ID;
        }
    }
    return -1;
}

void ModelLoader::set_buffer_data(unsigned int index)
{
	glGenVertexArrays(1, &m_MeshList[index].VAO);
	glGenBuffers(1, &m_MeshList[index].VBO1); // Alternative to using 3 separate VBOs, instead use only 1 VBO and set glVertexAttribPointer's offset...
	glGenBuffers(1, &m_MeshList[index].VBO2); // like was done in tutorial 3... Orbiting spinning cubes.
	glGenBuffers(1, &m_MeshList[index].VBO3);
	glGenBuffers(1, &m_MeshList[index].EBO);

	glBindVertexArray(m_MeshList[index].VAO);		

	// Vertex Positions
	// ---------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_MeshList[index].VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_MeshList[index].positions.size(), &m_MeshList[index].positions[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	// Vertex Normals
	// --------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_MeshList[index].VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_MeshList[index].normals.size(), &m_MeshList[index].normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// Texture Coordinates
	// ---------------------------
	glBindBuffer(GL_ARRAY_BUFFER, m_MeshList[index].VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_MeshList[index].texCoords.size(), &m_MeshList[index].texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	
	// Indices for: glDrawElements()
	// ---------------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_MeshList[index].EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_MeshList[index].indices.size(), &m_MeshList[index].indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0); 	// Unbind VAO
}

unsigned int ModelLoader::load_texture_image(std::string file_name, bool& load_complete)
{
	// stbi_set_flip_vertically_on_load(1); // Call this function if the image is upside-down.		

	std::size_t position = file_name.find_last_of("\\");
	file_name = "Images\\" + file_name.substr(position + 1);		

	int width, height, num_components;
	unsigned char* image_data = stbi_load(file_name.c_str(), &width, &height, &num_components, 0);

	unsigned int textureID;
	glGenTextures(1, &textureID);		

	if (image_data)
	{
		GLenum format{};

		if (num_components == 1)
			format = GL_RED;
		else if (num_components == 3)
			format = GL_RGB;
		else if (num_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Recommended by NVIDIA Rep: https://devtalk.nvidia.com/default/topic/875205/opengl/how-does-gl_unpack_alignment-work-/

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
		// ----------------------------------------------------------------------------------------------------------------
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // GL_REPEAT... GL_MIRRORED_REPEAT... GL_CLAMP_TO_EDGE... GL_CLAMP_TO_BORDER.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

			// float border_colour[] = {0.45, 0.55, 0.95};
			// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour); // For above when using: GL_CLAMP_TO_BORDER		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST... GL_LINEAR... GL_NEAREST_MIPMAP_NEAREST (See above link for full list)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.

		load_complete = true;
		stbi_image_free(image_data);			
		std::cout << "   Image loaded OK: " << file_name << "\n";
	}
	else
	{
		load_complete = false;
		stbi_image_free(image_data);
		std::cout << "   Image failed to load: " << file_name << "\n";
	}
	return textureID;
}