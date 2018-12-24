#include "image_c.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

image make_image(const int w, const int h, const int chan) // 1 byte per channel
{
  image img;
  img.w = w;
  img.h = h;
  img.chan = chan;
  img.data = (unsigned char*)malloc(w * h * chan);
  return img;
};

void free_image(image img)
{
//  stbi_image_free(img.data);
  free(img.data);
};

image load_image(const char* filename)
{
  image img;
  img.data = stbi_load(filename, &img.w, &img.h, &img.chan, 0);
  return img;
};

void save_image(image img, const char* filename)
{
  int comp = img.chan;
  int stride_in_bytes = 0;

  stbi_write_png(filename, img.w, img.h, comp, img.data, stride_in_bytes);
};

float get_pixel(image img, int x, int y, int chan)
{
  return img.data[y*img.w*img.chan + x*img.chan + chan]; // STB is interleaved channels
};

void set_pixel(image img, int x, int y, int chan, float val)
{
  img.data[y*img.w*img.chan + x*img.chan + chan] = (unsigned int)val; // STB is interleaved channels
};

image rgb_to_grayscale(image img)
{
  image imgG = make_image(img.w, img.h, 1);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float r = get_pixel(img, x, y, 0);
      float g = get_pixel(img, x, y, 1);
      float b = get_pixel(img, x, y, 2);
      float Y = 0.299 *r + 0.587 *g + .114 *b;
      set_pixel(imgG, x, y, 0, Y);
    }
  }
  return imgG;
};
