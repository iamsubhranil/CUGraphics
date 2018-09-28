#pragma once

void draw_graph();
void enable_transform(int en);
void init_driver();
void put_pixel(int x, int y);
void screen_clear();
void set_pivot(int x, int y);
void set_pixel(int x, int y, const char *pixel);
void show_msg(const char *msg);
void terminate_driver();
void transform();
int  wait_for_input();
