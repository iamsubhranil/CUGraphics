#include "clipping.h"
#include "display.h"
#include "driver.h"
#include "line_drawing.h"

static const char *bottom_left  = "\u2517\u2501";
static const char *bottom_right = "\u251b";
static const char *top_left     = "\u250f\u2501";
static const char *top_right    = "\u2513";
static const char *vertical     = "\u2503";
static const char *horizontal   = "\u2501\u2501";

static int get_region_code(int x, int y, int xmin, int ymin, 
        int xmax, int ymax){
    int code = x < xmin;
    code |= ((x > xmax) << 1);
    code |= ((y < ymin) << 2);
    code |= ((y > ymax) << 3);
    return code;
}

static void draw_rect(int bx, int by, int tx, int ty){
    set_pixel(bx, by, bottom_left);
    set_pixel(bx, ty, top_left);
    set_pixel(tx, by, bottom_right);
    set_pixel(tx, ty, top_right);
    int bak = bx;
    while(bx < tx){
        set_pixel(bx, ty, horizontal);
        set_pixel(bx, by, horizontal);
        bx++;
    }
    bx = bak;
    bak = by;
    while(by < ty){
        set_pixel(bx, by, vertical);
        set_pixel(tx, by, vertical);
        by++;
    }
}

static int prepare_clip(int sx, int sy, int ex, int ey,
        int bx, int by, int tx, int ty){
    draw_line_bresenham(sx, sy, ex, ey);
    draw_rect(bx, by, tx, ty);
    int rcs = get_region_code(sx, sy, bx, by, tx, ty);
    int rce = get_region_code(ex, ey, bx, by, tx, ty);
#ifdef NO_DRAW
    pdbg("rcs : %d\trce : %d", rcs, rce);
#endif
    if(rcs == 0 && rce == 0){
        show_msg("The line is completely inside the window! No need to clip!");
    }
    else{
        int res = rcs & rce;
        if(res != 0){
            show_msg("The line is completely outside the window! No need to clip!");
        }
        else{
            show_msg("Press any key to clip!");
            wait_for_input();
            return 1;
        }
    }
    return 0;
}

static int in_range(int l, int key, int u){
    return (l < key) && (key < u);
}

#define ROUND(x)    ((int)((x) + 0.5))

void clipping_cohen_sutherland(int x1, int y1, int x2, int y2,
        int xmin, int ymin, int xmax, int ymax){
    if(prepare_clip(x1, y1, x2, y2, xmin, ymin, xmax, ymax)){
        double m = (double)(y2 - y1)/(x2 - x1);
#ifdef NO_DRAW
        pdbg("xmin : %d\tymin : %d\txmax : %d\tymax : %d", xmin, ymin, xmax, ymax);
#endif
        int sx = x1, sy = y1, ex = x2, ey = y2;
#ifdef NO_DRAW
        pdbg("Before\nsx : %d\tsy : %d\tex : %d\tey : %d", sx, sy, ex, ey);
#endif
        int proposed_points[2][2] = {{0}};
        int ppointer = 0;
        proposed_points[0][0] = x1;
        proposed_points[0][1] = y1;
        proposed_points[1][0] = x2;
        proposed_points[1][1] = y2;
        if(get_region_code(sx, sy, xmin, ymin, xmax, ymax) != 0){
            int ymind = ROUND(m*(xmin - x1) + y1);
            int xmind = ROUND((ymin - y1)/m + x1);
#ifdef NO_DRAW
            pdbg("xmind : %d\tymind : %d", xmind, ymind);
#endif
            if(ymind != sy && in_range(ymin, ymind, ymax)){
                sy = ymind;
                proposed_points[0][0] = xmin;
                proposed_points[0][1] = sy;
                ppointer++;
            }
            if(xmind != sx && in_range(xmin, xmind, xmax)){
                sx = xmind;
                proposed_points[ppointer][0] = sx;
                proposed_points[ppointer][1] = ymin;
                ppointer++;
            }
        }
        if(get_region_code(ex, ey, xmin, ymin, xmax, ymax) != 0){
            int ymaxd = ROUND(m*(xmax - x1) + y1);
            int xmaxd = ROUND((ymax - y1)/m + x1);
#ifdef NO_DRAW
            pdbg("xmaxd : %d\tymaxd : %d", xmaxd, ymaxd);
#endif
            if(ymaxd != ey && in_range(ymin, ymaxd, ymax)){
                ey = ymaxd;
                proposed_points[ppointer][0] = xmax;
                proposed_points[ppointer][1] = ey;
                ppointer++;
            }
            if(xmaxd != ex && in_range(xmin, xmaxd, xmax)){
                ex = xmaxd;
                proposed_points[ppointer][0] = ex;
                proposed_points[ppointer][1] = ymax;
            }
        }
        sx = proposed_points[0][0];
        sy = proposed_points[0][1];
        ex = proposed_points[1][0];
        ey = proposed_points[1][1];
#ifdef NO_DRAW
        pdbg("After\nsx : %d\tsy : %d\tex : %d\tey : %d", sx, sy, ex, ey);
#endif
        screen_clear();
        draw_line_bresenham(sx, sy, ex, ey);
        draw_rect(xmin, ymin, xmax, ymax);
        show_msg("\t\t\t\t\t\t\t\t\t\rClipped!");
    }
}

static void clipping_midpoint_subdivision_impl(int x1, int y1, int x2, int y2,
        int xmin, int ymin, int xmax, int ymax){
    int r1 = get_region_code(x1, y1, xmin, ymin, xmax, ymax);
    int r2 = get_region_code(x2, y2, xmin, ymin, xmax, ymax);
    if(r1 == 0 && r2 == 0){
        draw_line_bresenham(x1, y1, x2, y2);
    }
    else if((r1 & r2) == 0){
        int m1 = (x1 + x2)/2;
        int n1 = (y1 + y2)/2;
        // Check if the resulting point is equal to any of the given points
        // This happens when two points are at 1 unit distance
        if((m1 == x1 && n1 == y1) || (m1 == x2 && n1 == y2)){
            return;
        }
        clipping_midpoint_subdivision_impl(x1, y1, m1, n1, xmin, ymin, xmax, ymax);
        clipping_midpoint_subdivision_impl(m1, n1, x2, y2, xmin, ymin, xmax, ymax);
    }
}

void clipping_midpoint_subdivision(int x1, int y1, int x2, int y2,
        int xmin, int ymin, int xmax, int ymax){
    if(prepare_clip(x1, y1, x2, y2, xmin, ymin, xmax, ymax)){
        screen_clear();
        draw_rect(xmin, ymin, xmax, ymax);
        clipping_midpoint_subdivision_impl(x1, y1, x2, y2, xmin, ymin, xmax, ymax);
        show_msg("\t\t\t\t\t\t\t");
        show_msg("Clipped!");
    }
}
