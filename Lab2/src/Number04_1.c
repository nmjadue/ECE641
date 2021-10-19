#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

double noise(double stdev,
            double mu);

int main(int argc, char **argv)
{

int i, j, k, l, col, row;
FILE *fp;
double sum, rand;
double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},};
struct TIFF_img input_img, output_img;


/* open image file */
if ((fp = fopen("img04g.tif", "rb"))==NULL){
 fprintf(stderr, "cannot open file\n");
 exit(1);
}

/* read image */
if (read_TIFF(fp, &input_img)){
 fprintf(stderr, "error reading file \n");
 exit(1);
}

/* close image */
fclose(fp);


printf("Image type: %c \n", input_img.TIFF_type);
printf("Image dimensions: (%i,%i) \n", input_img.width, input_img.height);
get_TIFF(&output_img, input_img.height, input_img.width, 'g');


for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  sum = 0.0;
  for (k=0; k < 5; k++){
    for (l=0; l<5; l++){
      row = (i+k-2+input_img.height)%input_img.height;
      col = (j+l-2+input_img.width)%input_img.width;
      sum += g[k][l]*input_img.mono[row][col];
    }
  }

  // adding noise and clipping values
    rand = noise(4,0);
    if (sum+rand < 0){
      output_img.mono[i][j] = 0;
    }
    else if (sum+rand > 255){
      output_img.mono[i][j] = 255;
    }
    else{
      output_img.mono[i][j] = sum+rand;
   }
}
}

if((fp=fopen("Blur_Noise.tif", "wb"))==NULL){
fprintf(stderr, "cannot open file\n");
exit(1);
}

if (write_TIFF(fp, &output_img)){
fprintf(stderr, "error writing TIFF file");
exit(1);
}

fclose(fp);



return (0);
}


double noise(double stdev,
            double mu)
{
  double u, v, s;
  u = 0;
  v = 0;
  s = u*u+v*v;
  while (s >= 1 || s <= 0){
    // generating random numbers in [0,1]:
    u = rand()/((double)RAND_MAX);
    v = rand()/((double)RAND_MAX);
    // scaling them to be in -1 to 1
    u = u*2.0-1.0;
    v = v*2.0-1.0;
    s = u*u+v*v;
  }
  s = sqrt(-2.0 * log(s) / s);

  return mu +  stdev * u * s;

}
