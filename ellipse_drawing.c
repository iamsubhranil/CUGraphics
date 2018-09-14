#include "driver.h"
#include "display.h"

// Plotting points in 4 point symmetry
static void ellipse_points(int a, int b, int x, int y){
    //pdbg("q1 : %d %d", a + x, b + y);
    // pdbg("q2 : %d %d", a - x, b + y);
    // pdbg("q3 : %d %d", a - x, b - y);
    // pdbg("q4 : %d %d\n", a + x, b - y);
    put_pixel(a + x, b + y);
    put_pixel(a - x, b + y);
    put_pixel(a + x, b - y);
    put_pixel(a - x, b - y);
}

#define sqr(x)  ((x) * (x))

// c,d are the centre
void draw_ellipse_midpoint(int c, int d, int a, int b){
    double x = 0;
    double y = b;
    ellipse_points(c, d, x, y);
    double p = sqr(b) + sqr(a)*((double)a - 0.5) - sqr(a*b);
    int terminator = 0;
    do{
        x++;
        if(p < 0)
            p = p + sqr(b)*(2*x + 3);
        else{
            y--;
            p = p + sqr(b)*(2*x + 3) + sqr(a)*(-2*y + 2);
        }
        ellipse_points(c, d, x, y);
        
        terminator = sqr(b * (x + 1)) < sqr(a * (y - 0.5));
    } while(terminator);

    while(y > 0){
        y--;
        if(p < 0)
            p = p + sqr(a)*(-2*y + 2);
        else{
            x++;
            p = p + sqr(b)*(2*x + 2) + sqr(a)*(-2*y + 3);
        }
        ellipse_points(c, d, x, y);
    }
}
