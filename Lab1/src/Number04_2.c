#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

double calc_sigma(double **img,
                  int width,
                  int height,
                  double power);

int main(int argc, char **argv)
{

int i, j, k, l, m, col, row;
FILE *fp;
double sum, **img, **e, v, theta_1, theta_2, val;
double sigma_x, sigma_w, cost, g_sum, y_sum;
double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0}};
struct TIFF_img input_img, output_img;


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
img = (double **)get_img(input_img.width, input_img.height, sizeof(double));
e = (double **)get_img(input_img.width, input_img.height, sizeof(double));

// initializing x<- y
for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  img[i][j] = input_img.mono[i][j];
}
}

// selecting sigma_x and sigma_w
sigma_x = 16.95;
sigma_w = 16.0;
printf("sigma_x: %f\n", sigma_x);

// initializing e <- y-Hx
for (i = 0; i<input_img.height; i++){
for (j = 0; j<input_img.width; j++){
  sum = 0.0;
  for (l=0; l<5; l++){
  for (m=0; m<5; m++){
    row = (i+l-2+input_img.height)%input_img.height;
    col = (j+m-2+input_img.width)%input_img.width;
    sum += g[l][m]*img[row][col];
  }
  }
  e[i][j] = input_img.mono[i][j]-sum;

}
}
fp = fopen("cost_function_2_4.txt", "w+");

for (k=0; k<20; k++){

  for (i = 0; i<input_img.height; i++){
  for (j = 0; j<input_img.width; j++){
    v = img[i][j];
    theta_1 = 0.0;
    theta_2 = 0.0;
    for (l=0; l<5; l++){
    for (m=0; m<5; m++){
      row = (i+l-2+input_img.height)%input_img.height;
      col = (j+m-2+input_img.width)%input_img.width;
      theta_1 += g[l][m]*e[row][col];
      theta_2 += g[l][m]*g[l][m];
    }
    }
    theta_1 = -theta_1/sigma_w;
    theta_2 = theta_2/sigma_w;


    sum = 0.0;
    for (l=0; l<5; l++){
    for (m=0; m<5; m++){
      row = (i+l-2+input_img.height)%input_img.height;
      col = (j+m-2+input_img.width)%input_img.width;
      sum += g[l][m]*img[row][col];
    }
    }
    val = (theta_2*v-theta_1+sum/sigma_x)/(theta_2+1/sigma_x);
    if (val < 0){
      val = 0;
    }

    img[i][j] = val;

    for (l=0; l<5; l++){
    for (m=0; m<5; m++){
      row = (i+l-2+input_img.height)%input_img.height;
      col = (j+m-2+input_img.width)%input_img.width;
      e[row][col] -= g[l][m]*(val-v);
    }
    }
  }
  }

  g_sum = 0.0;
  y_sum = 0.0;
  for (i = 0; i<input_img.height; i++){
  for (j = 0; j<input_img.width; j++){
    for (l=0; l<5; l++){
    for (m=0; m<5; m++){
        row = (i+l-2+input_img.height)%input_img.height;
        col = (j+m-2+input_img.width)%input_img.width;
        if(l!= 2 && m!=2){
        g_sum += g[l][m]*pow(img[i][j]-img[row][col], 2);
      }
      }

    }

    y_sum = e[i][j]*e[i][j];
  }
}

cost = y_sum/(2*sigma_w)+g_sum/(4*sigma_x);
  printf("cost: %f\n", cost);
  fprintf(fp, "%f\n", cost);

}
fclose(fp);





for (i=0; i< input_img.height; i++){
for (j=0; j< input_img.width; j++){
  if(img[i][j] > 255){
    output_img.mono[i][j] = 255;
  }
  else if(img[i][j] < 0){
    output_img.mono[i][j] = 0;
  }
  else{
  output_img.mono[i][j] = img[i][j];
}
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

double calc_sigma(double **img,
                  int width,
                  int height,
                  double power){
  int i,j, m, n, row, col;
  double sum=0.0;
  double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                    {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                    {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                    {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                    {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0}};


  for (i =0; i<height; i++ ){
  for (j = 0; j<width; j++){
    for (m=0; m<5; m++){
    for (n=0; n<5; n++){
      row = (i+m-2+height)%height;
      col = (j+n-2+width)%width;
      if (m!=2 && n!= 2){
      sum += g[m][n]*pow(fabs((double)img[i][j]-(double)img[row][col]), power);
    }
    }
    }
  }
}
  return sum/(width*height);


}
