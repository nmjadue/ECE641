#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"
#include "qGGMRF.h"

double calc_sigma(double **img,
                  double power);

double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0}};
double **e, **img;
double sigma_x, sigma_w;
int width, height;


// double f(double x, void * pblock);

int main(int argc, char **argv)
{

int i, j, k, l, m;
int col, row;
FILE *fp;
double sum;
double cost, g_sum, y_sum;
double theta_1, theta_2, alpha, diff;
double b[5][5];
struct TIFF_img input_img, output_img;


/* open image file */
if ((fp = fopen("Blur_Noise.tif", "rb"))==NULL){
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
width = input_img.width;
height = input_img.height;

printf("Image type: %c \n", input_img.TIFF_type);
printf("Image dimensions: (%i,%i) \n", width, height);
get_TIFF(&output_img, height, width, 'g');
img = (double **)get_img(width, height, sizeof(double));
e = (double **)get_img(width, height, sizeof(double));

// Initialize x<-y
for (i=0; i< height; i++){
for (j=0; j< width; j++){
  img[i][j] = input_img.mono[i][j];
}
}

// Initialize e<-y-Hx
for (i=0; i< height; i++){
for (j=0; j< width; j++){
  sum = 0.0;
  for (l=0; l< 5; l++){
  for (m=0; m< 5; m++){
    row = (i+l-2+height)%height;
    col = (j+m-2+width)%width;
    sum += g[l][m]*img[row][col];
  }
  }
}
}

// Selecting desired sigma_x and sigma_w
sigma_w = 16.0;
sigma_x = calc_sigma(img, 1.2);
sigma_x += 5;
printf("sigma_x %f\n", sigma_x);

fp = fopen("cost_function_2_1.txt", "w+");
alpha = 0.0;
for(k=0; k<20; k++){
  y_sum = 0.0;
  g_sum = 0.0;
  for (i=0; i< height; i++){
  for (j=0; j< width; j++){
    theta_1 = 0.0;
    theta_2 = 0.0;
    // Setting b_tilde
    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      if (l != 2 && m != 2){
        b[l][m] = get_btilde(img[i][j]-img[row][col], g[l][m], sigma_x, 1.2, 2.0, 1.0);
      }
    }
    }
    b[2][2] = 0;

    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      theta_1 -= (e[row][col]*g[l][m])/sigma_w;
      theta_1 += 2*b[l][m]*(img[i][j]-img[row][col]);
      theta_2 += g[l][m]*g[l][m]/sigma_w+2*b[l][m];
    }
    }

    alpha = -theta_1/theta_2;

    if (alpha < -img[i][j]){
      alpha = -img[i][j];
    }

    img[i][j] += alpha;

    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      e[row][col] -= g[l][m]*alpha;
    }
    }


  }
  }


  for (i = 0; i<input_img.height; i++){
  for (j = 0; j<input_img.width; j++){
    for (l=0; l<5; l++){
    for (m=0; m<5; m++){
        row = (i+l-2+input_img.height)%input_img.height;
        col = (j+m-2+input_img.width)%input_img.width;
        diff = fabs(img[i][j]-img[row][col]);
        g_sum += g[l][m]*pow(diff, 1.2)*(pow(diff/sigma_x, 0.8))/(1+pow(diff/sigma_x, 0.8));
      }

    }

    y_sum = e[i][j]*e[i][j];
  }
}

  cost = y_sum/(2*sigma_w)+g_sum/(1.2*pow(sigma_x, 1.2)*2);
  printf("%d:%f\n", k, cost);
  fprintf(fp, "%f\n", cost);

}
fclose(fp);


for (i=0; i< height; i++){
for (j=0; j< width; j++){
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

if((fp=fopen("Number02_01.tif", "wb"))==NULL){
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
                  double power){
  int i,j, m, n, row, col;
  double sum=0.0;



  for (i =0; i<height; i++ ){
  for (j = 0; j<width; j++){
    for (m=0; m<5; m++){
    for (n=0; n<5; n++){
      row = (i+m-2+height)%height;
      col = (j+n-2+width)%width;
      sum += g[m][n]*pow(fabs(img[i][j]-img[row][col]), power);
    }
    }
  }
  }
return pow(sum/(2.0*width*height), 1/power);


}
