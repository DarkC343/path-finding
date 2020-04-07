#include "gui.h"
#include <stdlib.h>

void update_markup(GtkWidget *button, const gchar *label_text)
{
    gtk_widget_set_name(button, "visited");
    while(gtk_events_pending())
        gtk_main_iteration();
	gtk_button_set_label(GTK_BUTTON(button), label_text);
}

void start_button_clicked (GtkWidget *widget, struct activate_bind *activate_bind_ptr)
{
    int result = 0;
    if(activate_bind_ptr->algorithm == 0)
    {
        result = dfs(activate_bind_ptr->maze_ptr, activate_bind_ptr->board_ptr->source, activate_bind_ptr->board_ptr->destination, activate_bind_ptr->interval_delay, activate_bind_ptr->perform_statistic);
    }
    else if(activate_bind_ptr->algorithm == 1)
    {
        result = bfs(activate_bind_ptr->maze_ptr, activate_bind_ptr->board_ptr->source, activate_bind_ptr->board_ptr->destination, activate_bind_ptr->interval_delay, activate_bind_ptr->perform_statistic);
    }
    else if(activate_bind_ptr->algorithm == 2)
    {
        fprintf(stderr, "err: algorithm is not implemented yet\n");
        // return EXIT_FAILURE;
    }
    else
    {
        fprintf(stderr, "err: no algorithm is selected\n");
        // return EXIT_FAILURE;
    }

    if(!activate_bind_ptr->perform_statistic)
    {
        if(result == 0)
        {
            // popup("found!");
            fprintf(stdout, "success: there a path from source to destination\n");
        }
        else if(result == 1)
        {
            // popup("not found!");
            fprintf(stderr, "err: there is no path from source to destination\n");
        }
    }
    else
    {
        exit(0);
    }
}

static void activate (GtkApplication *app, struct activate_bind *activate_bind_ptr)
{
    window = gtk_application_window_new (app);
    gchar title[256];
    sprintf(title, "Path Finder - file: %s", activate_bind_ptr->filename);
    gtk_window_set_title (GTK_WINDOW (window), title);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    grid = gtk_grid_new ();

    gtk_container_add (GTK_CONTAINER (window), grid);

    int index = 0;
    for(int i = 0; i < activate_bind_ptr->board_ptr->height; ++i)
        for(int j = 0; j < activate_bind_ptr->board_ptr->width; ++j)
        {
            index = j + activate_bind_ptr->board_ptr->height * i;
            
            buttons[index] = gtk_button_new_with_label (" ");
            if(j == activate_bind_ptr->board_ptr->source->x && i == activate_bind_ptr->board_ptr->source->y)
            {
                gtk_widget_set_name(buttons[index], "source");
            }
            else if(j == activate_bind_ptr->board_ptr->destination->x && i == activate_bind_ptr->board_ptr->destination->y)
            {
                gtk_widget_set_name(buttons[index], "destination");
            }
            else
            {
                gtk_widget_set_name(buttons[index], ((activate_bind_ptr->board_ptr->cell[i][j].type == Wall) ? "wall" : "free"));
            }
            gtk_widget_set_size_request(buttons[index], 25, 25);
            
            gtk_grid_attach (GTK_GRID (grid), buttons[index], j, i, 1, 1);
        }

    if(!activate_bind_ptr->perform_statistic)
    {
        index++;
        buttons[index] = gtk_button_new_with_label ("Start");
        g_signal_connect (buttons[index], "clicked", G_CALLBACK (start_button_clicked), activate_bind_ptr);
        gtk_grid_attach (GTK_GRID (grid), buttons[index], 0, activate_bind_ptr->board_ptr->height, activate_bind_ptr->board_ptr->width, 1);
    }
    else
    {
        start_button_clicked(NULL, activate_bind_ptr);
    }

    // add CSS button properties
	GtkCssProvider *provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_data(
		GTK_CSS_PROVIDER(provider), 
		"button { border-radius:0px; padding-left:9px; padding-right:9px; }\
		#visited { background: green; color: white; }\
		#free { background: white }\
        #wall { background: url(\"img/wall.jpg\"); background-size: 50px 50px; }\
        #source { background: black; background: url(\"img/person.png\"); background-size: 30px 30px; background-repeat: no-repeat; background-position: center; }\
        #destination { background: black; background: url(\"img/home.png\"); background-size: 30px 30px; background-repeat: no-repeat; background-position: center; }",
		-1, 
		NULL
	);

    gtk_widget_show_all (window);
}

int play_gui(int algorithm, int interval_delay, bool perform_statistic, char *filename, maze_t *maze_ptr, board_t *board_ptr)
{
    // activate GTK application
    struct activate_bind av = { algorithm, interval_delay, filename, maze_ptr, board_ptr, perform_statistic };
	buttons = (GtkWidget **)malloc((board_ptr->width * board_ptr->height + 1) * sizeof(GtkWidget *));
	GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &av);
	int status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	// clean up
	free(buttons);

	return status;
}
