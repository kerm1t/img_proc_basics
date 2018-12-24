#include "image_c.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image_write.h"

image make_image(const int w, const int h, const int chan) // 1 byte per channel
{
  image img;
  img.data = (unsigned char*)malloc(w * h * chan);
  return img;
};

void free_image(image img)
{
  stbi_image_free(img.data);
  free(img.data);
};

image load_image(const char* filename)
{
  image img;
  img.data = (filename, &img.w, &img.h, &img.chan, 0);
  return img;
};

void save_image(image img, const char* filename)
{
  int comp = img.chan;
  int stride_in_bytes = 0;

  stbi_write_png(filename, img.w, img.h, comp, img.data, stride_in_bytes);
};
