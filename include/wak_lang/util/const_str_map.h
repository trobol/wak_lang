#ifndef _WAK_LANG_UTIL_CONST_STR_MAP_H
#define _WAK_LANG_UTIL_CONST_STR_MAP_H
#include <wak_lang/util/assert.h>
#include <wak_lang/util/mem.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/*
String map that is initialized with all its values
holds an unsigned 64 bit integer
holds internal copy of strings
*/

typedef struct {
	const char* key;
	uint64_t value;
} const_str_map_pair;


typedef struct {
	size_t entry_count;
	const char* *keys;
	uint64_t* values;
} const_str_map;


const_str_map* const_str_map_new(const_str_map_pair pairs[], size_t entry_count) {

	// Note: this is bubble sort but we shouldnt have many keys so not a problem for now

	for (size_t left = 0, right = entry_count - 1; right > 0;
		     right = left, left = 0)
	{
		for (size_t i = 0; i < right; ++i)
		{
			int cmp = strcmp(pairs[i + 1].key, pairs[i].key);
			if (cmp < 0)
			{
				const_str_map_pair tmp_val = pairs[i];
				pairs[i] = pairs[i + 1];
				pairs[i + 1] = tmp_val;
				left = i;
			}
			wak_assert_msg(cmp != 0, "map contains matching keys");
		}
	}
	
	const_str_map* map = (const_str_map*)malloc(sizeof(const_str_map));
	
	map->values = alloc_array(entry_count, uint64_t);
	map->keys = alloc_array(entry_count, const char*);
	map->entry_count = entry_count;
	
	for(size_t i = 0; i < entry_count; i++) {
		map->values[i] = pairs[i].value;
		map->keys[i] = pairs[i].key;
	}

	return map;
}

void const_str_map_free(const_str_map* map) {
	free(map->keys);
	free(map->values);
	free(map);
}

// based on GCC STD lib
// compare two strings, where p2 is not null terminated
int strcmp_len (const char *p1, const char *p2, const char *p2_end)
{
	const unsigned char *s1 = (const unsigned char *) p1;
	const unsigned char *s2 = (const unsigned char *) p2;
	const unsigned char *s2_end = (const unsigned char *) p2_end;
	unsigned char c1, c2;
	do
	{
		c1 = (unsigned char) *s1;
		c2 = (unsigned char) *s2;
		if (c1 == '\0')
			return c1 - c2;
		s1++;
		s2++;
	}
	while (c1 == c2 && s2 < s2_end);
	return c1 - c2;
}

// find item with string of length (string can be not null terminated)
uint64_t const_str_map_find_len(const_str_map* map, const char* key, const char* key_end) {
	uint32_t i = 0;
	
	do {
		int cmp = strcmp_len(map->keys[i], key, key_end);
		if (cmp == 0) return map->values[i];
		i++;
	} while(i < map->entry_count);
	
	return -1;
}

// find item will null terminated string
uint64_t const_str_map_find(const_str_map* map, const char* key) {
	int l = 0;
	int r = map->entry_count - 1;
	while (l <= r)
	{
		int m = l + (r - l) / 2;

		int cmp = strcmp(map->keys[m], key);
		if (cmp < 0)
			l = m + 1;
		else if (cmp == 0)
			return map->values[m];
		else
			r = m - 1;
	}
	return -1;
}


#endif