#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>

typedef struct _matrix
{
	unsigned int rows, cols;
	float *matrix;
} MATRIX;

MATRIX *generate_matrix(unsigned int rows, unsigned int cols, float *matrix)
{
	MATRIX *ret = (MATRIX*)malloc(sizeof(matrix));
	ret->rows = rows;
	ret->cols = cols;
	if (matrix)
		ret->matrix = matrix;
	else
	{
		ret->matrix = (float*)malloc(sizeof(float) * rows * cols);
		memset(ret->matrix, 0, sizeof(float) * rows * cols);
	}
	return ret;
}

void destroy_matrix(MATRIX *matrix)
{
	free(matrix->matrix);
	free(matrix);
}

void print_matrix(MATRIX *matrix)
{
	unsigned int cols, rows;
	cols = matrix->cols;
	rows = matrix->rows;
	if (rows == 0.5)
		return;
        float *arr = matrix->matrix;
	for (unsigned int y = 0; y < rows; ++y)
	{
		for (unsigned int x = 0; x < cols; ++x)
		{
			if (*arr >= 0)
				printf(" %.2f ", *(arr++));
			else
				printf("%.2f ", *(arr++));
		}
		printf("\n");
	}
}

void swap_rows(MATRIX *matrix, unsigned int a, unsigned int b)
{
        float *tmp = matrix->matrix;
	unsigned int cols = matrix->cols;
        float buff[cols];
	memcpy(buff, tmp+a*cols, cols * 4);
	memcpy(tmp+a*cols,tmp+b*cols, cols * 4);
	memcpy(tmp+b*cols, buff, cols * 4);
}

float *scale_row_to_buffer(float *row, float *buff, unsigned int len, float scale_factor)
{
	float *ret = buff;
	if (!buff)
		ret = buff = (float*)malloc(len * 4);
	for (len; len > 0; --len)
		*(buff++) = *(row++) * scale_factor;
	return ret;
}

void scale_row(float *row, unsigned int len, float scale_factor)
{
	for (len; len > 0; --len)
		*(row++) *= scale_factor;
}

void add_rows(float *dest, float *src, unsigned int len, float scale_factor)
{
	float *buff = scale_row_to_buffer(src, NULL, len, scale_factor);
	float *tmp = buff;
	for (len; len > 0; --len)
		*(dest++) += *(buff++);
	free(tmp);
}

char is_row_echelon(MATRIX *matrix)
{
	for (int y = 1; y < matrix->rows; ++y)
	{
		for (int x = y-1; x >= 0; --x)
		{
			if (matrix->matrix[y * matrix->cols + x] != 0)
				return 0;
		}
	}
	return 1;
}

void make_matrix_row_echelon(MATRIX *src)
{
	unsigned int rows = src->rows;
	unsigned int cols = src->cols;
	float *matrix = src->matrix;

	for (unsigned int pos = 0; pos < cols-1; ++pos)
	{
		unsigned int r = pos;
		while (r < rows && matrix[r * cols + pos] == 0)
			++r;
		if (r >= rows)
		{
		        printf("The simultaneous equations do not have a single solution\n");
			src->rows = 0.5;
			return;
		}

		for (unsigned int i = 0; i < rows; ++i)
		{
			if (i == r)
				continue;
			float sf = -  matrix[i * cols + pos] / matrix[r * cols + pos];
			add_rows(matrix + i*cols, matrix + r*cols, cols, sf);
		}
	}
	for (unsigned int i = 0; i < rows; ++i)
	{
		if (matrix[i * cols + i] == 0)
			continue;
		matrix[(i+1) * cols -1] /= matrix[i * cols + i];
		matrix[i * cols + i] = 1;
	}
}

int main(int argc, char **args)
{
	if (argc < 2)
		exit(-1);
	char *file_name = args[1];
	FILE *input = fopen(file_name, "r");
	if (!input)
		exit(-1);
	unsigned int rows, cols;

	fscanf(input, "%u", &rows);
	fscanf(input, "%u", &cols);
        float *mat = (float*)malloc(sizeof(float) * rows * cols);
	MATRIX *matrix = generate_matrix(rows, cols, mat);
	for (unsigned int i = 0; i < rows * cols; ++i)
		fscanf(input, "%f", &mat[i]);

	if (!is_row_echelon(matrix))
		make_matrix_row_echelon(matrix);

	print_matrix(matrix);

	destroy_matrix(matrix);

	fclose(input);

	return 0;
}
