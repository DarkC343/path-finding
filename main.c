#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "gui.h"
#include "pathfinder.h"
#include "designer.h"

struct arguments
{
  char *filename;
  int algorithm; // 0 for dfs, 1 for bfs, 2 for a-star
  bool perform_statistic;
  int interval_delay;
  bool run;
};

int main(int argc, char **argv)
{
    if(argc == 1 || argc == 2)
    {
        if(argc == 2 && strcmp(argv[1], "--help") == 0)
            fprintf(stdout, "Usage: %s [OPTION...] run\npath finder -- a visualizing tool for path finding algorithms on a maze\n\n -A      Use A-Star path finding algorithm\n -B      Use BFS path finding algorithm\n -D      Use DFS path finding algorithm\n -i      Input maze design file\n -p      Perform statisctic\n -t      Delay between intervals (100 ms default)\n\n --usage      Give a short usage message\n", argv[0]);
        else if(argc == 2 && strcmp(argv[1], "--usage") == 0)
            fprintf(stdout, "Usage: %s [OPTION...] run\n", argv[0]);
        else
            fprintf(stdout, "Usage: %s [OPTION...] run\nTry `%s --help' or `%s --usage' for more information.\n", argv[0], argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    struct arguments arguments = { NULL, -1, false, -1, false};
    file_t in_file;

    opterr = 0;
    int c;
    int index;
    while ((c = getopt (argc, argv, "i:DBAt:p")) != -1)
        switch (c)
            {
            case 'i':
                arguments.filename = optarg;
                break;
            case 'D':
                arguments.algorithm = 0;
                break;
            case 'B':
                arguments.algorithm = 1;
                break;
            case 'A':
                arguments.algorithm = 2;
                break;
            case 't':
                arguments.interval_delay = atoi(optarg);
                break;
            case 'p':
                arguments.perform_statistic = true;
                break;
            case '?':
                if (optopt == 'i')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 't')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort ();
            }

    for (index = optind; index < argc; index++)
        if(strcmp(argv[index], "run") == 0) arguments.run = true;

    if (arguments.filename)
    {
        if(access(arguments.filename, F_OK) != -1)
        {
            in_file.file = fopen(arguments.filename, "rb");
            in_file.filename = arguments.filename;
        }
        else
        {
            fprintf(stderr, "err: file does not exist\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        fprintf(stderr, "err: no input file selected\n");
        return EXIT_FAILURE;
    }

    if(!arguments.perform_statistic)
    {
        if(arguments.interval_delay == -1)
        {
            arguments.interval_delay = 100;
        }
        else if(arguments.interval_delay < 0)
        {
            fprintf(stderr, "err: interval delay can not be a negative number (set to 0)\n");
        }
    }

    if(arguments.algorithm == -1)
    {
        fprintf(stderr, "err: algorithm should be set. see `%s --help`\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(arguments.run == false)
    {
        fprintf(stdout, "Usage: %s [OPTION...] run\n", argv[0]);
        return EXIT_FAILURE;
    }

    int edge_list_size = 0;
    edge_t *edge_list = NULL;

    board_t *board = read_from_file(in_file.file, &edge_list, &edge_list_size);
    maze_t *maze = create_maze(edge_list, edge_list_size, board);

    return play_gui(arguments.algorithm, arguments.interval_delay, arguments.perform_statistic, arguments.filename, maze, board);
}