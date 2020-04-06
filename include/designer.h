#ifndef DESIGNER_H
#define DESIGNER_H

#include <stdbool.h>
#include <gtk/gtk.h>

typedef enum Type
{
    Free,
    Wall
} type_t;

typedef struct position
{
    int x;
    int y;
} position_t;

typedef struct cell
{
    type_t type;
    position_t *position;
} cell_t;

typedef struct board
{
    int width;
    int height;
    cell_t **cell;
    position_t *source;
    position_t *destination;
} board_t;

typedef struct bind
{
    board_t *board;
    cell_t *cell;
    GtkWidget *button;
} bind_t;

typedef struct file
{
    FILE *file;
    char *filename;
} file_t;


#endif