#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <unistd.h>

#include "pathfinder.h"
#include "gui.h"



maze_t *create_maze(edge_t *list, int list_size, board_t *board)
{
    maze_t *maze = (maze_t *) malloc(sizeof(maze_t));
    maze->head = (node_t **) malloc(board->width * board->height * sizeof(node_t *));
    maze->position = (position_t **) malloc(board->width * board->height * sizeof(position_t *));
    maze->visit = (visit_e *) malloc(board->width * board->height * sizeof(visit_e));
    maze->block = (type_t *) malloc(board->width * board->height * sizeof(type_t));
    maze->width = board->width;
    
    for(int i = 0; i < board->width * board->height; ++i)
    {
        maze->head[i] = NULL;
        maze->position[i] = board->cell[i / board->width][i % board->width].position;
        maze->visit[i] = Unvisited;
        
        if(board->cell[i / board->width][i % board->width].type == Wall)
            maze->block[i] = Wall;
        else
            maze->block[i] = Free;
    }
    edge_t *current_edge = (edge_t *) malloc(sizeof(edge_t));
    if(!current_edge) perror(NULL);

    for(int i = 0; i < list_size; ++i)
    {
        current_edge->source = list[i].source;
        current_edge->destination = list[i].destination;

        // src -> dest
        node_t *dest = (node_t *) malloc(sizeof(node_t));
        dest->position = current_edge->destination;
        dest->next = maze->head[current_edge->source->x + board->width * current_edge->source->y];
        maze->head[current_edge->source->x + board->width * current_edge->source->y] = dest;
        // src -> dest
        node_t *src = (node_t *) malloc(sizeof(node_t));
        src->position = current_edge->source;
        src->next = maze->head[current_edge->destination->x + board->width * current_edge->destination->y];
        maze->head[current_edge->destination->x + board->width * current_edge->destination->y] = src;
    }
    return maze;
}

void print_maze(maze_t *maze, int graph_size)
{
    for(int i = 0; i < graph_size; ++i)
    {
        printf("n(%d, %d): ", maze->position[i]->x, maze->position[i]->y);
        node_t *neighbors = maze->head[i];
        while(neighbors)
        {
            printf("(%d, %d), ", neighbors->position->x, neighbors->position->y);
            neighbors = neighbors->next;
        }
        printf("\n");
    }
}

node_t *get_node_by_direction(maze_t *maze, position_t *source, direction_e direction)
{
    node_t *current_head = maze->head[source->x + source->y * maze->width];
    while (current_head)
    {
        if(direction == Top)
        {
            if(current_head->position->x == source->x && current_head->position->y == source->y - 1) return current_head;
        }
        else if(direction == Bottom)
        {
            if(current_head->position->x == source->x && current_head->position->y == source->y + 1) return current_head;
        }
        else if(direction == Right)
        {
            if(current_head->position->x == source->x + 1 && current_head->position->y == source->y) return current_head;
        }
        else if(direction == Left)
        {
            if(current_head->position->x == source->x - 1 && current_head->position->y == source->y) return current_head;
        }
        current_head = current_head->next;
    }
    return NULL;
}

