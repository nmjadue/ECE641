#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

struct pixel{int m,n;};

double calc_sigma(unsigned char **img,
                  int width,
                int height,
              double g[5][5]);

int main(int argc, char **argv)
{
// struct pixel s;
// double T;
int i, j, k, l, m;
double sigma_x, sigma_w, v;
double theta_1, theta_2;
FILE *fp;
unsigned char **img, **e;
struct TIFF_img input_img, output_img;
double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},};
double sum, val;

/* open image file */
if ((fp = fopen("Number04_01.tif", "rb"))==NULL){
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
img = (unsigned char **)get_img(input_img.width+4, input_img.height+4, sizeof(double));
e = (unsigned char **)get_img(input_img.width, input_img.height, sizeof(double));
// img_noise = (unsigned char **)get_img(input_img.width, input_img.height, sizeof(double));


// creating an image padded with 0s to do the filter
for (i = 0; i<input_img.height+3; i++){
for (j = 0; j<input_img.width+3; j++){
  if (i<2){
    img[i][j]=0;
    // img_noise[i][j]=0;
  }
  else if (j<2){
    img[i][j]=0;
    // img_noise[i][j]=0;
  }
  else if (i>input_img.height+1){
    img[i][j]=0;
    // img_noise[i][j]=0;
  }
  else if (j>input_img.width+1){
    img[i][j]=0;
    // img_noise[i][j]=0;
  }
  else{
  img[i][j]= input_img.mono[i-2][j-2];
  // img_noise[i][j]= input_img.mono[i-2][j-2];
}
}
}

sigma_x = calc_sigma(img, input_img.width, input_img.height, g);
sigma_w = 16.0;


for (i = 2; i<input_img.height+2; i++){
for (j = 2; j<input_img.width+2; j++){
  sum = 0.0;
  for (k=0; k < 5; k++){
    for (l=0; l<5; l++){
      sum += g[k][l]*img[i+k-2][j+l-2];
    }
  }
  e[i-2][j-2] = img[i][j]-sum;
}
}

for (k = 0; k<20; k++){
  for (i = 0; i<input_img.height; i++){
  for (j = 0; j<input_img.width; j++){
    v = img[i][j];

    sum = 0.0;
    for (l=0; l < 5; l++){
      for (m=0; m<5; m++){
        sum += g[l][m]*e[i+l-2][j+m-2];
      }
    }

    theta_1 = -sum/sigma_w;


    theta_2 = 0.0;

    // calculate sum
    sum = 0.0;
    for (l=0; l < 5; l++){
      for (m=0; m<5; m++){
        sum += g[l][m]*img[i+l-2][j+m-2];
      }
    }
    sum = sum/sigma_x;
    val = (theta_2*v-theta_1+sum)/(theta_2+1/sigma_x);
    if (val > 0){
      img[i][j] = val;
    }
    else{
      img[i][j] = 0;
    }
// Update e
    // e = 0.0;
  }
}
}

for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  output_img.mono[i][j] = img[i+1][j+1];
}
}

if((fp=fopen("Number04_02.tif", "wb"))==NULL){
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

double calc_sigma(unsigned char **img,
                  int width,
                  int height,
                  double g[5][5]){
  int i,j, k, l;
  double sum=0.0;

  for (i =1; i<height+1; i++ ){
  for (j = 1; j<width+1; j++){
    for (k = 0; k<5; k++){
    for (l = 0; l<5; l++){
      sum += g[k][l]*(img[i][j]-img[i+k-2][j+l-2]);
    }
    }
  }
}
  return sum/(width*height);


}
