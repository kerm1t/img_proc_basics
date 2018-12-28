#include "image_c.h"
#include <math.h> // exp - where is M_PI ???

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

image make_image(const int w, const int h, const int chan) // 1 byte per channel. gray = 1 chan, RGB = 3 chan
{
  image img;
  img.w = w;
  img.h = h;
  img.chan = chan;
  img.data = malloc(w * h * chan * sizeof(float));  // malloc = no initialization - otherwise use calloc, ...
                                                    // ... almost, s. https://vorpus.org/blog/why-does-calloc-exist/
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
  assert(data != NULL); // e.g. file not found

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

image copy_image(const image img)
{
  image imgN = make_image(img.w, img.h, img.chan);
  memcpy(imgN.data, img.data, img.w* img.h* img.chan * sizeof(float));
  return imgN;
};

float get_pixel(image img, int x, int y, int chan) // CHW : channel -> height -> width
{
  // padding strategy: clamp
  if (x < 0) x = 0;
  if (x >= img.w) x = img.w-1;
  if (y < 0) y = 0;
  if (y >= img.h) y = img.h-1;
  return img.data[chan*img.w*img.h + y*img.w + x];
};

void set_pixel(image img, int x, int y, int chan, float val) // CHW
{
  img.data[chan*img.w*img.h + y*img.w + x] = val;
};

image rgb_to_grayscale(image img)
{
  assert(img.chan >= 3); // e.g. a PNG with 4 channels
  image imgG = make_image(img.w, img.h, 1);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float r = get_pixel(img, x, y, 0);
      float g = get_pixel(img, x, y, 1);
      float b = get_pixel(img, x, y, 2);
      float Y = 0.299f *r + 0.587f *g + .114f *b; // relative luminance, green light predominant - s. https://github.com/pjreddie/vision-hw0 and Wiki!
      set_pixel(imgG, x, y, 0, Y);
    }
  }
  return imgG;
};

image rgb_to_hsv(image img) // convert RGB cube to HSV cylinder
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
      float X = C * (float)(1.0f - fabs(fmod(h,2) - 1.0f));
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

void shift_image(image img, int chan, float fshift) // shift intensity/color
{
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, chan, get_pixel(img, x, y, chan) + fshift);
    }
  }
};

void scale_image(image img, int chan, float fscale) // i.e. scale color value
{
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, chan, get_pixel(img,x,y,chan)*fscale);
    }
  }
};

// clamp only after all calculations, i.e. before save or display!
void clamp_image(image img) // limit RGB values to [0.0 ... 1.0], otherwise overflow, when converting to [0 ... 255]
{
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      for (int chan = 0; chan < img.chan; chan++)
      {
        if      (get_pixel(img, x, y, chan) < 0.0f) set_pixel(img, x, y, chan, 0.0f);
        else if (get_pixel(img, x, y, chan) > 1.0f) set_pixel(img, x, y, chan, 1.0f);
      }
    }
  }
};

// 1) make it work
// 2) make it fast
float nn_interpolate(image img, float x, float y, int chan)
{
  // 2D nearest neighbour, s. https://en.wikipedia.org/wiki/Bilinear_interpolation#/media/File:Comparison_of_1D_and_2D_interpolation.svg
  return get_pixel(img, (int)round(x), (int)round(y), chan);
};

image nn_resize(image img, int w, int h) // O(n^2)
{
  float xScale = (float)img.w / w; // not intuitive, but faster than w/img.w --> xN = 1.0f / x*xScale (s. below)
  float yScale = (float)img.h / h;

  image imgN = make_image(w, h, img.chan);

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      for (int chan = 0; chan < img.chan; chan++)
      {
        float xN = (float)x*xScale;
        float yN = (float)y*yScale;

//        float val = get_pixel(img, (int)xN, (int)yN, chan); // a) fast approach, no interpolation, just pick upper left neighbour
                                                              //    result very similar to nn_interpolate
        float val = nn_interpolate(img, xN, yN, chan);
        set_pixel(imgN, x, y, chan, val);
      }
    }
  }

  return imgN;
};

float bilinear_interpolate(image img, float x, float y, int chan) // this is very slow, as it fetches values again for next pixels
{
  float f0 = get_pixel(img, (int)x,     (int)y,     chan);
  float f1 = get_pixel(img, (int)(x+1), (int)y,     chan);
  float f2 = get_pixel(img, (int)x,     (int)(y+1), chan);
  float f3 = get_pixel(img, (int)(x+1), (int)(y+1), chan);
 
  float fX = x - (int)x;
  float fY = y - (int)y;
  float xI1 = f0 + fX * (f1 - f0); // first interpolate upper x-values
  float xI2 = f2 + fX * (f3 - f2); // then lower x-values
  return xI1 + fY * (xI2 - xI1);   // then interpolate in y-direction
};

image bilinear_resize(image img, int w, int h) // O(n^2)
{
  float xScale = (float)img.w / w; // not intuitive, but faster than w/img.w --> xN = 1.0f / x*xScale (s. below)
  float yScale = (float)img.h / h;

  image imgN = make_image(w, h, img.chan);

  for (int y = 0; y < h; y++) // h == imgN.h
  {
    for (int x = 0; x < w; x++) // w == imgN.w
    {
      for (int chan = 0; chan < img.chan; chan++) // img.chan == imgN.chan
      {
        float xN = (float)x*xScale;
        float yN = (float)y*yScale;

        float val = bilinear_interpolate(img, xN, yN, chan);
        set_pixel(imgN, x, y, chan, val);
      }
    }
  }

  return imgN;
};

