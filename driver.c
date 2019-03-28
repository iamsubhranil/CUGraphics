
#include <locale.h>
#include <math.h>
#include <memory.h>
#include <ncurses.h>
#ifdef NO_DRAW
#include <termios.h>
#endif

#include "common.h"
#include "display.h"
#include "driver.h"
#include "matrix.h"

#ifndef NO_DRAW
static const char *pixel_fill = "\u25a0";
#endif
static int  pivot_x = -1, pivot_y = -1;
static u8   *pixels       = NULL;
static int  do_transform = 1;

#define mod_y(y) (LINES - y - 1)
#define orig_y(y) (LINES - y - 1)
#define mod_x(x) ((x * 2) + 1)
#define orig_x(x) ((x - 1) / 2)
#define pxy(x, y)   (((x) * COLS) + (y))

void init_driver() {
	setlocale(LC_ALL, "");
#ifndef NO_DRAW
	initscr();
	refresh();
#else
	pdbg("Intialized screen");
	LINES = 200, COLS = 200;
#endif
	pixels = (u8 *)malloc(sizeof(u8 *) * LINES * COLS);
#ifndef NO_DRAW
	clear();
#else
	pdbg("Cleared screen");
#endif
}

int get_rows() {
	return LINES;
}

int get_columns() {
	return COLS;
}

void enable_transform(int t) {
	do_transform = t;
}

void draw_graph() {
#ifndef NO_DRAW
	for(int i = 0; i < LINES - 1; i++) {
		mvprintw(i, 0, "%2d", LINES - (i + 1));
		// for(int j = 2;j < COLS - 1;j += 2){
		//    mvprintw(i, j, "|_");
		//}
	}
	for(int j = 0; j < COLS - 1; j += 2) {
		mvprintw(LINES - 1, j, "%2d", j / 2);
	}
#else
	pdbg("Graph drawn");
#endif
}

void set_pixel(int x, int y, const char *fill) {
	if(mod_x(x) > COLS - 1 || mod_y(y) < 0)
		return;
	pixels[pxy(mod_y(y),mod_x(x))] = 1;
#ifndef NO_DRAW
	mvaddstr(mod_y(y), mod_x(x), fill);
	refresh();
#else
	pdbg("Pixel drawn : (%d, %d) as (%d, %d)", x, y, mod_x(x), mod_y(y));
#endif
}

void put_pixel(int x, int y) {
#ifndef NO_DRAW
	set_pixel(x, y, pixel_fill);
#else
	set_pixel(x, y, "");
#endif
}

void set_pivot(int x, int y) {
	pivot_x = mod_x(x);
	pivot_y = mod_y(y);
}

static void redraw() {
#ifndef NO_DRAW
	clear();
	for(int i = 0; i < LINES; i++) {
		for(int j = 0; j < COLS; j++) {
			if(pixels[pxy(i, j)])
				mvaddstr(i, j, pixel_fill);
		}
	}
	refresh();
#else
	pdbg("Screen redrawn");
#endif
}

void screen_clear() {
#ifndef NO_DRAW
	clear();
	for(int i = 0; i < LINES; i++) {
		for(int j = 0; j < COLS; j++) pixels[pxy(i, j)] = 0;
	}
	refresh();
#else
	pdbg("Screen cleared");
#endif
}

static void transform_mat(Matrix m, u8 use_pivot) {
#ifdef NO_DRAW
	pdbg("Transformation matrix : ");
	mat_print(m);
#endif
	u8 new_pixels[LINES * COLS];
	for(int i = 0; i < LINES; i++) {
		for(int j = 0; j < COLS; j++) new_pixels[pxy(i, j)] = 0;
	}
	Matrix point = mat_new(3, 1);
	Matrix pivot = mat_new(3, 1);
	mat_fill(pivot, pivot_x * 1.0, pivot_y * 1.0, 0.0);
#ifdef NO_DRAW
	pdbg("Pivot (F) : ");
	mat_print(pivot);
#endif
	for(int i = 0; i < LINES; i++) {
		for(int j = 0; j < COLS; j++) {
			if(pixels[pxy(i, j)]) {
				mat_fill(point, j * 1.0, i * 1.0, 1.0);
#ifdef NO_DRAW
				pdbg("Point (P) : ");
				mat_print(point);
#endif
				Matrix np;
				if(use_pivot) {
					Matrix refocus = mat_sub(point, pivot);
#ifdef NO_DRAW
					pdbg("Translated point (P - F) : ");
					mat_print(refocus);
#endif
					Matrix res1 = mat_mult(m, refocus);
#ifdef NO_DRAW
					pdbg("Transformed point (T) : ");
					mat_print(res1);
#endif
					np = mat_add(res1, pivot);
#ifdef NO_DRAW
					pdbg("Retranslated point (T + F) : ");
					mat_print(np);
#endif
					mat_free(refocus);
					mat_free(res1);
				} else {
					Matrix newpivot = mat_mult(m, pivot);
					pivot_y         = mat_get(newpivot, 1, 0);
					pivot_x         = mat_get(newpivot, 0, 0);
#ifdef NO_DRAW
					pdbg("Transformed pivot (F) : ");
					mat_print(newpivot);
#endif
					mat_free(newpivot);
					np = mat_mult(m, point);
#ifdef NO_DRAW
					pdbg("Transformed point (T) : ");
					mat_print(np);
#endif
				}
				int px = (int)(floor(mat_get(np, 0, 0))),
				    py = (int)(floor(mat_get(np, 1, 0)));
#ifdef NO_DRAW
				pdbg("(px, py) : (%d, %d)", px, py);
#endif
				if((py < LINES - 1 && py > 0) && (px < COLS - 1 && px > 0))
					new_pixels[pxy(py, px)] = 1;
				mat_free(np);
			}
		}
	}
	mat_free(point);
	mat_free(pivot);
	for(int i = 0; i < LINES; i++) {
		for(int j = 0; j < COLS; j++) {
			pixels[pxy(i, j)] = new_pixels[pxy(i, j)];
		}
	}
	redraw();
}

