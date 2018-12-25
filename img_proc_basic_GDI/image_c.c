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
  img.data = malloc(w * h * chan * sizeof(float));
  return img;
};

void free_image(image img)
{
  free(img.data);
};

image load_image(const char* filename) // convert from HWC (channels interleaved) to CHW
{
  int w;
  int h;
  int chan;
  unsigned char* data;
  data = stbi_load(filename, &w, &h, &chan, 0);
  assert(data != NULL);

  image img = make_image(w, h, chan);

  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      unsigned char r = data[y*img.w*img.chan + x*img.chan + 0]; // STB is interleaved channels
      unsigned char g = data[y*img.w*img.chan + x*img.chan + 1];
      unsigned char b = data[y*img.w*img.chan + x*img.chan + 2];
      set_pixel(img, x, y, 0, (float)r / 255.0f);
      set_pixel(img, x, y, 1, (float)g / 255.0f);
      set_pixel(img, x, y, 2, (float)b / 255.0f);
    }
  }
  free(data);

  return img;
};

void save_image(image img, const char* filename) // convert to HWC (channels interleaved)
{
// 2do: chan == 1 (gray)
  unsigned char* data = malloc(img.w*img.h*img.chan);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      unsigned char r = (unsigned char)(get_pixel(img, x, y, 0) * 255.0f);
      unsigned char g = (unsigned char)(get_pixel(img, x, y, 1) * 255.0f);
      unsigned char b = (unsigned char)(get_pixel(img, x, y, 2) * 255.0f);
      data[y*img.w*img.chan + x*img.chan + 0] = r; // STB is interleaved channels
      data[y*img.w*img.chan + x*img.chan + 1] = g;
      data[y*img.w*img.chan + x*img.chan + 2] = b;
    }
  }

  int comp = img.chan;
  int stride_in_bytes = 0;
  stbi_write_png(filename, img.w, img.h, comp, data, stride_in_bytes);

  free(data);
};

image copy_image(image img)
{
  image imgOut = make_image(img.w, img.h, img.chan);
  memcpy(imgOut.data, img.data, img.w* img.h* img.chan * sizeof(float));
  return imgOut;
};

float get_pixel(image img, int x, int y, int chan) // CHW : channel -> height -> width
{
  // padding strategy: clamp
  if (x < 0) x = 0;
  if (x >= img.w) x = img.w;
  if (y < 0) y = 0;
  if (y >= img.h) y = img.h;
  return img.data[chan*img.w*img.h + y*img.w + x];
};

void set_pixel(image img, int x, int y, int chan, float val) // CHW
{
  img.data[chan*img.w*img.h + y*img.w + x] = val;
};

image rgb_to_grayscale(image img)
{
  assert(img.chan == 3);
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

image rgb_to_hsv(image img)
{
  assert(img.chan == 3);
  image imgHSV = make_image(img.w, img.h, img.chan);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float r = get_pixel(img, x, y, 0);
      float g = get_pixel(img, x, y, 1);
      float b = get_pixel(img, x, y, 2);
      float V = max(max(r, g), b);
      float m = min(min(r, g), b);
      float C = V - m;
      float S = C / V;
      float h;
      if (C == 0) h = 0; // MAX == MIN
      if (V == r) h = (g - b) / C + 0.0f;
      if (V == g) h = (b - r) / C + 2.0f;
      if (V == b) h = (r - g) / C + 4.0f;
      float H = h / 6.0f;      // H = h * 60 deg
      if (h < 0) H = H + 1.0f; // H = H + 360 deg
      set_pixel(imgHSV, x, y, 0, H);
      set_pixel(imgHSV, x, y, 1, S);
      set_pixel(imgHSV, x, y, 2, V);
    }
  }
  return imgHSV;
};

image hsv_to_rgb(image img)
{
  assert(img.chan == 3);
  image imgRGB = make_image(img.w, img.h, img.chan);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float H = get_pixel(img, x, y, 0);
      float S = get_pixel(img, x, y, 1);
      float V = get_pixel(img, x, y, 2);
      
// https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
      float C = V * S;
      float h = H * 6.0f;
      float X = C * (1.0f - fabs(fmod(h,2) - 1.0f));
      float r, g, b;
      r = g = b = 0.0f; // if h / H is undefined
      // find bottom points for r,g,b   ...   position on hexagon
      if ((0 <= h) && (h <= 1))
      {
        r = C;
        g = X;
        b = 0;
      }
      else if ((1 < h) && (h <= 2))
      {
        r = X;
        g = C;
        b = 0;
      }
      else if ((2 < h) && (h <= 3))
      {
        r = 0;
        g = C;
        b = X;
      }
      else if ((3 < h) && (h <= 4))
      {
        r = 0;
        g = X;
        b = C;
      }
      else if ((4 < h) && (h <= 5))
      {
        r = X;
        g = 0;
        b = C;
      }
      else if ((5 < h) && (h <= 6))
      {
        r = C;
        g = 0;
        b = X;
      }
      float m = V - C;
      set_pixel(imgRGB, x, y, 0, r+m);
      set_pixel(imgRGB, x, y, 1, g+m);
      set_pixel(imgRGB, x, y, 2, b+m);
    }
  }
  return imgRGB;

};

void scale_image(image img, int chan, float val)
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
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      for (int chan = 0; chan < 3; chan++)
      {
        if (get_pixel(img, x, y, chan) > 1.0f)
        {
          set_pixel(img, x, y, chan, 1.0f);
        }
      }
    }
  }
};