
#ifndef RENDERING_IMAGE
#define RENDERING_IMAGE

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

namespace render::image
{

unsigned int loadImage(const char* a_filename)
{
	int width;
	int height;
	int comp;
	unsigned char* image = stbi_load(a_filename, &width, &height, &comp, STBI_rgb);
	if(image == nullptr)
	{
		std::cout << "Failed to load image '" << a_filename << "'" << std::endl;
		return 0;
	}

	std::printf("Texture width=%d, height=%d\n", width, height);

	unsigned int textureId = 0;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return textureId;
}

unsigned int loadImageFromBuffer(const unsigned char* a_buffer, int a_length)
{
	int width;
	int height;
	int comp;

	unsigned char* image = stbi_load_from_memory(a_buffer, a_length, &width, &height, &comp, STBI_rgb);
	if(image == nullptr)
	{
		std::cout << "Failed to load image from buffer" << std::endl;
		return 0;
	}

	std::printf("Texture width=%d, height=%d\n", width, height);

	unsigned int textureId = 0;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return textureId;
}


} // render::image

#endif  // RENDERING_IMAGE
