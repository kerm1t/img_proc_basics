#include "image_c.h"

#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
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

image copy_image(image img)
{
  image imgOut = make_image(img.w, img.h, img.chan);
  memcpy(imgOut.data, img.data, img.w* img.h* img.chan);
  return imgOut;
};

float get_pixel(image img, int x, int y, int chan)
{
  // padding strategy: clamp
  if (x < 0) x = 0;
  if (x >= img.w) x = img.w-1;
  if (y < 0) y = 0;
  if (y >= img.h) y = img.h - 1;
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
      float Y = 0.299f *r + 0.587f *g + .114f *b;
      set_pixel(imgG, x, y, 0, Y);
    }
  }
  return imgG;
};

void shift_image(image img, int chan, float val)
{
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, chan, get_pixel(img,x,y,chan)*val);
    }
  }
};

void clamp_image(image img)
{
/*  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      for (int chan = 0; chan < 3; chan++)
      {
        if (get_pixel(img, x, y, chan) > 255)
        {
          set_pixel(img, x, y, chan, 255.0f);
        }
      }
    }
  }
*/
};