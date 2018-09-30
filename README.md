CUGraphics
==========
## An ncurses based not-so-aptly-named collection of graphics algorithms on my curriculam

This program contains implementations of all the line-circle-ellipse drawing, clipping/filling primitives 
that I've learnt in my course. This program uses ncurses to draw simulated "pixels" (unicode block characters)
 on the terminal.

#### Building
```
$ <your-favourite-c-compiler> *.c -lncurses -lm -O3
```

#### Running
Just run `./a.out` (or your output executable name) to have a look at all the available options.

The options basically start with an `--object` or `-o=`, which is the drawable object, then optionally 
an `--algo` or `-a=` when the program has more than one algorithms available for that particular object 
drawing, and the rest of the arguments are basically inputs to the algorithm itself.

#### Files

1. `cargparser.c` : An argument parser written in C which supports both shorthand (`-a=<value>`) and longhand (`--argument <value>`) arguments, 
and has verbose error reporting to the user [`(CargParser)`](https://gitlab.com/iamsubhranil/CargParser).

2. `cargparser.h` : Interface for the argument parser.

3. `circle_drawing.c` : Implementation of circle drawing primitives.

4. `circle_drawing.h` : Interface for the circle drawing primitives.

5. `clipping.c` : Implementation of clipping primitives.

6. `clipping.h` : Interface for the clipping primitives.

7. `common.h` : Some shorter `typedefs` and general `#define`s to use throughout the program.

8. `display.c` : Styled ASCII text printing library.

9. `display.h` : Interface for the styled text printing library.

10. `driver.c` : The graphics driver for the program. 
It is basically wrapper around the `ncurses` library calls which exports only necessary functions to the primitives. 
All drawing primitives manipulate the screen using this wrapper only. This wrapper also provides some transformations 
of pixels after an object has been drawn using various keys on the keyboard.
This wrapper allows to do some fancy `-DNO_DRAW` stuff at compile time, which, when specified, forces the wrapper 
to emulate the actual drawing calls and show the effect in `stdout` rather than actually drawing to the scene. This is 
very helpful for debug purposes, as `ncurses` generally messes up the terminal when exits abruptly. This will also 
allow for a `-DNON_CURSES` flag, i.e. an optional implementation without the `ncurses` dependency, and I will 
probably look into it.

11. `driver.h` : Interface for the wrapper which exports only the bare minimum functions to the primitives.

12. `ellipse_drawing.c` : Implementation of ellipse drawing primitives.

13. `ellipse_drawing.h` : Interface for ellipse drawing primitives.

14. `line_drawing.c` : Implementation of line drawing primitives.

15. `line_drawing.h` : Interface for line drawing primitives.

16. `main.c` : The driver for the program which parses the given arguments using the `CargParser` library, 
converts them to function calls, initializes the graphics driver and calls the required functions.

17. `matrix.c` : Implementation of some matrix multiplication and addition primitives for tranformations.

18. `matrix.h` : Interface for the matrix manipulation primitives.
