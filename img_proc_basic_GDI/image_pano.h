#ifndef __IMAGE_PANO__
#define __IMAGE_PANO__

#include "image_c.h"

typedef struct {
} descriptor;

typedef struct {
} match;

typedef struct {
} matrix;

image structure_matrix(const image im, const float sigma); 

descriptor* harris_corner_detector(const image img, const float sigma, const float thresh, const int nms, int* n); // n = num corners found

match* match_descriptors(const descriptor ad, const int an, const descriptor bd, const int bn, int* mn); // mn = match number!?

matrix RANSAC(const match m, const int mn, const float inlier_thresh, const int iters, const int cutoff);

image combine_images(const image a, const image b, const matrix H);

#endif // __IMAGE_PANO__
