#include <stdio.h>
#include <stdlib.h>
#include "chapter_1.h"
#include <sys/time.h>
#include <time.h>

int compare(const void *min, const void *max)
{
	return *(int *)min - *(int *)max;
}

int sort_lib(int *result)
{
	int index = 0;
	FILE *fp = fopen(SOURCE_FILE, "r+");
	FILE *outfp = fopen(RESULT_FILE, "w+");

	while (fscanf(fp, "%d", &result[index]) != EOF)
		index++;

	qsort(result, DATA_NUMS, sizeof(int), compare);
	
	for (int i = 0; i < DATA_NUMS; i++)
		fprintf(outfp, "%d ", result[i]);

	fprintf(outfp, "\n");

	fclose(fp);
	fclose(outfp);

	return 0;
}

int main(int argc, char *argv[])
{
	int *result	= NULL;

	result = calloc(DATA_NUMS, sizeof(int));

	TIME_CACULATE(sort_lib(result));

	free(result);
	return 0;
}
