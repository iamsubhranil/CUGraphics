#include <ncurses.h>
#include <stdarg.h>
#include <string.h>

#include "bench.h"
#include "cargparser.h"
#include "circle_drawing.h"
#include "clipping.h"
#include "display.h"
#include "driver.h"
#include "ellipse_drawing.h"
#include "line_drawing.h"

static void usage(const char *name) {
	pinfo("Usage : %s <args>\n\n"
	      "Arguments for line drawing : \n"
	      "\t[-o|--object]    : line\n"
	      "\t[-a|--algo]      : [dda|bresenham|midpoint]\n"
	      "\t[-x|--start]     : Coordinates of first endpoint     <int,int>\n"
	      "\t[-y|--end]       : Coordinates of second endpoint    <int,int>\n\n"
	      "Arguments for circle drawing : \n"
	      "\t[-o|--object]    : circle\n"
	      "\t[-a|--algo]      : [bresenham|midpoint]\n"
	      "\t[-x|--start]     : Coordinates of the centre         <int,int>\n"
	      "\t[-r|--radius]    : Radius of the circle              <int>\n"
	      "\t[-s|--symmetry]  : point symmetry of the circle      <int> "
	      "[optional, uses 8 by default]\n\n"
	      "Arguments for ellipse drawing : \n"
	      "\t[-o|--object]    : ellipse\n"
	      "\t[-x|--start]     : Coordinates of the centre         <int,int>\n"
	      "\t[-m|--major]     : Length of the major axis          <int>\n"
	      "\t[-n|--minor]     : Length of the minor axis          <int>\n\n"
	      "Arguments for line clipping : \n"
	      "\t[-o|--object]    : clip\n"
	      "\t[-a|--algo]      : [cohen|midpoint]\n"
	      "\t[-x|--start]     : First endpoint of the line        <int,int>\n"
	      "\t[-y|--end]       : Second endpoint of the line       <int,int>\n"
	      "\t[-b|--bottom]    : Bottom left point of the window   <int,int>\n"
	      "\t[-t|--top]       : Top right point of the window     <int,int>\n\n"
	      "To specify a coordinate, write it in the following format : \n"
	      "\t<abscissa>,<ordinate>\n"
	      "Don't add any spaces in between the comma and the numbers.\n\n"
	      "Arguments for benchmarking (ignores all other arguments) : \n"
	      "\t[-c|--bench]     : [create|fill|add|sub|draw|all]\n"
	      "\tThe options perform the following benchmarks respectively :\n"
	      "\t create          : 3x3 matrix creation\n"
	      "\t fill            : 3x3 matrix fill\n"
	      "\t add             : 3x3 matrix addition\n"
	      "\t sub             : 3x3 matrix subtraction\n"
	      "\t draw            : put_pixel calls to the driver\n"
	      "\t all             : all of the above\n",
	      name);
}

static int expect_oneof(char s, ArgumentList list, const char *err,
                        const char *argv0, int count, const char **args) {
	if(arg_is_present(list, s)) {
		char *val = arg_value(list, s);
		for(int i = 0; i < count; i++) {
			if(strcmp(args[i], val) == 0)
				return i + 1;
		}
		perr("Wrong value for argument '-%c'!", s);
		perr("Expected one of : %s", args[0]);
		for(int i = 1; i < count; i++) printf(", %s", args[i]);
		printf("\n");
		exit(1);
	} else {
		perr("%s!", err);
		arg_free(list);
		usage(argv0);
		exit(1);
	}
}

static int parse_int(const char *str, ArgumentList list, const char *argv0) {
	char *end   = NULL;
	int   value = strtol(str, &end, 10);
	if(*end != 0) {
		perr("Bad integer '%s'!", str);
		arg_free(list);
		usage(argv0);
		exit(1);
	}
	return value;
}

static void get_int_impl(char s, int *slot, const char *name, ArgumentList list,
                         char *argv0, bool isoptional, int defaultValue) {
	if(arg_is_present(list, s)) {
		*slot = parse_int(arg_value(list, s), list, argv0);
	} else if(isoptional) {
		*slot = defaultValue;
	} else {
		perr("Expected argument '-%c' (%s)!", s, name);
		arg_free(list);
		usage(argv0);
		exit(1);
	}
}

static void get_int(char s, int *slot, const char *name, ArgumentList list,
                    char *argv0) {
	return get_int_impl(s, slot, name, list, argv0, false, 0);
}

static void get_int_optional(char s, int *slot, const char *name,
                             ArgumentList list, char *argv0, int defaultValue) {
	return get_int_impl(s, slot, name, list, argv0, true, defaultValue);
}

static void get_point(char p, const char *name, int *slotx, int *sloty,
                      ArgumentList list, char *argv0) {
	if(arg_is_present(list, p)) {
		char * str = arg_value(list, p);
		size_t i   = 0;
		while(str[i] != '\0') {
			if(str[i] == ',')
				break;
			i++;
		}
		if(i == 0 || str[i] == '\0' || str[i + 1] == '\0') {
			perr("Bad point : '%s'!", str);
			perr("Correct format : x,y");
			goto endpoint;
		}
		str[i] = '\0';
		*slotx = parse_int(str, list, argv0);
		*sloty = parse_int(&str[i + 1], list, argv0);
	} else {
		perr("Expected argument '-%c' (coordinates of %s)!", p, name);
	endpoint:
		arg_free(list);
		usage(argv0);
		exit(1);
	}
}