static void make_mat_trans(Matrix mat, double tx, double ty) {
	mat_fill(mat, 1.0, 0.0, tx, 0.0, 1.0, ty, 0.0, 0.0, 1.0);
}

static void make_mat_scale(Matrix mat, double sx, double sy) {
	mat_fill(mat, sx, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, 1.0);
}

#ifdef ENABLE_ROTATION
static void make_mat_rot(Matrix mat, double deg) {
	deg = (M_PI / 180) * deg;
	mat_fill(mat, cos(deg), -sin(deg), 0.0, sin(deg), cos(deg), 0.0, 0.0, 0.0,
	         1.0);
}
#endif

void show_msg(const char *msg) {
#ifndef NO_DRAW
	mvaddstr(0, 0, msg);
#else
	pinfo("%s", msg);
#endif
}

static u8 keypad_init_done = 0;

static void keypad_init() {
	if(keypad_init_done)
		return;
#ifndef NO_DRAW
	keypad(stdscr, TRUE);
	noecho();
#else
	struct termios t;
	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSANOW, &t);
	pdbg("Intialized keypad and set noecho");
#endif
	keypad_init_done = 1;
}

static void keypad_restore() {
#ifdef NO_DRAW
	struct termios t;
	tcgetattr(0, &t);
	t.c_lflag &= ECHO | ICANON;
	tcsetattr(0, TCSANOW, &t);
	pdbg("Keypad state restored!");
#else
	keypad(stdscr, FALSE);
	echo();
#endif
	keypad_init_done = 0;
}

int wait_for_input() {
	keypad_init();
#ifdef NO_DRAW
	return getchar();
#else
	return getch();
#endif
}

void transform() {
	keypad_init();
#ifndef NO_DRAW
#define KB_UP KEY_UP
#define KB_DOWN KEY_DOWN
#define KB_RIGHT KEY_RIGHT
#define KB_LEFT KEY_LEFT
#else
#define KB_ESC 27
#define KB_UP 65
#define KB_DOWN 66
#define KB_RIGHT 67
#define KB_LEFT 68
#undef getch
#define getch getchar
	u8 esceen = 0;
#endif
	int    c;
	Matrix tm = mat_new(3, 3);
	while((c = getch()) != 'q' && c != 'Q') {
		if(!do_transform) {
#ifdef NO_DRAW
			pdbg("Transformation disabled!");
#endif
			continue;
		}
		// echo();
#ifdef NO_DRAW
		pdbg("KeyPressed : %d", c);
		if(c == KB_ESC && (c = getch()) == '[') {
			esceen = 1;
			c      = getch();
		} else
			esceen = 0;
#endif
		switch(c) {
			case KB_LEFT:
#ifdef NO_DRAW
#ifdef ENABLE_ROTATION
				if(!esceen) { // 'A' and left has same key codes
					make_mat_rot(tm, 1.0);
					transform_mat(tm, 1);
					show_msg("rotate 1deg anticlockwise");
					break;
				}
#endif
#endif
				make_mat_trans(tm, -1, 0);
				transform_mat(tm, 0);
				show_msg("move left");
#ifdef NO_DRAW
				esceen = 0;
#endif
				break;
			case KB_RIGHT:
				make_mat_trans(tm, 1, 0);
				transform_mat(tm, 0);
				show_msg("move right");
				break;
			case KB_UP:
				make_mat_trans(tm, 0, -1);
				transform_mat(tm, 0);
				show_msg("move up");
				break;
			case KB_DOWN:
				make_mat_trans(tm, 0, 1);
				transform_mat(tm, 0);
				show_msg("move down");
				break;
			case 'z':
			case 'Z':
				make_mat_scale(tm, 1.5, 1.5);
				transform_mat(tm, 1);
				show_msg("zoom in");
				break;
			case 'x':
			case 'X':
				make_mat_scale(tm, .67, .67);
				transform_mat(tm, 1);
				show_msg("zoom out");
				break;
#ifdef ENABLE_ROTATION
			case 'a':
#ifndef NO_DRAW
			case 'A':
#endif
				make_mat_rot(tm, 1.0);
				transform_mat(tm, 1);
				show_msg("rotate 1deg anticlockwise");
				break;
			case 's':
			case 'S':
				make_mat_rot(tm, -1.0);
				transform_mat(tm, 1);
				show_msg("rotate 1deg clockwise");
				break;
#endif
		}
	}
	keypad_restore();
	mat_free(tm);
}

void terminate_driver() {
	free(pixels);
#ifndef NO_DRAW
	endwin();
#else
	pdbg("Window terminated!\n");
#endif
}
