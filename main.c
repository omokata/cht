#include <stdio.h>
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

int main()
{
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


