typedef __builtin_va_list va_list;
#include <Kure.h>

// Map from embedded-lang functions to Lua functions
// Defined in parser.c
extern struct kure_fun_map_t {
 char * name;
 int arg_count;
 char * transl_name;
} kure_fun_map[];
