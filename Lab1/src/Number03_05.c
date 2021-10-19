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
                double power);

int main(int argc, char **argv)
{
// struct pixel s;
// double T;
int i, j,k, l,m;
double sigma_x, sigma_c, y_sum, x_sum, c;
FILE *fp;
unsigned char **img, **img_noise;
struct TIFF_img input_img, output_img;
double g[3][3] = {{1.0/12.0, 1.0/6.0, 1.0/12.0},
                  {1.0/6.0, 0.0, 1.0/6.0},
                  {1.0/12.0, 1.0/6.0, 1.0/12.0}};
double sum, val;

/* open image file */
if ((fp = fopen("Number03.tif", "rb"))==NULL){
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
img = (unsigned char **)get_img(input_img.width+2, input_img.height+2, sizeof(double));
img_noise = (unsigned char **)get_img(input_img.width, input_img.height, sizeof(double));

// creating an image padded with 0s to do the filter
for (i = 0; i<input_img.height+2; i++){
for (j = 0; j<input_img.width+2; j++){
  if (i==0){
    img[i][j]=0;
  }
  else if (j==0){
    img[i][j]=0;
  }
  else if (i==input_img.height+1){
    img[i][j]=0;
  }
  else if (j==input_img.width+1){
    img[i][j]=0;
  }
  else{
  img[i][j]= input_img.mono[i-1][j-1];
}
if (i < input_img.height && j < input_img.width){
  img_noise[i][j]= input_img.mono[i][j];
}
}
}

sigma_x = calc_sigma(img, input_img.width, input_img.height,2.0);
// sigma_x = sigma_x/5.0;
printf("%f\n", sigma_x);

// sigma_x = sigma_x/5.0;
sigma_c = 256.0/sigma_x;
fp = fopen("sigma_vals_number3.txt", "w+");
for (k = 0; k<20; k++){
  y_sum = 0.0;
  x_sum = 0.0;
  for (i = 1; i<input_img.height; i++){
  for (j = 1; j<input_img.width; j++){
    sum = 0.0;
    sum += g[0][0]*img[i-1][j-1];
    sum += g[1][0]*img[i-1][j-1];
    sum += g[2][0]*img[i+1][j-1];
    sum += g[0][1]*img[i][j-1];
    sum += g[2][1]*img[i+1][j-1];
    sum += g[0][2]*img[i-1][j+1];
    sum += g[1][2]*img[i][j+1];
    sum += g[2][2]*img[i+1][j+1];
    val = (img_noise[i-1][j-1]+sigma_c*sum)/(1+sigma_c);
    if (val<0){
      img[i][j] = 0;
    }
    else{
      img[i][j]=val;
    }

  }
  }

  for (i = 1; i<input_img.height; i++){
  for (j = 1; j<input_img.width; j++){
    y_sum = pow(fabs((double)img_noise[i-1][j-1]-(double)img[i][j]), 2.0);
    for (l=0; l<3; l++){
    for (m=0; m<3; m++){
      x_sum += g[l][m]*pow(fabs((double)img[i][j]-(double)img[i-1+l][j-1+m]),2.0);
    }
    }
  }
}

c = y_sum/(2*256.0)+x_sum/(2*sigma_x);
fprintf(fp, "%f\n", c);

}

fclose(fp);

for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  output_img.mono[i][j] = img[i+1][j+1];
}
}

if((fp=fopen("Reconstruction_sigmax.tif", "wb"))==NULL){
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
                  double power){
  int i,j;
  double sum=0.0;
  double g[3][3] = {{1.0/12.0, 1.0/6.0, 1.0/12.0},
                    {1.0/6.0, 0.0, 1.0/6.0},
                    {1.0/12.0, 1.0/6.0, 1.0/12.0}};

  for (i =2; i<height; i++ ){
  for (j = 2; j<width; j++){
    sum += g[0][0]*pow(fabs((double)img[i][j]-(double)img[i-1][j-1]), power);
    sum += g[1][0]*pow(fabs((double)img[i][j]-(double)img[i][j-1]), power);
    sum += g[2][0]*pow(fabs((double)img[i][j]-(double)img[i+1][j-1]), power);
    sum += g[0][1]*pow(fabs((double)img[i][j]-(double)img[i-1][j]), power);
    sum += g[1][1]*pow(fabs((double)img[i][j]-(double)img[i][j]), power);
    sum += g[2][1]*pow(fabs((double)img[i][j]-(double)img[i+1][j]), power);
    sum += g[0][2]*pow(fabs((double)img[i][j]-(double)img[i-1][j+1]), power);
    sum += g[1][2]*pow(fabs((double)img[i][j]-(double)img[i][j+1]), power);
    sum += g[2][2]*pow(fabs((double)img[i][j]-(double)img[i+1][j+1]), power);
  }
}
  return sum/(width*height);


}
