#include <ncurses.h>
#include <string.h>

#include "cargparser.h"
#include "circle_drawing.h"
#include "display.h"
#include "driver.h"
#include "line_drawing.h"

static void usage(const char *name){
    printf("\n");
    pinfo("Usage : %s <args>\n"
            "Arguments for line drawing : \n"
            "\t[-o|--object]    : line\n"
            "\t[-a|--algo]      : [dda|bresenham|midpoint]\n"
            "\t[-x|--startx]    : Starting x coordinate of the line <int>\n"
            "\t[-y|--starty]    : Starting y coordinate of the line <int>\n"
            "\t[-p|--endx]      : Ending x coordinate of the line   <int>\n"
            "\t[-q|--endy]      : Ending y coordinate of the line   <int>\n\n"
            "Arguments for circle drawing : \n"
            "\t[-o|--object]    : circle\n"
            "\t[-a|--algo]      : bresenham\n"
            "\t[-x|--startx]    : x coordinate of the centre        <int>\n"
            "\t[-y|--starty]    : y coordinate of the centre        <int>\n"
            "\t[-r|--radius]    : radius of the circle              <int>\n"
            "\t[-s|--symmetry]  : point symmetry of the circle      <int> [optional, if not specified, uses 8 by default]\n", name);
}

static void get_int(char s, int *slot, const char *name, ArgumentList list, char *argv0){
    if(arg_is_present(list, s)){
        char *end = NULL, *str = arg_value(list, s);
        *slot = strtol(str, &end, 10);
        if(*end != 0){
            perr("Bad integer '%s'", str);
            arg_free(list);
            usage(argv0);
            exit(1);
        }
    }
    else{
        perr("Expected argument %s", name);
        arg_free(list);
        usage(argv0);
        exit(1);
    }
}

static void draw_line(ArgumentList list, char **argv){

    int algo = 0, x = 0, y = 0, p = 0, q = 0;
    
    if(arg_is_present(list, 'a')){
        char *al = arg_value(list, 'a');
        if(strcmp(al, "dda") == 0)
            algo = 1;
        else if(strcmp(al, "bresenham") == 0)
            algo = 2;
        else{
            perr("--algo must be one of 'dda' or 'bresenham'!");
            arg_free(list);
            usage(argv[0]);
            exit(1);
        }
    }
    else{
        perr("No algorithm specified!");
        arg_free(list);
        usage(argv[0]);
        exit(1);
    }

    get_int('x', &x, "starting x", list, argv[0]);
    get_int('y', &y, "starting y", list, argv[0]);
    get_int('p', &p, "ending x", list, argv[0]);
    get_int('q', &q, "ending y", list, argv[0]);

    init_driver();
    if(arg_is_present(list, 'g'))
        draw_graph();
    if(algo == 1)
        dda_draw_line(x, y, p, q);
    else
        bresenham_draw_line(x, y, p, q);
}

static void draw_circle(ArgumentList list, char **argv){
    int algo = 0, x = 0, y = 0, r = 0, s = 0;

    if(arg_is_present(list, 'a')){
        char *al = arg_value(list, 'a');
        if(strcmp(al, "bresenham") == 0)
            algo = 1;
        else{
            perr("--algo must be one of 'bresenham' or 'midpoint'!");
            arg_free(list);
            exit(1);
        }
    }
    else{
        perr("No algorithm specified!");
        arg_free(list);
        usage(argv[0]);
        exit(1);
    }

    get_int('x', &x, "centre x", list, argv[0]);
    get_int('y', &y, "centre y", list, argv[0]);
    get_int('r', &r, "radius", list, argv[0]);

    if(arg_is_present(list, 's')){
        get_int('s', &s, "symmetry", list, argv[0]);
        int j = 2;
        while((j << 1) <= s)
            j <<= 1;
        if(j != s || s < 4 || s > 256){
            perr("Symmetry must be a power of 2 (4 <= symmetry <= 256) (Given : %d)\n", s);
            arg_free(list);
            exit(2);
        }
    }

    init_driver();
    if(algo == 1){
        if(s > 0){
            bresenham_draw_circle_n_point(x, y, r, s);
        }
        else
            bresenham_draw_circle(x, y, r);
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        usage(argv[0]);
        return 0;
    }

    ArgumentList list = arg_list_create(9);
   
    arg_add(list, 'o', "object", true);
    arg_add(list, 'a', "algo", true);
    arg_add(list, 'r', "radius", true);
    arg_add(list, 'x', "startx", true);
    arg_add(list, 'y', "starty", true);
    arg_add(list, 'p', "endx", true);
    arg_add(list, 'q', "endy", true);
    arg_add(list, 'g', "showgraph", false);
    arg_add(list, 's', "symmetry", true);

    arg_parse(argc, &argv[0], list);
    
    if(arg_is_present(list, 'o')){
        char *obj = arg_value(list, 'o');
        if(strcmp(obj, "line") == 0)
            draw_line(list, argv);
        else if(strcmp(obj, "circle") == 0)
            draw_circle(list, argv);
        else{
            perr("--object must be one of 'circle' or 'line'!");
            arg_free(list);
            usage(argv[0]);
            return 1;
        }
    }
    else{
        perr("Specify object to draw!");
        usage(argv[0]);
        arg_free(list);
        return 1;
    }

    noecho();
    char c;
    while((c = getch()) != 'q' && c != 'Q');
    terminate_driver();
    arg_free(list);
    return 0;
}
