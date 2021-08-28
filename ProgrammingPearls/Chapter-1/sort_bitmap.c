#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "chapter_1.h"
#include "bitmap.h"

REGISTER_BITMAP_OPS(int)

int sort_bitmap(int *result)
{
	int value;
	FILE *fp = fopen(SOURCE_FILE, "r+");
	FILE *outfp = fopen(RESULT_FILE_BITMAP, "w+");

	while (fscanf(fp, "%d", &value) != EOF)
		set_bit_int(result, value);

	for (int i = 0; i < DATA_NUMS; i++)
		if (test_bit_int(result, i))
			fprintf(outfp, "%d ", i);

	fprintf(outfp, "\n");

	fclose(fp);
	fclose(outfp);

	return 0;
}

int main(int argc, char *argv[])
{
	int *result = NULL;

	result = calloc(get_int_array_length(DATA_NUMS), sizeof(int));

	TIME_CACULATE(sort_bitmap(result));

	free(result);
	return 0;
}
