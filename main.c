#include <ncurses.h>
#include <stdarg.h>
#include <string.h>

#include "cargparser.h"
#include "circle_drawing.h"
#include "display.h"
#include "driver.h"
#include "ellipse_drawing.h"
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
            "\t[-s|--symmetry]  : point symmetry of the circle      <int> [optional, if not specified, uses 8 by default]\n\n"
            "Arguments for ellipse drawing : \n"
            "\t[-o|--object]    : ellipse\n"
            "\t[-x|--startx]    : x coordinate of the centre        <int>\n"
            "\t[-y|--starty]    : y coordinate of the centre        <int>\n"
            "\t[-m|--major]     : length of the major axis          <int>\n"
            "\t[-n|--minor]     : length of the minor axis          <int>\n", name);
}

static int expect_oneof(char s, ArgumentList list, const char *err, const char *argv0, int count, const char **args){
    if(arg_is_present(list, s)){
        char *val = arg_value(list, s);
        for(int i = 0;i < count;i++){
            if(strcmp(args[i], val) == 0)
                return i + 1;
        }
        perr("Wrong value for argument -%c", s);
        perr("Expected one of : %s", args[0]);
        for(int i = 1;i < count;i++)
            printf(", %s", args[i]);
        printf("\n");
        exit(1);
    }
    else{
        perr("%s!", err);
        arg_free(list);
        usage(argv0);
        exit(1);
    }
}

static void get_int_impl(char s, int *slot, const char *name, ArgumentList list, char *argv0, bool isoptional, int defaultValue){
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
    else if(isoptional){
        *slot = defaultValue;
    }
    else{
        perr("Expected argument %s", name);
        arg_free(list);
        usage(argv0);
        exit(1);
    }
}

static void get_int(char s, int *slot, const char *name, ArgumentList list, char *argv0){
    return get_int_impl(s, slot, name, list, argv0, false, 0);
}

static void get_int_optional(char s, int *slot, const char *name, ArgumentList list, char *argv0, int defaultValue){
    return get_int_impl(s, slot, name, list, argv0, true, defaultValue);
}

static void draw_line(ArgumentList list, char **argv){

    int algo = 0, x = 0, y = 0, p = 0, q = 0;
    
    const char *algos[] = {"dda", "bresenham", "midpoint"};

    algo = expect_oneof('a', list, "Specify the algorithm to use", argv[0], 3, &algos[0]);

    get_int('x', &x, "starting x", list, argv[0]);
    get_int('y', &y, "starting y", list, argv[0]);
    get_int('p', &p, "ending x", list, argv[0]);
    get_int('q', &q, "ending y", list, argv[0]);

    init_driver();
    if(arg_is_present(list, 'g'))
        draw_graph();
    
    switch(algo){
        case 1:
            draw_line_dda(x, y, p, q);
            break;
        case 2:
            draw_line_bresenham(x, y, p, q);
            break;
        case 3:
            draw_line_midpoint(x, y, p, q);
            break;
    }
}

static void draw_circle(ArgumentList list, char **argv){
    int algo = 0, x = 0, y = 0, r = 0, s = 0;
    
    const char *algos[] = {"bresenham", "midpoint"};

    algo = expect_oneof('a', list, "Specify the algorithm to use", argv[0], 2, &algos[0]);

    get_int('x', &x, "centre x", list, argv[0]);
    get_int('y', &y, "centre y", list, argv[0]);
    get_int('r', &r, "radius", list, argv[0]);
    get_int_optional('s', &s, "symmetry", list, argv[0], 0);
    
    if(s != 0){
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
    switch(algo){
        case 1:
            if(s > 0){
                draw_circle_bresenham_n_point(x, y, r, s);
            }
            else
                draw_circle_bresenham(x, y, r);
            break;
        case 2:
            draw_circle_midpoint(x, y, r, s == 0 ? 4 : s);
            break;
    }
}

static void draw_ellipse(ArgumentList list, char **argv){
    int x = 0, y = 0, a = 0, b = 0; 
    get_int('x', &x, "centre x", list, argv[0]);
    get_int('y', &y, "centre y", list, argv[0]);
    get_int('m', &a, "major axis length", list, argv[0]);
    get_int('n', &b, "minor axis length", list, argv[0]);
    
    init_driver();
    draw_ellipse_midpoint(x, y, a, b);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        usage(argv[0]);
        return 0;
    }

    ArgumentList list = arg_list_create(11);
   
    arg_add(list, 'o', "object", true);
    arg_add(list, 'a', "algo", true);
    arg_add(list, 'r', "radius", true);
    arg_add(list, 'x', "startx", true);
    arg_add(list, 'y', "starty", true);
    arg_add(list, 'p', "endx", true);
    arg_add(list, 'q', "endy", true);
    arg_add(list, 'g', "showgraph", false);
    arg_add(list, 's', "symmetry", true);
    arg_add(list, 'm', "major", true);
    arg_add(list, 'n', "minor", true);

    arg_parse(argc, &argv[0], list);
   
    const char *objects[] = {"line", "circle", "ellipse"};

    int choice = expect_oneof('o', list, "Specify object to draw", argv[0], 3, &objects[0]);
    
    switch(choice){
        case 1:
            draw_line(list, &argv[0]);
            break;
        case 2:
            draw_circle(list, &argv[0]);
            break;
        case 3:
            draw_ellipse(list, &argv[0]);
            break;
    }

    noecho();
    char c;
    while((c = getch()) != 'q' && c != 'Q');
    terminate_driver();
    arg_free(list);
    return 0;
}
