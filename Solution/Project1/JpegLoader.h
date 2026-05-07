#pragma once
// Loads a JPEG directly into a raylib Image using stb_image.
// STB_IMAGE_IMPLEMENTATION is defined only in JpegLoader.cpp — include this .h anywhere.

#include "raylib.h"

Image LoadImageJpeg(const char* path);
void  UnloadImageJpeg(Image& img);
