#pragma once

#include <stdbool.h>
#include <stdlib.h>

// Opaque struct
typedef struct ArgumentArray *ArgumentList;

// Create an argument list of given size
ArgumentList arg_list_create(size_t size);

// Add an argument with its shorthand '-' form, longhand '--' form, and
// whether or not it requires a value
void arg_add(ArgumentList list, const char shorthand, const char *full,
             bool value_required);

// Parse the argument string
void arg_parse(int argc, char **argv, ArgumentList list);

// Check whether an argument is present
bool arg_is_present(ArgumentList list, const char shorthand);

// Get the value of an argument
char *arg_value(ArgumentList list, const char shorthand);

// Free the list
void arg_free(ArgumentList list);