queue_t *create_queue()
{
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

int is_empty(queue_t *queue)
{
    if(queue->head == NULL) return 1;
    else return 0;
}

void enqueue(queue_t *queue, position_t *position)
{
    node_t *n = (node_t *) malloc(sizeof(node_t));
    n->position = position;
    n->next = NULL;

    if(queue->head == NULL)
    {
        queue->head = n;
        queue->tail = n;
        return;
    }
    queue->tail->next = n;
    queue->tail = n;
}

position_t *dequeue(queue_t *queue)
{
    node_t *node = queue->head;

    position_t *copy_postion = (position_t *) malloc(sizeof(position_t));
    copy_postion->x = node->position->x;
    copy_postion->y = node->position->y;
    // *copy_postion = *node->position; ??

    queue->head = queue->head->next;
    free(node);

    return copy_postion;
}

bool bfs(maze_t *maze, position_t *source, position_t *destination, int interval_delay, bool perform_statistic)
{
    static bool first_time = true;
    queue_t *queue = create_queue();
    enqueue(queue, source);
    maze->visit[source->x + maze->width * source->y] = Visited;

    position_t *current_position = 0;
    char number[15];
    while(!is_empty(queue))
    {
        current_position = dequeue(queue);
        // printf("n(%d, %d) visited\n", current_position->x, current_position->y);
        if(!perform_statistic)
        {
            sprintf(number, "%d", current_position->x + maze->width * current_position->y);
            if(!(current_position->x == destination->x && current_position->y == destination->y))
            {
                if(first_time == false)
                {
                    update_markup(buttons[current_position->x + maze->width * current_position->y], number);
                }
                else
                {
                    first_time = false;
                }
            }
            usleep(interval_delay * 1e3);
        }

        if(current_position->x == destination->x && current_position->y == destination->y) return false;
        // if(current_position == destination) return 0;

        node_t *top_node = get_node_by_direction(maze, current_position, Top);
        node_t *right_node = get_node_by_direction(maze, current_position, Right);
        node_t *bottom_node = get_node_by_direction(maze, current_position, Bottom);
        node_t *left_node = get_node_by_direction(maze, current_position, Left);
        if(top_node)
        {
            if(maze->block[top_node->position->x + maze->width * top_node->position->y] == Free && maze->visit[top_node->position->x + maze->width * top_node->position->y] == Unvisited)
            {
                enqueue(queue, top_node->position);
                maze->visit[top_node->position->x + maze->width * top_node->position->y] = Visited;
            }
        }
        if(right_node)
        {
            if(maze->block[right_node->position->x + maze->width * right_node->position->y] == Free && maze->visit[right_node->position->x + maze->width * right_node->position->y] == Unvisited)
            {
                enqueue(queue, right_node->position);
                maze->visit[right_node->position->x + maze->width * right_node->position->y] = Visited;
            }
        }
        if(bottom_node)
        {
            if(maze->block[bottom_node->position->x + maze->width * bottom_node->position->y] == Free && maze->visit[bottom_node->position->x + maze->width * bottom_node->position->y] == Unvisited)
            {
                enqueue(queue, bottom_node->position);
                maze->visit[bottom_node->position->x + maze->width * bottom_node->position->y] = Visited;
            }
        }
        if(left_node)
        {
            if(maze->block[left_node->position->x + maze->width * left_node->position->y] == Free && maze->visit[left_node->position->x + maze->width * left_node->position->y] == Unvisited)
            {
                enqueue(queue, left_node->position);
                maze->visit[left_node->position->x + maze->width * left_node->position->y] = Visited;
            }
        }
    }
    return true;
}


bool dfs(maze_t *maze, position_t *source, position_t *destination, int interval_delay, bool perform_statistic)
{
    static bool success = 0;
    static bool first_time = true;
    node_t *neighbors = maze->head[source->x + source->y * maze->width];
    
    maze->visit[source->x + source->y * maze->width] = Visited;
    // printf("n(%d, %d) visited\n", source->x, source->y);
    
    if(!perform_statistic)
    {
        char number[15];
        sprintf(number, "%d", source->x + maze->width * source->y);
        if(!(source->x == destination->x && source->y == destination->y))
        {
            if(first_time == false)
            {
                update_markup(buttons[source->x + maze->width * source->y], number);
            }
            else
            {
                first_time = false;
            }
        }
        usleep(interval_delay * 1e3);
    }

    if(source->x == destination->x && source->y == destination->y)
        success = 1;

    node_t *top_node = get_node_by_direction(maze, source, Top);
    node_t *right_node = get_node_by_direction(maze, source, Right);
    node_t *bottom_node = get_node_by_direction(maze, source, Bottom);
    node_t *left_node = get_node_by_direction(maze, source, Left);
    // clock wise order
    if(top_node)
        if(maze->block[top_node->position->x + maze->width * top_node->position->y] == Free && maze->visit[top_node->position->x + maze->width * top_node->position->y] == Unvisited && success != true)
            dfs(maze, top_node->position, destination, interval_delay, perform_statistic);
    if(right_node)
        if(maze->block[right_node->position->x + maze->width * right_node->position->y] == Free && maze->visit[right_node->position->x + maze->width * right_node->position->y] == Unvisited && success != true)
            dfs(maze, right_node->position, destination, interval_delay, perform_statistic);
    if(bottom_node)
        if(maze->block[bottom_node->position->x + maze->width * bottom_node->position->y] == Free && maze->visit[bottom_node->position->x + maze->width * bottom_node->position->y] == Unvisited && success != true)
            dfs(maze, bottom_node->position, destination, interval_delay, perform_statistic);
    if(left_node)
        if(maze->block[left_node->position->x + maze->width * left_node->position->y] == Free && maze->visit[left_node->position->x + maze->width * left_node->position->y] == Unvisited && success != true)
            dfs(maze, left_node->position, destination, interval_delay, perform_statistic);
    if(success != true) return 1;
    else return 0;
}

board_t *read_from_file(FILE *file_ptr, edge_t **edge_list_ptr, int *edge_list_size)
{
    board_t *board = (board_t *) malloc(sizeof(board_t));
    board->cell = (cell_t **) malloc(sizeof(cell_t *) * board->height);

    fread(&board->width, sizeof(int), 1, file_ptr);
    // printf("%d\n", board->width);
    fread(&board->height, sizeof(int), 1, file_ptr);
    // printf("%d\n", board->height);
    board->source = (position_t *) malloc(sizeof(position_t));
    fread(board->source, sizeof(position_t), 1, file_ptr);
    // printf("%d, %d\n", board->source->x, board->source->y);
    board->destination = (position_t *) malloc(sizeof(position_t));
    fread(board->destination, sizeof(position_t), 1, file_ptr);
    // printf("%d, %d\n", board->destination->x, board->destination->y);
    board->cell = (cell_t **) malloc(sizeof(cell_t *) * board->height);

    for(int i = 0; i < board->height; ++i)
    {
        board->cell[i] = (cell_t *) malloc(sizeof(cell_t) * board->width);
        for(int j = 0; j < board->width; ++j)
        {
            board->cell[i][j].position = (position_t *) malloc(sizeof(position_t));
            fread(&board->cell[i][j].type, sizeof(type_t), 1, file_ptr);
            // printf("%s\n", board->cell[i][j].type == Wall ? "wall" : "free");
            fread(board->cell[i][j].position, sizeof(position_t), 1, file_ptr);
            // printf("%d, %d\n", board->cell[i][j].position->x, board->cell[i][j].position->y);
        }
    }


    *edge_list_size = 2 * board->height * board->width - (board->height + board->width);
    edge_t *edge_list = (edge_t *) malloc(sizeof(edge_t) * *edge_list_size);

    int edge_i;
    for(int i = 0; i < board->height; ++i)
    {
        for(int j = 0; j < board->width; ++j)
        {
            if(j == board->width - 1 && i == board->height - 1) continue;
            else if(j == board->width - 1) // add bottom cell
            {
                edge_list[edge_i].source = board->cell[i][j].position;
                edge_list[edge_i].destination = board->cell[i+1][j].position;
                edge_i++;
            }
            else if(i == board->height - 1) // add right cell
            {
                edge_list[edge_i].source = board->cell[i][j].position;
                edge_list[edge_i].destination = board->cell[i][j+1].position;
                edge_i++;
            }
            else
            {
                edge_list[edge_i].source = board->cell[i][j].position;
                edge_list[edge_i].destination = board->cell[i][j+1].position;
                edge_i++;
                edge_list[edge_i].source = board->cell[i][j].position;
                edge_list[edge_i].destination = board->cell[i+1][j].position;
                edge_i++;
            }   
        }
    }

    *edge_list_ptr = edge_list;

    return board;
}