#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "chapter_1.h"

unsigned long bigrand(void)
{
	return ((1UL << (sizeof(int) * 8)) - 1) * rand() + rand();
}

int randint(int min, int max)
{
	return min + bigrand() % (max - min + 1);
}

void swap(int *src, int *dst)
{
	int temp = *src;
	*src = *dst;
	*dst = temp;
}

int main(void)
{
	int *data = NULL;
	FILE *fp = NULL;

	data = calloc(DATA_NUMS, sizeof(int));
	memset(data, 0, sizeof(int) * DATA_NUMS);

	fp = fopen(SOURCE_FILE, "w+");
	if (!fp) {
		perror("try to open file failed!");
		return errno;
	}

	for (int i = 0; i < DATA_NUMS; i++)
		data[i] = i + 1;

	for (int i = 0; i < DATA_NUMS; i++) {
		int temp = randint(i, DATA_NUMS) % DATA_NUMS;
		swap(&data[i], &data[temp]);
	}

	for (int i = 0; i < DATA_NUMS; i++) {
		fprintf(fp, "%d ", data[i]);
	}

	fprintf(fp, "\n");

	return 0;
}
