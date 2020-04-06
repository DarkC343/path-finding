#include "designer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define LEVEL_OF_RANDOMNESS 2

bool source_changed = false;
bool destination_changed = false;
file_t *in_file;

GtkWidget *window;
GtkWidget *grid;
GtkWidget **buttons;

int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

static void maze_button_clicked (GtkWidget *widget, bind_t *bind_ptr)
{

    if(strcmp(gtk_widget_get_name(bind_ptr->button), "wall") == 0)
    {
        if(bind_ptr->cell->position->y == 0 || bind_ptr->cell->position->y == bind_ptr->board->height - 1 || bind_ptr->cell->position->x == 0 || bind_ptr->cell->position->x == bind_ptr->board->height- 1) return;
        if(source_changed || destination_changed) return;
        bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].type = Free;
        gtk_widget_set_name(bind_ptr->button, "free");
    }
    else if(strcmp(gtk_widget_get_name(bind_ptr->button), "free") == 0)
    {
        if(source_changed)
        {
            bind_ptr->board->source->x = bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].position->x;
            bind_ptr->board->source->y = bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].position->y;
            gtk_widget_set_name(bind_ptr->button, "source");
            source_changed = false;
        }
        else if(destination_changed)
        {
            bind_ptr->board->destination->x = bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].position->x;
            bind_ptr->board->destination->y = bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].position->y;
            gtk_widget_set_name(bind_ptr->button, "destination");
            destination_changed = false;
        }
        else
        {
            bind_ptr->board->cell[bind_ptr->cell->position->y][bind_ptr->cell->position->x].type = Wall;
            gtk_widget_set_name(bind_ptr->button, "wall");
        }
    }
    else if(strcmp(gtk_widget_get_name(bind_ptr->button), "source") == 0)
    {
        if(!destination_changed)
        {
            bind_ptr->board->source->x = -1;
            bind_ptr->board->source->y = -1;
            source_changed = true;
            gtk_widget_set_name(bind_ptr->button, "free");
        }
    }
    else if(strcmp(gtk_widget_get_name(bind_ptr->button), "destination") == 0)
    {
        if(!source_changed)
        {
            bind_ptr->board->destination->x = -1;
            bind_ptr->board->destination->y = -1;
            destination_changed = true;
            gtk_widget_set_name(bind_ptr->button, "free");
        }
    }
}

static void save_button_clicked (GtkWidget *widget, board_t *board_ptr)
{
    if(!source_changed && !destination_changed)
    {
        //1. width
        //2. height
        //3. source
        //4. destination
        //   each cell (width * height):
        //     {5. type,
        //      6. position}
        freopen(NULL, "wb", in_file->file);
        fwrite(&board_ptr->width, sizeof(int), 1, in_file->file);
        // g_print("%d\n", board_ptr->width);
        fwrite(&board_ptr->height, sizeof(int), 1, in_file->file);
        // g_print("%d\n", board_ptr->height);
        fwrite(board_ptr->source, sizeof(position_t), 1, in_file->file);
        // g_print("%d, %d\n", board_ptr->source->x, board_ptr->source->y);
        fwrite(board_ptr->destination, sizeof(position_t), 1, in_file->file);
        // g_print("%d, %d\n", board_ptr->destination->x, board_ptr->destination->y);
        for(int i = 0; i < board_ptr->height; ++i)
            for(int j = 0; j < board_ptr->width; ++j)
            {
                fwrite(&board_ptr->cell[i][j].type, sizeof(type_t), 1, in_file->file);
                // g_print("%s\n", board_ptr->cell[i][j].type == Wall ? "wall" : "free");
                fwrite(board_ptr->cell[i][j].position, sizeof(position_t), 1, in_file->file);
                // g_print("%d, %d\n", board_ptr->cell[i][j].position->x, board_ptr->cell[i][j].position->y);
            }
    }
    else
    {
        fprintf(stderr, "err: first set %s position and then try to save\n", !source_changed ? "source" : "destination");
    }
}

static void quit_button_clicked (GtkWidget *widget, GtkWidget *window)
{
    if(!source_changed && !destination_changed) gtk_widget_destroy(window);
    else
    {
        fprintf(stderr, "err: first set %s position and then try to quit\n", !source_changed ? "source" : "destination");
    }
}

