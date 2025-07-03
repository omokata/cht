#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

int tokenizer(FILE *stream, char word[])
{

	static char last_char = ' ';

	while (isspace(last_char))
		last_char = fgetc(stream);

	if (isalnum(last_char)) {
		int i = 0;
		word[i] = last_char;
		while (isalnum((last_char = fgetc(stream))))
			word[++i] = last_char;

		word[++i] = '\0';
		return 1;
	} else if (last_char == EOF) {
		return -1;
	} else {
		last_char = fgetc(stream);
		return 0;
	}
}

typedef struct {
	char *key;
	size_t val;
	bool occupied;
} FreqKV;

typedef struct {
	FreqKV *items;
	size_t count;
	size_t cap;
} FreqKVs;

int ht_insert(FreqKVs *ht, int at, char *key, int val);
void ht_free(FreqKVs *ht);
	
uint32_t dbj2(uint8_t *buf, size_t buf_size)
{
    uint32_t hash = 5381;

	for (size_t i = 0; i < buf_size; ++i) {
		hash = ((hash << 5) + hash) + (uint32_t)buf[i]; /* hash * 33 + c */
	}

	return hash;
}

int new_freq_kvs(FreqKVs *ht, size_t capacity)
{
	FreqKV *items = malloc(capacity*sizeof(FreqKV));
	if (items == NULL) {
		perror("ht");
		return 0;
	}
	size_t count = 0;
	ht->items = items;
	ht->cap = capacity;
	ht->count = count;
	return 1;
}

int ht_resize(FreqKVs *ht)
{

	// TODO FIXED RESIZE!!!
	int new_cap = 2 * ht->cap;
	// REHASHING
	FreqKV *items = calloc(new_cap, sizeof(FreqKV));
	if (items == NULL) return 0;
	
	for (size_t i = 0; i < ht->cap; ++i) {
		if (ht->items[i].occupied) {
			char *token = ht->items[i].key;
			size_t token_len = strlen(token);
			uint32_t hash = dbj2(token, token_len);
			int h = hash % new_cap;
			while(items[h].occupied)
				h = (h + 1) % new_cap;
			items[h].key = calloc(token_len + 1, sizeof(char));
			strcpy(items[h].key, token);
			items[h].val = ht->items[i].val;
			items[h].occupied = true;
		}
	}

	ht_free(ht);
	ht->items = items;
	ht->cap = new_cap;
	return 1;
}

int ht_insert(FreqKVs *ht, int at, char *key, int val)
{
	if (ht->count >= ht->cap) return 0;
		
    char *buf = calloc(strlen(key) + 1, sizeof(char));
	if (buf == NULL) {
		return 0;
	}
	strcpy(buf, key);
	ht->items[at].key = buf;
	ht->items[at].val = val;
	ht->items[at].occupied = true;
	ht->count++;
	return 1;

}

void ht_free(FreqKVs *ht)
{
	for (int i = 0; i < ht->count; ++i) {
		if (ht->items[i].key == NULL)
			free(ht->items[i].key);
	}
	free(ht->items);
}

void log_freq(FreqKVs ht)
{
	printf("cap: %d, length: %d\n", ht.cap, ht.count);
	for (size_t i = 0; i < ht.count; ++i) {
		printf("%s => %d\n", ht.items[i].key, ht.items[i].val);
	}
}

void top_10_freq(FreqKVs ht)
{
	printf("cap: %d, length: %d\n", ht.cap, ht.count);
	for (size_t i = 0; i < ht.count; ++i) {
		printf("%s => %d\n", ht.items[i].key, ht.items[i].val);
	}
}


void toy_analysis(FreqKVs ht, FILE *file)
{
	for (int i = 0; i < 1000; ++i) {
		char word[256];
	    if (!tokenizer(file, word)) continue;
		for (int j = 0; j < ht.cap; ++j) {
			if (ht.items[j].key == NULL) {
				if (!ht_insert(&ht, j, word, 1)) {
					perror("ht");
					break;
				}
				ht.count++;
				break;
			} else {
				if (strcmp(ht.items[j].key, word) == 0) {
					ht.items[j].val++;
					break;
				} else {
					continue;
				}
			}
		}
		if (ht.count >= ht.cap) ht_resize(&ht);
	}	
}

int main(void)
{
	FreqKVs ht = {0};
	if (!new_freq_kvs(&ht, 1000)) {
		return 1;
	}
	
	const char *path = "shakespear-smol.txt";
	FILE *file = fopen(path, "r");

	char token[256];
	int tokenizer_st = tokenizer(file, token);
	while (tokenizer_st != -1) {
		/* printf("Curr Token: %s\n", token); */
		if (tokenizer_st == 0) {
			tokenizer_st = tokenizer(file, token);
			continue;
		}

		size_t token_len = strlen(token);
		uint32_t hash = dbj2(token, token_len);
		int h = hash % ht.cap;

		FreqKV kv = ht.items[h];
		if (kv.occupied == false) {
			ht_insert(&ht, h, token, 1);
		} else if (strcmp(kv.key, token) == 0) {
			ht.items[h].val++;
		} else {
			int start = h;
			while (ht.items[h].occupied) {
				if (strcmp(ht.items[h].key, token) == 0) {
					ht.items[h].val++;
					break;
				}
				
				h = (h + 1) % ht.cap;
				
				if (h == start) {
					break;
				}
			}
		}
		
		tokenizer_st = tokenizer(file, token);
	}
	
	fclose(file);

	log_freq(ht);
	ht_free(&ht);
	return 0;
}
