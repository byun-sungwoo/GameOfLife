#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ncurses.h>
#include "cell.h"
#define DRAW_CHAR '+'

cell_t * create_cell (int init_x, int init_y)
{
	// create the cell
	cell_t * c = malloc(sizeof(cell_t));
	c->x = init_x;
	c->y = init_y;
	c->draw_char = DRAW_CHAR;

	c->state[OLD] = UNKNOWN;
	c->state[CURRENT] = DIE;
	c->state[NEW] = UNKNOWN;

	return(c);
}

int destroy_cell (cell_t * c)
{
	free(c);

	return SUCCESS;
}

int update_cell (cell_t * c)
{
	c->state[OLD] = c->state[CURRENT];
	c->state[CURRENT] = c->state[NEW];
	c->state[NEW] = UNKNOWN;

	return SUCCESS;
}

// updates all of the cell states
void update_cells (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int x, y;
	border /= 2;
	for (x = 0; x < c_size_x; x++)
		for (y = 0; y < c_size_y; y++)
			if(x >= border-1 && x <= c_size_x - border && y >= border-1 && y <= c_size_y - border)
				update_cell(c[x][y]);
}

void print_cell (cell_t * c)
{
	printf("cell->x: %d\t cell->y: %d\t cell->draw_char %c\n", c->x, c->y, c->draw_char);
	printf("cell->state[0]: %d\t cell->state[1]: %d\t cell->state[2]: %d\n",
		c->state[OLD], c->state[CURRENT], c->state[NEW]);
}

void display_cell (cell_t * c)
{
	if(c->state[CURRENT] == LIVE)
	{
		init_pair(7, COLOR_BLACK, COLOR_GREEN);
		attron(COLOR_PAIR(7));
		mvprintw(c->y, c->x, "%c", c->draw_char);
	}
	else
	{
		init_pair(20, COLOR_WHITE, COLOR_WHITE);
		attron(COLOR_PAIR(20));
		mvprintw(c->y, c->x, " ");
	}
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(2));
}

// display all the cells
void display_cells (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int x, y;
	border /= 2;
	for (x = 0; x < c_size_x; x++)
		for (y = 0; y < c_size_y; y++)
		{
			if(x >= border-1 && x <= c_size_x - border && y >= border-1 && y <= c_size_y - border)
				display_cell(c[x][y]);
			else
				undisplay_cell(c[x][y]);
		}
}

// updates the NEW state in a given cell
void update_cell_state (cell_t * c, int neighbours)
{
	if (c->state[CURRENT] == LIVE)
		if (neighbours == 2 || neighbours == 3)
			c->state[NEW] = LIVE;
		else
			c->state[NEW] = DIE;
	else
	{
		if (neighbours == 3)
			c->state[NEW] = LIVE;
		else
			c->state[NEW] = DIE;
	}
}

void undisplay_cell (cell_t * c)
{
	mvprintw(c->y, c->x, " ");
}

// count neighbours of each cell and update the state
void update_neighbours (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int neighbours;
	border /= 2;
	int x, y;
	for (x = 0; x < c_size_x; x++)
	{
		for (y = 0; y < c_size_y; y++)
		{
			if(x >= border-1 && x <= c_size_x - border && y >= border-1 && y <= c_size_y - border)
			{
				neighbours = 0;
				// Left column
				if (x - 1 >= border-1)
				{
					if (y - 1 >= border-1)
						neighbours += c[x-1][y-1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[x-1][c_size_y-border]->state[CURRENT] == LIVE ? 1 : 0;
					neighbours += c[x-1][y]->state[CURRENT] == LIVE ? 1 : 0;
					if (y <= c_size_y - border-1)
						neighbours += c[x-1][y+1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[x-1][border-1]->state[CURRENT] == LIVE ? 1 : 0;
				}
				else
				{
					if (y - 1 >= border-1)
						neighbours += c[c_size_x-border][y-1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[c_size_x-border][c_size_y-border]->state[CURRENT] == LIVE ? 1 : 0;
					neighbours += c[c_size_x-border][y]->state[CURRENT] == LIVE ? 1 : 0;
					if (y <= c_size_y - border-1)
						neighbours += c[c_size_x-border][y+1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[c_size_x-border][border-1]->state[CURRENT] == LIVE ? 1 : 0;
				}
				// Middle Column
				if(y - 1 >= border-1)
					neighbours += c[x][y-1]->state[CURRENT] == LIVE ? 1 : 0;
				else
					neighbours += c[x][c_size_y-border]->state[CURRENT] == LIVE ? 1 : 0;
				if(y <= c_size_y - border-1)
					neighbours += c[x][y+1]->state[CURRENT] == LIVE ? 1 : 0;
				else
					neighbours += c[x][border-1]->state[CURRENT] == LIVE ? 1 : 0;
				// Right Column
				if (x <= c_size_x - border-1)
				{
					if(y - 1 >= border-1)
						neighbours += c[x+1][y-1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[x+1][c_size_y-border]->state[CURRENT] == LIVE ? 1 : 0;
					neighbours += c[x+1][y]->state[CURRENT] == LIVE ? 1 : 0;
					if(y <= c_size_y - border-1)
						neighbours += c[x+1][y+1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[x+1][border-1]->state[CURRENT] == LIVE ? 1 : 0;
				}
				else
				{
					if(y - 1 >= border-1)
						neighbours += c[border-1][y-1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[border-1][c_size_y-border]->state[CURRENT] == LIVE ? 1 : 0;
					neighbours += c[border-1][y]->state[CURRENT] == LIVE ? 1 : 0;
					if(y <= c_size_y - border-1)
						neighbours += c[border-1][y+1]->state[CURRENT] == LIVE ? 1 : 0;
					else
						neighbours += c[border-1][border-1]->state[CURRENT] == LIVE ? 1 : 0;
				}
				update_cell_state(c[x][y], neighbours);
			}
		}
	}
}

