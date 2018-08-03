#include <ncurses.h>

#include "common.h"
#include "driver.h"

#define ABS(x)      ((x) < 0 ? -(x) : (x))
#define ROUND(x)    (int)((x) + 0.5)

void draw_line_dda(int x1, int y1, int x2, int y2){
    int dx = x2 - x1;
    int dy = y2 - y1;

    int adx = ABS(dx);
    int ady = ABS(dy);

    int steps;

    if(adx > ady)
        steps = adx;
    else
        steps = ady;

    double xinc = (double)adx/steps;
    double yinc = (double)ady/steps;

    double x = x1, y = y1;

    put_pixel(x1, y1);

#ifdef ENHANCED_DDA
    for(;x < x2;){
#else
    for(int i = 0;i < dx;i++){
#endif
        x = x + xinc;
        y = y + yinc;
        put_pixel(ROUND(x), ROUND(y));
    }
}

void draw_line_bresenham(int x1, int y1, int x2, int y2){
    int dy = ABS(y1 - y2);
    int dx = ABS(x1 - x2);
    int x = x1;
    int y = y1;

    put_pixel(x, y);

    int p = 2*dy - dx;
    for(int i = 0;i < dx;i++){
        if(x < x2)
            x++;
        else
            x--;

        if(p >= 0){
            if(y < y2)
                y++;
            else
                y--;
            p = p + 2*(dy -dx);
        }
        else
            p = p + 2*dy;

        put_pixel(x, y);
    }
}

void draw_line_midpoint(int x1, int y1, int x2, int y2){
    int dy = ABS(y2 - y1);
    int dx = ABS(x2 - x1);
    int a = dy;
    int b = -dx;
    double x = x1;
    double y = y1;

    put_pixel(x, y);
    double p = a + (double)(b/2);
    
    while(x < x2){
        if(p < 0){
            p = p + a + b;
            y += 1.5;
        }
        else{
            p = p + a;
            y += 0.5;
        }
        x++;
        put_pixel(x, y);
    }
}
