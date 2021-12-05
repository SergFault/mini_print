#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct s_shape
{
	char type;
	float x;
	float y;
	float r;
	char color;
} t_shape;

typedef struct s_zone
{
	int width;
	int height;
	char bg;
} t_zone;

void print_str(const char *str)
{
	while(*str)
		write(1, str++, 1);
}

int get_zone(t_zone *zone, FILE *file)
{
	if (fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->bg) !=3)
	{
		return 0;
	}
	return 1;
}

int check_zone(t_zone *zone)
{
	return ((zone->height > 0) && (zone->height <=300) && (zone->width > 0) && (zone->width <= 300));
}

int fill_bg(char **drawing, t_zone *zone)
{
	if (!(*drawing = (char *)malloc(sizeof(char) * zone->width * zone->height)))
		return 0;
	int i = 0;
	while (i < zone->width * zone->height)
	{
		(*drawing)[i++] = zone->bg;
	}
	return 1;
}

void show(char *drawing, t_zone *zone)
{
	int i, j;
	char ln = '\n';
	i = 0;
	while(i < zone->height)
	{
		j = 0;
		while(j < zone->width)
		{
			write(1, &drawing[i * zone->width + j], 1);
			j++;
		}
		write(1, &ln, 1);
		i++;
	}
}

int check_shape(t_shape *shape)
{
	return ((shape->r > 0.00000000) && (shape->type == 'c' || shape->type == 'C'));
}

int is_in(int x, int y, t_shape *shape)
{
	float distance;
	distance = sqrtf( pow(x - shape->x, 2) + pow(y - shape->y, 2));
	float differ;
	differ = distance - shape->r;
	if (differ > 0.00000000)
		return 0;
	if (differ > -1.00000000)
		return 2;
	return 1;
}

void draw_shape(char *drawing, t_shape *shape, t_zone *zone)
{
	int in;
	int i, j;
	i = 0;
	while(i < zone->height)
	{
		j = 0;
		while(j < zone->width)
		{
			in = is_in(j, i, shape);
			if (in == 2 && shape->type == 'c')
				drawing[i * zone->width + j] = shape->color;
			if (in && shape->type == 'C')
				drawing[i * zone->width + j] = shape->color;
			j++;
		}
		i++;
	}
}

int draw_shapes(char *drawing, FILE *file, t_zone *zone)
{
	int status;
	t_shape shape;

	status = fscanf(file, "%c %f %f %f %c\n", &shape.type, &shape.x, &shape.y, &shape.r, &shape.color);
	while (status == 5)
	{
		if (!(check_shape(&shape)))
			return 0;
		draw_shape(drawing, &shape, zone);
		status = fscanf(file, "%c %f %f %f %c\n", &shape.type, &shape.x, &shape.y, &shape.r, &shape.color);
	}
	if (status == -1)
		return (1);
	return (0);
}

int main(int argc, char *argv[])
{
	t_zone zone;
	FILE *file;
	char *drawing;

	if (argc != 2)
	{
		print_str("Error: argument\n");
		return 1;
	}
	if (!(file = fopen(argv[1], "r")))
	{	
		print_str("Error: Operation file corrupted\n");
		return 1;
	}
	if (!get_zone(&zone, file))
	{
		fclose(file);
		print_str("Error: Operation file corrupted\n");
		return 1;
	}
	if (!check_zone(&zone))
	{
		fclose(file);
		print_str("Error: Operation file corrupted\n");
		return 1;
	}
	if (!fill_bg(&drawing, &zone))
	{
		fclose(file);
		print_str("Error: Operation file corrupted\n");
		return 1;
	}
	if (!draw_shapes(drawing, file, &zone))
	{
		fclose(file);
		free(drawing);
		print_str("Error: Operation file corrupted\n");
		return 1;
	}
	show(drawing, &zone);
	fclose(file);
	free(drawing);
	return 0;
}