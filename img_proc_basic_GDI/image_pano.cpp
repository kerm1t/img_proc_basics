#include "image_pano.h"

image structure_matrix(const image im, const float sigma)
{
  return make_image(0, 0, 0);
};

/*
Calculate image derivatives Ix and Iy.
Calculate measures IxIx, IyIy, and IxIy.
Calculate structure matrix components as weighted sum of nearby measures.
Calculate Harris "cornerness" as estimate of 2nd eigenvalue: det(S) - alpha trace(S)^2, alpha = .06
Run non-max suppression on response map
*/
descriptor* harris_corner_detector(const image img, const float sigma, const float thresh, const int nms, int* n) // n = num corners found
{
  descriptor p;
  return &p;
};

match* match_descriptors(const descriptor ad, const int an, const descriptor bd, const int bn, int* mn) // mn = match number!?
{
  match m;
  return &m;
};

matrix RANSAC(const match m, const int mn, const float inlier_thresh, const int iters, const int cutoff)
{
  matrix mat;
  return mat;
};

image combine_images(const image a, const image b, const matrix H)
{
  return make_image(0, 0, 0);
};

image panorama_image(image a, image b, float sigma, float thresh, int nms, float inlier_thresh, int iters, int cutoff)
{
  int an;
  int bn;
  int mn;

  // Calculate corners and descriptors
  descriptor *ad = harris_corner_detector(a, sigma, thresh, nms, &an);
  descriptor *bd = harris_corner_detector(b, sigma, thresh, nms, &bn);

  // Find matches
  match *m = match_descriptors(*ad, an, *bd, bn, &mn);

  // Run RANSAC to find the homography
  matrix H = RANSAC(*m, mn, inlier_thresh, iters, cutoff);

  // Stitch the images together with the homography
  image comb = combine_images(a, b, H);
  return comb;
}
