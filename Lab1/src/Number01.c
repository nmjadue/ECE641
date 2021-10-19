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
int i, j, m, n;
FILE *fp;
unsigned char **img;
double sum;
double g[3][3] = {{1.0/12.0, 1.0/6.0, 1.0/12.0},
                  {1.0/6.0, 0.0, 1.0/6.0},
                  {1.0/12.0, 1.0/6.0, 1.0/12.0}};
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
}
}


for (i = 1; i<input_img.height+1; i++){
for (j = 1; j<input_img.width+1; j++){
  sum = img[i][j];
  for (m = 0; m<3; m++){
  for (n = 0; n<3; n++){
    sum -= g[m][n]*img[i-1+m][j-1+n];
  }
  }
  sum += 127;
  if (sum < 0){
    output_img.mono[i-1][j-1] = 0;
  }
  else if (sum > 255){
    output_img.mono[i-1][j-1] = 255;
  }
  else{
    output_img.mono[i-1][j-1] = sum;
  }

}
}

if((fp=fopen("Number01.tif", "wb"))==NULL){
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
