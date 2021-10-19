#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

int main(int argc, char **argv)
{
// struct pixel s;
// double T;
int i, j, k, m, n;
double sigma_x, sigma_c;
FILE *fp;
double **img;
double cost, y_sum, g_sum;
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
img = (double **)get_img(input_img.width, input_img.height, sizeof(double));


for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  img[i][j] = input_img.mono[i][j];
}
}
sigma_x = 14.939826*14.939826;
// sigma_x = sigma_x/5.0;
sigma_c = 256.0/sigma_x;

fp = fopen("cost_function.txt", "w+");
for (k = 0; k<20; k++){
  y_sum = 0.0;
  g_sum = 0.0;
  for (i = 1; i<input_img.height-1; i++){
  for (j = 1; j<input_img.width-1; j++){
    sum = 0.0;

    for(m=0; m<3; m++){
    for(n=0; n<3; n++){
      sum += g[m][n]*img[m+i-1][n+j-1];
    }
    }
    val = (input_img.mono[i-1][j-1]+sigma_c*sum)/(1+sigma_c);

    if (val<0){
      val = 0.0;
    }

    img[i][j]=val;
    y_sum += (input_img.mono[i-1][j-1]-val)*(input_img.mono[i-1][j-1]-val);



  }
  }
  for (i = 1; i<input_img.height-1; i++){
  for (j = 1; j<input_img.width-1; j++){
  for(m=0; m<3; m++){
  for(n=0; n<3; n++){
      g_sum += g[m][n]*(img[i+m-1][j+n-1]-img[i][j])*(img[i+m-1][j+n-1]-img[i][j]);
  }
  }
}
}
// printf("y: %f\n", y_sum);
// printf("g: %f\n", g_sum);
  // put cost function calc here
  cost = y_sum/(2*256.0)+g_sum/(4*sigma_x);
  printf("cost: %f\n", cost);
  fprintf(fp, "%f\n", cost);


}
fclose(fp);
for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  output_img.mono[i][j] = img[i][j];
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
