#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#include "gui.h"
#include "pathfinder.h"
#include "designer.h"

static error_t parse_opt (int key, char *arg, struct argp_state *state);

struct arguments
{
  char *filename;
  int algorithm; // 0 for dfs, 1 for bfs, 2 for a-star
  bool perform_statistic;
//   bool output_results;
//   char *outfile;
//   int repeat_count;
  int interval_delay;
  bool run;
};

static struct argp_option options[] =
{
  {"input", 'i', "FILENAME", 0, "Input maze design file"},

  {"dfs", 'D', 0, 0, "Use DFS path finding algorithm"},
  {"bfs", 'B', 0, 0, "Use BFS path finding algorithm"},
  {"a-star", 'A', 0, 0, "Use A-Star path finding algorithm"},
  
  {"delay-ms", 't', "TIME", 0, "Delay between intervals (100 ms default)"},

  {"perform-statistic", 'p', 0, 0, "Perform statisctic"},
  // will be done in bash!
//   {"output", 'o', "FILENAME", 0, "Output file in case of performing statisctics"},
//   {"repeat-count", 'r', "NUM", 0, "Number of repeats in case of performing statisctics"},
  {0}
};

static char args_doc[] = "run";
static char doc[] = "path finder -- a visualizing tool for path finding algorithms on a maze";
const char *argp_program_version = "path finder (beta)";
const char *argp_program_bug_address = "<dark.c343@gmail.com>";

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv)
{
    struct arguments arguments = { NULL, -1, false, -1, false};
    file_t in_file;

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

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

    int edge_list_size = 0;
    edge_t *edge_list = NULL;

    board_t *board = read_from_file(in_file.file, &edge_list, &edge_list_size);
    maze_t *maze = create_maze(edge_list, edge_list_size, board);

    return play_gui(arguments.algorithm, arguments.interval_delay, arguments.perform_statistic, arguments.filename, maze, board);
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
        case 'i':
            arguments->filename = arg;
            break;
        case 'D':
            arguments->algorithm = 0;
            break;
        case 'B':
            arguments->algorithm = 1;
            break;
        case 'A':
            arguments->algorithm = 2;
            break;
        case 't':
            arguments->interval_delay = atoi(arg);
            break;
        case 'p':
            arguments->perform_statistic = true;
            break;
        // case 'o':
        //     arguments->outfile = arg;
        //     break;
        // case 'r':
        //     arguments->repeat_count = atoi(arg);
        //     break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
            {
                argp_usage(state);
            }
            if(strcmp(arg, "run") == 0) arguments->run = true;
            else arguments->run = false;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
            {
                argp_usage (state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
