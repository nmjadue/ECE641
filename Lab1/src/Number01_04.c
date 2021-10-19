#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

double calc_sigma(unsigned char **img,
                  int width,
                  int height,
                  double power);

int main(int argc, char **argv)
{

int i, j;
FILE *fp;
unsigned char **img;
double sigma_x;
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
img = (unsigned char **)get_img(input_img.width+2, input_img.height+2, sizeof(double));


//creating an image padded with 0s to do the filter
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
}
}

fp = fopen("sigma_vals.txt", "w+");
for (i=1; i<21; i++){
sigma_x = calc_sigma(img, input_img.width, input_img.height, i*0.1);
printf("%f\n", sigma_x);
fprintf(fp, "%f\n", sigma_x);
}

fclose(fp);



return (0);
}

double calc_sigma(unsigned char **img,
                  int width,
                  int height,
                  double power){
  int i,j, m, n;
  double sum=0.0;
  double g[3][3] = {{1.0/12.0, 1.0/6.0, 1.0/12.0},
                    {1.0/6.0, 0.0, 1.0/6.0},
                    {1.0/12.0, 1.0/6.0, 1.0/12.0}};

  for (i =2; i<height; i++ ){
  for (j = 2; j<width; j++){
    for (m=0; m<3; m++){
    for (n=0; n<3; n++){
      sum += g[m][n]*pow(fabs((double)img[i][j]-(double)img[i+m-1][j+m-1]), power);
    }
    }
  }
}
  return pow(sum/(2.0*width*height), 1/power);


}
