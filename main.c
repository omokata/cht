#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	char *key;
	size_t val;
} FreqKV;

typedef struct {
	FreqKV *items;
	size_t count;
	size_t capacity;
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
	ht->capacity = capacity;
	ht->count = count;
	return 1;
}

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

int main()
{
	FreqKVs freq_kv = {0};
	if (!new_freq_kvs(&freq_kv, 25)) {
		return 1;
	}

	printf("cap: %d, length: %d\n", freq_kv.capacity, freq_kv.count);
	
	const char *path = "t8.shakespeare.txt";
	FILE *file = fopen(path, "r");

	for (int i = 0; i < 1000; ++i) {
		char word[256];
	    if (tokenizer(file, word))
			printf("%s\n", word);
	}

	fclose(file);
	return 0;
}


