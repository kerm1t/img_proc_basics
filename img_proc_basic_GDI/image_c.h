#ifndef IMAGE_C_H
#define IMAGE_C_H

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

enum img_type { basic = 0, uint = 2, real = 4, complex = 8 };

typedef struct
{
  int w;
  int h;
  int chan;
//  img_type type;

  // In our data array we store the image in CHW format.
  // The first pixel in data is at channel 0, row 0, column 0.
  // The next pixel is channel 0, row 0, column 1, then channel 0, row 0, column 2, etc.
  unsigned char * data;

} image;

image make_image(const int w, const int h, const int chan);
void free_image(image img);
image load_image(const char* filename);
void save_image(image img, const char* filename);
image copy_image(image img);

float get_pixel(image img, int x, int y, int chan);
void set_pixel(image img, int x, int y, int chan, float val);

// manipulate
image rgb_to_grayscale(image img);
void shift_image(image img, int chan, float val);
void clamp_image(image img);

#ifdef __cplusplus
}
#endif

#endif // #define IMAGE_C_H
