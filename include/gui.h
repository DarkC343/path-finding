#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "designer.h"
#include "pathfinder.h"

GtkWidget *window;
GtkWidget *grid;
GtkWidget **buttons;

struct activate_bind
{
    int algorithm; // 0 for dfs, 1 for bfs, 2 for a-star
    int interval_delay;
    char *filename;
    maze_t *maze_ptr;
    board_t *board_ptr;
    bool perform_statistic;
};

void update_markup(GtkWidget *button, const gchar *label_text);
static void start_button_clicked (GtkWidget *widget, struct activate_bind *activate_bind_ptr);
static void activate (GtkApplication *app, struct activate_bind *activate_bind_ptr);
int play_gui(int algorithm, int interval_delay, bool perform_statistic, char *filename, maze_t *maze_ptr, board_t *board_ptr);

#endif