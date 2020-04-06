#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "designer.h"

enum Visit {
    Visited,
    Unvisited
};
typedef enum Visit visit_e;

struct Node {
    position_t *position;
    // int value;
    struct Node *next;
};
typedef struct Node node_t;


struct Edge {
    position_t *source;
    position_t *destination;
};
typedef struct Edge edge_t;

struct Graph {
    position_t **position;
    type_t *block;
    visit_e *visit;
    node_t **head;
    int width;
};
typedef struct Graph maze_t;

struct Queue {
    node_t *head;
    node_t *tail;
};
typedef struct Queue queue_t;

enum Direction {
    Top,
    Right,
    Bottom,
    Left
};
typedef enum Direction direction_e;

maze_t *create_maze(edge_t *list, int list_size, board_t *board);
void print_maze(maze_t *maze, int maze_size);
node_t *get_node_by_direction(maze_t *maze, position_t *source, direction_e direction);
queue_t *create_queue();
int is_empty(queue_t *queue);
void enqueue(queue_t *queue, position_t *position);
position_t *dequeue(queue_t *queue);
bool bfs(maze_t *maze, position_t *source, position_t *destination, int interval_delay, bool perform_statistic);
bool dfs(maze_t *maze, position_t *source, position_t *destination, int interval_delay, bool perform_statistic);
board_t *read_from_file(FILE *file_ptr, edge_t **edge_list_ptr, int *edge_list_size);

#endif