static void activate (GtkApplication *app, board_t *board_ptr)
{
    window = gtk_application_window_new (app);
    gchar title[256];
    sprintf(title, "Maze Designer - file: %s", in_file->filename);
    gtk_window_set_title (GTK_WINDOW (window), title);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    grid = gtk_grid_new ();

    gtk_container_add (GTK_CONTAINER (window), grid);

    int index = 0;
    for(int i = 0; i < board_ptr->height; ++i)
        for(int j = 0; j < board_ptr->width; ++j)
        {
            index = j + board_ptr->height * i;

            bind_t *bind = (bind_t *) malloc(sizeof(bind_t));
            bind->board = board_ptr;
            buttons[index] = gtk_button_new_with_label (" ");

            if(j == board_ptr->source->x && i == board_ptr->source->y)
            {
                gtk_widget_set_name(buttons[index], "source");
            }
            else if(j == board_ptr->destination->x && i == board_ptr->destination->y)
            {
                gtk_widget_set_name(buttons[index], "destination");
            }
            else
            {
                gtk_widget_set_name(buttons[index], ((board_ptr->cell[i][j].type == Wall) ? "wall" : "free"));
            }
            
            bind->cell = &board_ptr->cell[i][j];
            bind->button = buttons[index];
            g_signal_connect (buttons[index], "clicked", G_CALLBACK (maze_button_clicked), bind);
            gtk_grid_attach (GTK_GRID (grid), buttons[index], j, i, 1, 1);
        }

    index++;
    buttons[index] = gtk_button_new_with_label ("Save");
    g_signal_connect (buttons[index], "clicked", G_CALLBACK (save_button_clicked), board_ptr);
    gtk_grid_attach (GTK_GRID (grid), buttons[index], 0, board_ptr->height, board_ptr->width, 1);

    index++;
    buttons[index] = gtk_button_new_with_label ("Quit");
    g_signal_connect (buttons[index], "clicked", G_CALLBACK (quit_button_clicked), window);
    gtk_grid_attach (GTK_GRID (grid), buttons[index], 0, board_ptr->height + 1, board_ptr->width, 1);

    // add CSS button properties
	GtkCssProvider *provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_data(
		GTK_CSS_PROVIDER(provider), 
		"button { border-radius:0px; padding-left:9px; padding-right:9px; }\
		#free { background: white }\
        #wall { background: url(\"img/wall.jpg\"); background-size: 50px 50px; }\
        #source { background: black; background: url(\"img/person.png\"); background-size: 30px 30px; background-repeat: no-repeat; background-position: center; }\
        #destination { background: black; background: url(\"img/home.png\"); background-size: 30px 30px; background-repeat: no-repeat; background-position: center; }",
		-1, 
		NULL
	);

    gtk_widget_show_all (window);
}

