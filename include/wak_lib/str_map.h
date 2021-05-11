#ifndef _WAK_LIB_STR_MAP_H
#define _WAK_LIB_STR_MAP_H

#include <wak_lib/assert.h>
#include <wak_lib/mem.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*
String map that can be added to
holds an unsigned 64 bit integer
holds internal copies of strings
*/

struct vector_str;
typedef struct vector_str vector_str;

struct vector_uint64;
typedef struct vector_uint64 vector_uint64;



typedef struct {
	const char* key;
	uint64_t value;
} str_map_pair;


typedef struct {
	size_t entry_count;
	vector_str* keys;
	vector_uint64* values;
} str_map;


str_map* str_map_new();

void str_map_free(str_map* map);

uint64_t str_map_find(str_map* map, const char* key);
void str_map_add_pair(str_map* map, str_map_pair pair);
void str_map_add(str_map* map, const char* key, uint64_t value);

#endif