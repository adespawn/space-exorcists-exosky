#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);

#endif // TEXTURE_LOADER_H