static void draw_line(ArgumentList list, char **argv) {

	int algo = 0, x = 0, y = 0, p = 0, q = 0;

	const char *algos[] = {"dda", "bresenham", "midpoint"};

	algo = expect_oneof('a', list, "Specify the algorithm to use", argv[0], 3,
	                    &algos[0]);

	get_point('x', "starting point", &x, &y, list, argv[0]);
	get_point('y', "ending point", &p, &q, list, argv[0]);

	init_driver();
	set_pivot((x + p) / 2, (y + q) / 2);
	if(arg_is_present(list, 'g'))
		draw_graph();

	switch(algo) {
		case 1: draw_line_dda(x, y, p, q); break;
		case 2: draw_line_bresenham(x, y, p, q); break;
		case 3: draw_line_midpoint(x, y, p, q); break;
	}
}

static void draw_circle(ArgumentList list, char **argv) {
	int algo = 0, x = 0, y = 0, r = 0, s = 0;

	const char *algos[] = {"bresenham", "midpoint"};

	algo = expect_oneof('a', list, "Specify the algorithm to use", argv[0], 2,
	                    &algos[0]);

	get_point('x', "centre", &x, &y, list, argv[0]);
	get_int('r', &r, "radius", list, argv[0]);
	get_int_optional('s', &s, "symmetry", list, argv[0], 0);

	if(s != 0) {
		int j = 2;
		while((j << 1) <= s) j <<= 1;
		if(j != s || s < 4 || s > 256) {
			perr("Symmetry must be a power of 2 (4 <= symmetry <= 256) (Given "
			     ": %d)\n",
			     s);
			arg_free(list);
			exit(2);
		}
	}

	init_driver();
	set_pivot(x, y);
	switch(algo) {
		case 1:
			if(s > 0) {
				draw_circle_bresenham_n_point(x, y, r, s);
			} else
				draw_circle_bresenham(x, y, r);
			break;
		case 2: draw_circle_midpoint(x, y, r, s == 0 ? 4 : s); break;
	}
}

static void draw_ellipse(ArgumentList list, char **argv) {
	int x = 0, y = 0, a = 0, b = 0;
	get_point('x', "centre", &x, &y, list, argv[0]);
	get_int('m', &a, "major axis length", list, argv[0]);
	get_int('n', &b, "minor axis length", list, argv[0]);

	init_driver();
	set_pivot(x, y);
	draw_ellipse_midpoint(x, y, a, b);
}

static void draw_clip(ArgumentList list, char **argv) {
	int x = 0, y = 0, p = 0, q = 0, // coordinates of the line
	    bx = 0, by = 0, tx = 0,
	    ty = 0; // coordinates of the diagonal of the clip window
	get_point('x', "starting point of the line", &x, &y, list, argv[0]);
	get_point('y', "ending point of the line", &p, &q, list, argv[0]);
	get_point('b', "bottom left corner of the clip window", &bx, &by, list,
	          argv[0]);
	get_point('t', "top right corner of the clip window", &tx, &ty, list,
	          argv[0]);

	const char *algos[] = {"cohen", "midpoint"};

	int choice = expect_oneof('a', list, "Specify the algorithm to use",
	                          argv[0], 2, &algos[0]);

	enable_transform(0);
	init_driver();
	switch(choice) {
		case 1: clipping_cohen_sutherland(x, y, p, q, bx, by, tx, ty); break;
		case 2:
			clipping_midpoint_subdivision(x, y, p, q, bx, by, tx, ty);
			break;
	}
}

static void perform_bench(ArgumentList list, char **argv) {
	const char *benches[] = {"create", "fill", "add", "sub",
	                         "mult",   "draw", "all"};

	int choice = expect_oneof('c', list, "Specify the benchmark to perform",
	                          argv[0], 7, &benches[0]);

	bench((BenchType)choice);
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		usage(argv[0]);
		return 0;
	}

	ArgumentList list = arg_list_create(12);

	arg_add(list, 'a', "algo", true);
	arg_add(list, 'b', "bottom", true);
	arg_add(list, 'c', "bench", true);
	arg_add(list, 'g', "showgraph", false);
	arg_add(list, 'm', "major", true);
	arg_add(list, 'n', "minor", true);
	arg_add(list, 'o', "object", true);
	arg_add(list, 'r', "radius", true);
	arg_add(list, 's', "symmetry", true);
	arg_add(list, 't', "top", true);
	arg_add(list, 'x', "start", true);
	arg_add(list, 'y', "end", true);

	arg_parse(argc, &argv[0], list);

	if(arg_is_present(list, 'c')) {
		perform_bench(list, &argv[0]);
		arg_free(list);
		return 0;
	}

	const char *objects[] = {"line", "circle", "ellipse", "clip"};

	int choice = expect_oneof('o', list, "Specify object to draw", argv[0], 4,
	                          &objects[0]);

	switch(choice) {
		case 1: draw_line(list, &argv[0]); break;
		case 2: draw_circle(list, &argv[0]); break;
		case 3: draw_ellipse(list, &argv[0]); break;
		case 4: draw_clip(list, &argv[0]); break;
	}
	transform();
	terminate_driver();
	arg_free(list);
	return 0;
}
