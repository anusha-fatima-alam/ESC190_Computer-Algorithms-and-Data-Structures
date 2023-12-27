
#include "seamcarving.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int change_x(struct rgb_img *im, int y, int x){
    // red = r, blue = b, green = g
    if(x == 0){
        int r=-get_pixel(im,y,x+1,0)+get_pixel(im, y,(im->width)-1,0);
        int g=-get_pixel(im,y,x+1,1)+get_pixel(im, y,(im->width)-1,1);
        int b=-get_pixel(im,y,x+1,2)+get_pixel(im, y,(im->width)-1,2);
        return (r*r)+(g*g)+(b*b);
    }else if(x==im->width-1){
        int r=-get_pixel(im,y,x-1,0)+get_pixel(im,y,0,0);
        int g=-get_pixel(im,y,x-1,1)+get_pixel(im,y,0,1);
        int b=-get_pixel(im,y,x-1,2)+get_pixel(im,y,0,2);
        return (r*r)+(g*g)+(b*b);
    }else{
        int r=-get_pixel(im,y,x-1,0)+get_pixel(im,y,x+1,0);
        int g=-get_pixel(im,y,x-1,1)+get_pixel(im,y,x+1,1);
        int b=-get_pixel(im,y,x-1,2)+get_pixel(im,y,x+1,2);
        return (r*r)+(g*g)+(b*b);
    }
}
//Checked 
int change_y(struct rgb_img *im, int y, int x){
    if(y==0){
        int g=-get_pixel(im,y+1,x,1)+get_pixel(im,(im->height)-1,x,1);
        int b=-get_pixel(im,y+1,x,2)+get_pixel(im,(im->height)-1,x,2);
        int r=-get_pixel(im,y+1,x,0)+get_pixel(im,(im->height)-1,x,0);
        return (r*r)+(g*g)+(b*b);
    }else if(y==im->height-1){
        int r=-get_pixel(im,y-1,x,0)+get_pixel(im,0,x,0);
        int b=-get_pixel(im,y-1,x,2)+get_pixel(im,0,x,2);
        int g=-get_pixel(im,y-1,x,1)+get_pixel(im,0,x,1);
        return (r*r)+(g*g)+(b*b);
    }else{
        int r=-get_pixel(im,y-1,x,0)+get_pixel(im,y+1,x,0);
        int g=-get_pixel(im,y-1,x,1)+get_pixel(im,y+1,x,1);
        int b=-get_pixel(im,y-1,x,2)+get_pixel(im,y+1,x,2);
        return (r*r)+(g*g)+(b*b);
    }
}
void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    // red = r, blue = b, green = g
    create_img(grad, im->height, im->width);
    int m=0, k=0;
    while(m<im->height){
        k=0;
        while(k<im->width){
            int x_change=change_x(im,m,k);
            int y_change=change_y(im,m,k);
            double E=((double)sqrt((double)y_change+(double)x_change))/10;
            int set_E=(uint8_t)(E);
            set_pixel(*grad, m, k, set_E, set_E, set_E);
            k++;
        } m++;
    }
}

double min_array(int y, int x, double *array, int width){
    if(x==0){
        return array[(y-1)*width+x]>array[(y-1)*width+x+1]?array[(y-1)*width+x+1]:array[(y-1)*width+x];
    }else if(x==width-1){
        return array[(y-1)*width+x]>array[(y-1)*width+x-1]?array[(y-1)*width+x-1]:array[(y-1)*width+x];
    }else{
        double current_minimum=array[(y-1)*width+x]>array[(y-1)*width+x+1]?array[(y-1)*width+x+1]:array[(y-1)*width+x];
        return current_minimum>array[(y-1)*width+x-1]?array[(y-1)*width+x-1]:current_minimum;
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr=(double *)malloc(sizeof(double)*(grad->width)*(grad->height));
    int a=0, b=0, c=0;
    while(a<grad->height){
        b=0;
        while (b<grad->width){
            if(a==0){
                (*best_arr)[c] = get_pixel(grad, a, b, 0);
                c++;
            }else if(a > 0){
                (*best_arr)[c] = (double)get_pixel(grad, a, b, 0) + (double)min_array(a, b, *best_arr, grad->width);
                c++;
            }b++;
        }a++;
    }
}

void recover_path(double *best, int height, int width, int **path){
    int i_minimum=2147483647,a=0,b=0,i=0,current_column=0;
    *path=(int *)malloc(sizeof(int) * height);
    while(b<width){
        if(best[a*width+b]<i_minimum){
            i_minimum=(int)best[a*width+b],current_column=b;
        }b++;
    }
    (*path)[i]=current_column;
    i++;
    while(a<height-1){
        if(current_column==0){
            if((int)(best[(a+1)*width+current_column])>(int)(best[(a+1)*width+current_column+1])){
                current_column++,(*path)[i]=current_column,i++;
            }else{
                (*path)[i]=current_column,i++;
            }
        }else if(current_column==width-1){
            if((int)(best[(a+1)*width+current_column])>(int)(best[(a+1)*width+current_column-1])){
                current_column--,(*path)[i]=current_column,i++;
            }else{
                (*path)[i]=current_column,i++;
            }
        }else{
            if(best[(a+1)*width+current_column] > best[(a+1)*width+current_column+1]){
                int temporary=current_column+1;
                if(best[(a+1)*width+temporary]>best[(a+1)*width+current_column-1]){
                    current_column--,(*path)[i]=current_column,i++;
                }else{
                    current_column++,(*path)[i]=current_column,i++;
                }
            }else if((int)(best[(a+1)*width+current_column])<(int)(best[(a+1)*width+current_column+1])){
                int temporary=current_column;
                if((int)(best[(a+1)*width+temporary]) >(int)(best[(a+1)*width+current_column-1])){
                    current_column--,(*path)[i]=current_column,i++;
                }else{
                    (*path)[i]=current_column,i++;
                }
            }
        }a++;
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int* path){
    create_img(dest, src->height, (src->width)- 1);
    int a, b = 0;
    while (b<(*dest)->height){
        a=0;
        while (a<(*dest)->width){
            if (a==path[a]){
                continue;
            }else{
                set_pixel(*dest,b,a,get_pixel(src,b,a,0),get_pixel(src,b,a,1),get_pixel(src,b,a,0));
                a++;
            }
        }b++;
    }
}