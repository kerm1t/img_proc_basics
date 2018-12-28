#ifndef IMAGE_C_H
#define IMAGE_C_H

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  int w;
  int h;
  int chan;

  // In our data array we store the image in CHW format.
  // The first pixel in data is at channel 0, row 0, column 0.
  // The next pixel is channel 0, row 0, column 1, then channel 0, row 0, column 2, etc.
  float * data; // [0.0 ... 1.0]
} image;

image make_image(const int w, const int h, const int chan);
void free_image(image img);
image load_image(const char* filename);
void save_image(image img, const char* filename);
image copy_image(const image img);

float get_pixel(image img, int x, int y, int chan);
void set_pixel(image img, int x, int y, int chan, float val);

// global manipulate
image rgb_to_grayscale(image img);
image rgb_to_hsv(image img);
image hsv_to_rgb(image img);
void shift_image(image img, int chan, float fshift);
void scale_image(image img, int chan, float val);
void clamp_image(image img);

float nn_interpolate(image img, float x, float y, int chan);
image nn_resize(image img, int w, int h);
float bilinear_interpolate(image img, float x, float y, int chan);
image bilinear_resize(image img, int w, int h);

void l1_normalize(image img);
image make_box_filter(const int w);
image make_filter_kernel(const int w, float kernel[]);
image make_gaussian_filter(float sigma);
image convolve_image(image img, image filter, int preserve);
image* sobel_image(image img);
void feature_normalize(image img); // -> global manipulate
image colorize_sobel(image img);

#ifdef __cplusplus
}
#endif

#endif // #define IMAGE_C_H
