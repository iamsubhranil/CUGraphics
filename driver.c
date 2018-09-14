#include <ncurses.h>
#include <locale.h>

#include "common.h"

static const char *pixel_fill = "\u25a0";
static const char *pixel_empty = "\u25cb";

void init_driver(){
    setlocale(LC_ALL, "");

    initscr();
    refresh();
    clear();
}

void draw_graph(){
    for(int i = 0;i < LINES - 1;i++){
        mvprintw(i, 0, "%2d", LINES - (i + 1));
        //for(int j = 2;j < COLS - 1;j += 2){
        //    mvprintw(i, j, "|_");
        //}
    }
    for(int j = 0; j < COLS - 1;j += 2){
        mvprintw(LINES - 1, j, "%2d", j/2);
    }
}

void put_pixel(int x, int y){
    mvaddstr(LINES - y - 1, x*2 + 1, pixel_fill);
    refresh();
}

void terminate_driver(){
    endwin();
}
