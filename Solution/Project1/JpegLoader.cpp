// STB_IMAGE_IMPLEMENTATION must be defined in exactly ONE .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "JpegLoader.h"
#include <cstring>

Image LoadImageJpeg(const char* path)
{
    Image img = {};
    int w = 0, h = 0, comp = 0;
    unsigned char* data = stbi_load(path, &w, &h, &comp, 4);
    if (!data) return img;

    img.data    = data;
    img.width   = w;
    img.height  = h;
    img.mipmaps = 1;
    img.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    return img;
}

void UnloadImageJpeg(Image& img)
{
    if (img.data) stbi_image_free(img.data);
    memset(&img, 0, sizeof(img));
}
