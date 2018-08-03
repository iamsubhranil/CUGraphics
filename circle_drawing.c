#include <math.h>

#include "display.h"
#include "driver.h"

static void circle_8_points(int a, int b, int xd, int yd){
    int x = xd - a;
    int y = yd - b;

    //pdbg("(%d, %d)\t(%d, %d)\t(%d, %d)\t(%d, %d)", x, y, -x, y, -x, -y, x, -y);
    put_pixel(a + x, b + y);
    put_pixel(a - x, b + y);
    put_pixel(a - x, b - y);
    put_pixel(a + x, b - y);

    y = yd - a;
    x = xd - b;

    //pdbg("(%d, %d)\t(%d, %d)\t(%d, %d)\t(%d, %d)\n", y, x, -y, x, -y, -x, y, -x);
    put_pixel(a + y, b + x);
    put_pixel(a - y, b + x);
    put_pixel(a - y, b - x);
    put_pixel(a + y, b - x);
}

void draw_circle_bresenham(int a, int b, int r){
    int x = a;
    int y = b + r;
    circle_8_points(a, b, x, y);
    int p = 3 - 2*r;
    while(y > x){
        if(p < 0)
            p = p + 4*x + 6;
        else{
            p = p + 4*(x - y) + 10;
            y--;
        }
        x++;
        circle_8_points(a, b, x, y);
    }
}

static void circle_n_points(int a, int b, int x, int y, int points){
    // Find distance of x, y from the centre
    double nx = x - a;
    double ny = y - b;

    //pdbg("\n(%g, %g)", nx, ny);
    put_pixel(a + nx, b + ny);

    double delta = 360 / points;

    for(double theta = delta; theta < 360; theta += delta){
        double thetar = (-theta) * M_PI / 180;
        
        // The axis with centre at (a, b) rotated by theta,
        // hence new points :
        double xd = nx*cos(thetar) + ny * sin(thetar);
        double yd = - nx*sin(thetar) + ny * cos(thetar);

        // Reflected points
        double rx = - xd;
        double ry = yd;

        // Now transform back the reflected points
        // to the actual axis with centre at (a, b)
        // by -theta rotation
        double finx = rx*cos(-thetar) + ry*sin(-thetar);
        double finy = - rx*sin(-thetar) + ry*cos(-thetar);

        // Finally, draw the final points
        put_pixel(a + round(finx), b + round(finy));

        //pdbg("(%f, %f)", finx, finy);
        nx = finx;
        ny = finy;
    }
}

void draw_circle_bresenham_n_point(int a, int b, int r, int points){
    double x = a;
    double y = b + r;

    circle_n_points(a, b, x, y, points);
    
    double p = 3 - 2*r;

    double theta = (90 - (360/points)) * (M_PI/180);
    double expectedSlope = tan(theta);

    do{
        if(p < 0)
            p = p + 4*x + 6;
        else{
            p = p + 4*(x - y) + 10;
            y--;
        }
        x++;
        circle_n_points(a, b, x, y, points);
    } while((y - b)/(x - a) > expectedSlope);
}