int main(int argc, char *argv[], char **envp)
{
    bool custom_paint = true;
    board_t *board = (board_t *) malloc(sizeof(board_t));
    in_file = (file_t *) malloc(sizeof(file_t));
    if(argc == 5 || argc == 2)
    {
        if(argc == 5) // new
        {
            if(strcmp(argv[4], "random") == 0)
            {
                custom_paint = false;
            }
            else if(strcmp(argv[4], "custom") == 0)
            {
                custom_paint = true;
            }
            else
            {
                fprintf(stderr, "err: wrong method selected\n");
                return EXIT_FAILURE;
            }
            
            if(atoi(argv[1]) > 2 && atoi(argv[1]) < 40) board->width = atoi(argv[1]);
            else
            {
                fprintf(stderr, "err: width must be greater than 2 and less than 40\n");
                return EXIT_FAILURE;
            }
            if(atoi(argv[2]) > 2 && atoi(argv[2]) < 40) board->height = atoi(argv[2]);
            else
            {
                fprintf(stderr, "err: height must be greater than 2 and less than 40\n");
                return EXIT_FAILURE;
            }
            in_file->file = fopen(argv[3], "wb");
            in_file->filename = argv[3];
            if(!in_file->file)
            {
                fprintf(stderr, "err: file can not be created\n");
                return EXIT_FAILURE;
            }
            // fprintf(stdout, "width = %d, height = %d\n", board->width, board->height);
        }
        else // edit
        {
            if(access(argv[1], F_OK) != -1)
            {
                in_file->file = fopen(argv[1], "rb");
                in_file->filename = argv[1];
            }
            else
            {
                fprintf(stderr, "err: file does not exist\n");
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        fprintf(stderr, "usage: %s WIDTH HEIGHT FILENAME [random|custom] (create new)\nusage: %s FILENAME (edit existing)\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    board->cell = (cell_t **) malloc(sizeof(cell_t *) * board->height);
    
    if(argc == 5) // create for new
    {
        for(int i = 0; i < board->height; ++i)
        {
            board->cell[i] = (cell_t *) malloc(sizeof(cell_t) * board->width);
            for(int j = 0; j < board->width; ++j)
            {
                // set current position info
                board->cell[i][j].position = (position_t *) malloc(sizeof(position_t));
                board->cell[i][j].position->x = j;
                board->cell[i][j].position->y = i;

                // set wall areas
                if(i == 0 || i == board->height - 1)
                {
                    board->cell[i][j].type = Wall;
                }
                else
                {
                    if(j == 0 || j == board->width - 1)
                    {
                        board->cell[i][j].type = Wall;
                    }
                    else
                    {
                        board->cell[i][j].type = Free;
                    }
                }
            }
        }
        board->source = (position_t *) malloc(sizeof(position_t));
        board->destination = (position_t *) malloc(sizeof(position_t));
        if(custom_paint == true)
        {
            // set source pos
            board->source->x = 1;
            board->source->y = 1;
            // set destination pos
            board->destination->x = board->width - 2;
            board->destination->y = board->height - 2;
        }
        else
        {
            board->source->x = random_number(1, board->width - 2);
            board->source->y = random_number(1, board->height - 2);

            board->destination->x = random_number(1, board->width - 2);
            board->destination->y = random_number(1, board->height - 2);

            // avoid duplicate
            while(board->source->x == board->destination->x && board->source->y == board->destination->y)
            {
                board->destination->x = random_number(1, board->width - 2);
                board->destination->y = random_number(1, board->height - 2);
            }

            for(int i = 1; i < board->height - 1; ++i)
            {
                for(int j = 0; j < LEVEL_OF_RANDOMNESS; ++j)
                {
                    position_t *random_position = (position_t *) malloc(sizeof(position_t));
                    random_position->x = random_number(i, board->width - 2);
                    random_position->y = i;
                    if(!((random_position->x == board->source->x && random_position->y == board->source->y) ||
                            (random_position->x == board->destination->x && random_position->y == board->destination->y)))
                            {
                                board->cell[random_position->y][random_position->x].type = Wall;
                            }
                }
            }
            for(int i = 1; i < board->width - 1; ++i)
            {
                for(int j = 0; j < LEVEL_OF_RANDOMNESS; ++j)
                {
                    position_t *random_position = (position_t *) malloc(sizeof(position_t));
                    random_position->x = i;
                    random_position->y = random_number(i, board->width - 2);
                    if(!((random_position->x == board->source->x && random_position->y == board->source->y) ||
                            (random_position->x == board->destination->x && random_position->y == board->destination->y)))
                            {
                                board->cell[random_position->y][random_position->x].type = Wall;
                            }
                }
            }
        }
    }
    else // read for edit
    {
        fread(&board->width, sizeof(int), 1, in_file->file);
        // g_print("%d\n", board->width);
        fread(&board->height, sizeof(int), 1, in_file->file);
        // g_print("%d\n", board->height);
        board->source = (position_t *) malloc(sizeof(position_t));
        fread(board->source, sizeof(position_t), 1, in_file->file);
        // g_print("%d, %d\n", board->source->x, board->source->y);
        board->destination = (position_t *) malloc(sizeof(position_t));
        fread(board->destination, sizeof(position_t), 1, in_file->file);
        // g_print("%d, %d\n", board->destination->x, board->destination->y);
        board->cell = (cell_t **) malloc(sizeof(cell_t *) * board->height);
        
        for(int i = 0; i < board->height; ++i)
        {
            board->cell[i] = (cell_t *) malloc(sizeof(cell_t) * board->width);
            for(int j = 0; j < board->width; ++j)
            {
                board->cell[i][j].position = (position_t *) malloc(sizeof(position_t));
                fread(&board->cell[i][j].type, sizeof(type_t), 1, in_file->file);
                // g_print("%s\n", board->cell[i][j].type == Wall ? "wall" : "free");
                fread(board->cell[i][j].position, sizeof(position_t), 1, in_file->file);
                // g_print("%d, %d\n", board->cell[i][j].position->x, board->cell[i][j].position->y);
            }
        }
    }
    
    argc = 0;

    GtkApplication *app;
    int status;

    buttons = (GtkWidget **)malloc((board->width * board->height + 2) * sizeof(GtkWidget *));
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), board);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}