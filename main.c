#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
	} else {
		last_char = fgetc(stream);
		return 0;
	}
}

typedef struct {
	char *key;
	size_t val;
} FreqKV;

typedef struct {
	FreqKV *items;
	size_t count;
	size_t cap;
} FreqKVs;

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
	FreqKV *items = realloc(ht->items, 2*ht->cap*sizeof(FreqKV));
	if (items == NULL) {
		return 0;
	} else {
		ht->items = items;
		return 1;
	}
}

void ht_free(FreqKVs *ht)
{
	for (int i = 0; i < ht->count; ++i) {
		free(ht->items[i].key);
	}
	free(ht->items);
}

void log_freq(FreqKVs ht)
{
	printf("HT Count: %d\n", ht.count);
	for (size_t i = 0; i < ht.count; ++i) {
		printf("%s => %d\n", ht.items[i].key, ht.items[i].val);
	}
}

int main(void)
{
	FreqKVs ht = {0};
	if (!new_freq_kvs(&ht, 25)) {
		return 1;
	}

	printf("cap: %d, length: %d\n", ht.cap, ht.count);
	
	const char *path = "t8.shakespeare.txt";
	FILE *file = fopen(path, "r");

	for (int i = 0; i < 5000; ++i) {
		char word[256];
	    if (!tokenizer(file, word)) continue;
		/* printf("Word: %s\n", word); */
		for (int j = 0; j < ht.cap; ++j) {
			/* printf("Val: %d\n", ht.items[j].val); */
			/* printf("Word: %s\n", word); */
			if (ht.items[j].val == 0) {
				ht.items[j].key = malloc(16*sizeof(char));
				strcpy(ht.items[j].key, word);
				ht.items[j].val++;
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

	fclose(file);

	log_freq(ht);
	ht_free(&ht);
	return 0;
}