void l1_normalize(image img)
{
  assert(img.chan == 1);
  float fSum = 0.0f;
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      fSum += get_pixel(img, x, y, 0); // do we get too LARGE values here ?
    }
  }
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, 0, (float)get_pixel(img, x, y, 0) / fSum); // do we get too small values here ?
    }
  }
};

image make_box_filter(const int w)
{
  image img = make_image(w, w, 1);
// float val = 1.0f / (w*w);  // do normalization right here
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, 0, 1.0f);
    }
  }
  l1_normalize(img); // s. above
  return img;
};

image make_filter_kernel(const int w, float kernel[]) // no normalization, take care yourself of proper vals!
{
  image img = make_image(w, w, 1);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(img, x, y, 0, (float)kernel[y*w+x]);
    }
  }
  return img;
};

// thanks, Wikipedia ;-) https://en.wikipedia.org/wiki/C_mathematical_functions
image make_gaussian_filter(float sigma)
{
  int w = 3;
  image img = make_image(w, w, 1);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float val = 1.0f / (2.0f * M_PI * sigma * sigma) * (float)exp(-(x*x + y*y) / (2 * sigma * sigma));
      set_pixel(img, x, y, 0, val);
    }
  }
  l1_normalize(img);
  return img;
};

image convolve_image(image img, image filter, int preserve) // O(n^4) ... terribly slow
{
  assert(img.chan == 1);
  image imgN = make_image(img.w, img.h, img.chan);
  float filtHalf = filter.w / 2.0f;
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float fConv = 0.0f;
      for (int Y = 0; Y < filter.h; Y++)
      {
        for (int X = 0; X < filter.w; X++)
        {
          // std. filter kernel:                   x+(-filtHalf + X)
          // matched (reverted) filter kernel:     x+( filtHalf - X)
          fConv += get_pixel(img, (int)(x + (-filtHalf + X)), (int)(y + (-filtHalf + Y)), 0) * get_pixel(filter, X, Y, 0);
// dbg         float fPix = get_pixel(img, (int)(x + (-filtHalf + X)), (int)(y + (-filtHalf + Y)), 0);
// dbg         float fF = get_pixel(filter, X, Y, 0);
// dbg         fConv += fPix * fF;
        }
      }
      set_pixel(imgN, x, y, 0, fConv);
    }
  }
  return imgN;
};

image* sobel_image(image img) // return gradient magnitude and direction
{
  float sobelx[9] = { -1,0,1, -2,0,2, -1,0,1 };
  float sobely[9] = { -1,-2,-1, 0,0,0, 1,2,1 };
  image filtx = make_filter_kernel(3, sobelx);
  image filty = make_filter_kernel(3, sobely);
// nok  image imgXY[2];                     // creates the array on the stack, instead use malloc (to use outside of this function) ...
  image* imgXY = malloc(2 * sizeof(image)); // ... s. https://stackoverflow.com/questions/5378768/returning-arrays-pointers-from-a-function
                                            // very helpful explanation on the above site  
  imgXY[0] = convolve_image(img, filtx, 0);
  imgXY[1] = convolve_image(img, filty, 0);
  free_image(filtx);
  free_image(filty);
  // imgXY is a vector
  // imgXY[0] is x-direction, imgS[1] y-direction
  
  // magnitude = sqrt(x*x+y*y)
  image* imgMD = malloc(2 * sizeof(image)); // ... s. https://stackoverflow.com/questions/5378768/returning-arrays-pointers-from-a-function
  imgMD[0] = make_image(img.w, img.h, img.chan);
  imgMD[1] = make_image(img.w, img.h, img.chan);
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      float fx = get_pixel(imgXY[0], x, y, 0);
      float fy = get_pixel(imgXY[1], x, y, 0);
      float mag = sqrt(fx*fx + fy*fy); // slow
      set_pixel(imgMD[0], x, y, 0, mag);
      // direction = atan(y/x), s. https://en.wikipedia.org/wiki/Sobel_operator#Formulation
      set_pixel(imgMD[1], x, y, 0, atan(fy/fx)); // slow, too -> use lookup table
    }
  }
  clamp_image(imgMD[0]); // clamp only after all calculations, i.e. before save or display!
  clamp_image(imgMD[1]);


  // cleanup
  free_image(imgXY[0]);
  free_image(imgXY[1]);
  free(imgXY);
  return imgMD;
};

image colorize_sobel(image img)
{
  image imgCol = make_image(img.w,img.h,3);
  image* pImg;
  pImg = sobel_image(img); // get Magn. + Direction
  for (int y = 0; y < img.h; y++)
  {
    for (int x = 0; x < img.w; x++)
    {
      set_pixel(imgCol, x, y, 0, get_pixel(pImg[1], x, y, 0)); // [h]sv         hue = direction
      set_pixel(imgCol, x, y, 1, get_pixel(pImg[0], x, y, 0)); // h[s]v  saturation = magnitude
      set_pixel(imgCol, x, y, 2, 1.0f);                        // hs[v]       value = 1.0f
    }
  }
  imgCol = hsv_to_rgb(imgCol);

  // cleanup
  free_image(pImg[0]);
  free_image(pImg[1]);
  free(pImg);
  return imgCol;
};
