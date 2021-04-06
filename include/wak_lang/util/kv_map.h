#ifndef _WAK_LANG_KV_MAP_H
#define _WAK_LANG_KV_MAP_H
#include <stdlib.h>
#include <string.h>
#include <wak_lang/util/assert.h>
#include <wak_lang/util/mem.h>

typedef struct kv_pair {
	const char* key;
	int value;
} kv_pair;

typedef struct kv_map {
	size_t entry_count;
	const char* *keys;
	int *values;
} kv_map;

kv_map* kv_map_new(kv_pair pairs[], size_t entry_count) {


	// TODO: this is bubble sort but we shouldnt have many keys so not a problem for now

	for (size_t left = 0, right = entry_count - 1; right > 0;
		     right = left, left = 0)
	{
		for (size_t i = 0; i < right; ++i)
		{
			int cmp = strcmp(pairs[i + 1].key, pairs[i].key);
			if (cmp < 0)
			{
				kv_pair tmp_val = pairs[i];
				pairs[i] = pairs[i + 1];
				pairs[i + 1] = tmp_val;
				left = i;
			} else if (cmp == 0) {
				printf("map contains matching keys\n");
			}
		}
	}
	
	kv_map* map = (kv_map*)malloc(sizeof(kv_map));
	
	map->values = alloc_array(entry_count, int);
	map->keys = alloc_array(entry_count, const char*);
	map->entry_count = entry_count;
	
	for(size_t i = 0; i < entry_count; i++) {
		map->values[i] = pairs[i].value;
		map->keys[i] = pairs[i].key;
	}

	return map;
}

void kv_map_free(kv_map* map) {
	free(map->keys);
	free(map->values);
	free(map);
}

int kv_map_find(kv_map* map, const char* key) {
	int l = 0;
	int r = map->entry_count - 1;
	while (l <= r)
	{
		int m = l + (r - l) / 2;

		const char *lhs = map->keys[m];
		const char *rhs = key;
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