#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"
#include "solve.h"

double calc_sigma(double **img,
                  double power);
static double f(double x, void * pblock);

double g[5][5] = {{1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {3.0/81.0, 6.0/81.0, 9.0/81.0, 6.0/81.0, 3.0/81.0},
                  {2.0/81.0, 4.0/81.0, 6.0/81.0, 4.0/81.0, 2.0/81.0},
                  {1.0/81.0, 2.0/81.0, 3.0/81.0, 2.0/81.0, 1.0/81.0}};
double v, **e, **img;
double sigma_x, sigma_w;
int width, height;

struct Parameters{double theta_1, theta_2;
int i,j;};


// double f(double x, void * pblock);

int main(int argc, char **argv)
{

int i, j, k, l, m;
int col, row;
FILE *fp;
double sum;
double cost, g_sum, y_sum;
int code;
double a, b;
struct Parameters param;
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
sigma_x = (pow(sigma_x, 1.2))/5.0;
printf("sigma_x %f\n", sigma_x);

// fp = fopen("cost_function_1_1.txt", "w+");

for(k=0; k<20; k++){
  y_sum = 0.0;
  g_sum = 0.0;
  for (i=0; i< height; i++){
  for (j=0; j< width; j++){
    param.i = i;
    param.j = j;
    v= img[i][j];
    param.theta_1 = 0.0;
    param.theta_2 = 0.0;
    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      param.theta_1 += e[row][col]*g[l][m];
      param.theta_2 += g[l][m]*g[l][m];
    }
    }

    param.theta_1 = -param.theta_1/sigma_w;
    param.theta_2 = param.theta_2/sigma_w;


    a = img[(i-2+height)%height][(j-2+width)%width];
    b = img[(i-2+height)%height][(j-2+width)%width];
    // Finding a and b
    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      if (img[row][col]<a){
        a = img[row][col];
      }
      if (img[row][col]>b){
        b = img[row][col];
      }
    }
    }

    if (a > v-param.theta_1/param.theta_2){
      a = v-param.theta_1/param.theta_2;
    }
    if (b < v-param.theta_1/param.theta_2){
      b = v - param.theta_1/param.theta_2;
    }

    img[i][j] = solve(f, &param, a,b, 1e-7, &code);

    for (l=0; l< 5; l++){
    for (m=0; m< 5; m++){
      row = (i+l-2+height)%height;
      col = (j+m-2+width)%width;
      e[row][col] -= g[l][m]*(img[i][j]-v);
    }
    }


  }
  }


  // for (i = 0; i<input_img.height; i++){
  // for (j = 0; j<input_img.width; j++){
  //   for (l=0; l<5; l++){
  //   for (m=0; m<5; m++){
  //       row = (i+l-2+input_img.height)%input_img.height;
  //       col = (j+m-2+input_img.width)%input_img.width;
  //       g_sum += g[l][m]*pow(fabs(img[i][j]-img[row][col]), 1.2);
  //     }
  //
  //   }
  //
  //   y_sum = e[i][j]*e[i][j];
  // }
// }

  // cost = y_sum/(2*sigma_w)+g_sum/(1.2*sigma_x*2);
cost = 0.0;
  printf("%d:%f\n", k, cost);
//   fprintf(fp, "%f\n", cost);

}
// fclose(fp);


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

if((fp=fopen("02_Number01_01.tif", "wb"))==NULL){
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
      sum += g[m][n]*pow(fabs((double)img[i][j]-(double)img[row][col]), power);
    }
    }
  }
  }
return pow(sum/(2.0*width*height), 1/power);


}

static double f(double x, void * pblock)
{
  struct Parameters * p = (struct Parameters *) pblock;
  double sum;
  int m,l, col, row;
  sum = 0.0;
  for (l=0; l<5; l++){
  for (m=0; m<5; m++){
    row = (p->i+l-2+height)%height;
    col = (p->j+m-2+width)%width;
    if (x-img[row][col] < 0){
      sum -= pow(fabs(x-img[row][col]), 0.2)*g[l][m];
    }
  else{
    sum += pow(fabs(x-img[row][col]), 0.2)*g[l][m];
  }

  }
  }
  return p->theta_1+p->theta_2*(x-v)+sum/pow(sigma_x, 1.2);
}
