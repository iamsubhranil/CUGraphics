#include <ncurses.h>
#include <string.h>

#include "driver.h"
#include "line_drawing.h"
#include "cargparser.h"
#include "display.h"

static void usage(const char *name){
    pinfo("Usage : %s [-a=|--algo] [dda|bresenham] [-x=|--startx] <int> [-y=|--starty] <int>"
            "[-p=|--endx] <int> [-q=|--endy] <int>\n", name);
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

int main(int argc, char *argv[]){
    if(argc < 2){
        usage(argv[0]);
        return 0;
    }

    ArgumentList list = arg_list_create(6);
    
    arg_add(list, 'a', "algo", true);
    arg_add(list, 'x', "startx", true);
    arg_add(list, 'y', "starty", true);
    arg_add(list, 'p', "endx", true);
    arg_add(list, 'q', "endy", true);
    arg_add(list, 'g', "showgraph", false);

    arg_parse(argc, &argv[0], list);

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
            return 0;
        }
    }
    else{
        perr("No algorithm specified!");
        arg_free(list);
        usage(argv[0]);
        return 0;
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
    getch();
    terminate_driver();
    arg_free(list);
    return 0;
}
