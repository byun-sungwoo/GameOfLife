#include <stdlib.h>
#include <ncurses.h>
#include "well.h"

well_t *init_well(int upper_left_x, int upper_left_y, int width, int height)
{
	well_t *w;
	w = malloc(sizeof(well_t));
	w->upper_left_x = upper_left_x;
	w->upper_left_y = upper_left_y;
	w->width = width;
	w->height = height;
	w->draw_char = '#';
	w->color[0] = 0;
	w->color[1] = 0;
	w->color[2] = 255;
	return (w);
}

void draw_well(well_t *w)
{
	int row_counter, column_counter;

	// Draw Top of well
	for (row_counter = w->upper_left_x;
			row_counter <= (w->upper_left_x + w->width + 2);
			row_counter++) {
		mvprintw(w->upper_left_y, row_counter, "%c", w->draw_char);
	}

	// Draw left side of well
	for (column_counter = w->upper_left_y;
			column_counter <= (w->upper_left_y + w->height + 2);
			column_counter++) {
		mvprintw(column_counter, w->upper_left_x, "%c", w->draw_char);
	}

	// Draw right side of well
	for (column_counter = w->upper_left_y;
			column_counter <= (w->upper_left_y + w->height + 2);
			column_counter++) {
		mvprintw(column_counter, (w->upper_left_x + w->width + 2), "%c", w->draw_char);
	}

	// Draw Bottom of well
	for (row_counter = w->upper_left_x;
			row_counter <= (w->upper_left_x + w->width + 2);
			row_counter++) {
		mvprintw(w->upper_left_y + w->height + 2, row_counter, "%c", w->draw_char);
	}
}

well_t *changeWellSize(int upper_left_x, int upper_left_y, int width, int height, well_t *w)
{
	w->upper_left_x = upper_left_x;
	w->upper_left_y = upper_left_y;

	if (width < 10)
		w->width = 10;
	else
		w->width = width;

	if (height < 10)
		w->height = 10;
	else
		w->height = height;

	return (w);
}

void undraw_well(well_t *w)
{
	int row_counter, column_counter;

	// Undraw Top of well
	for (row_counter = w->upper_left_x;
			row_counter <= (w->upper_left_x + w->width);
			row_counter++) {
		mvprintw(w->upper_left_y, row_counter, " ");
	}

	// Undraw left side of well
	for (column_counter = w->upper_left_y;
			column_counter<=(w->upper_left_y + w->height);
			column_counter++) {
		mvprintw(column_counter, w->upper_left_x, " ");
	}

	// Undraw right side of well
	for (column_counter = w->upper_left_y;
			column_counter <= (w->upper_left_y + w->height);
			column_counter++) {
		mvprintw(column_counter, (w->upper_left_x + w->width), " ");
	}

	// Undraw Bottom of well
	for (row_counter = w->upper_left_x;
			row_counter <= (w->upper_left_x + w->width);
			row_counter++) {
		mvprintw(w->upper_left_y + w->height, row_counter, " ");
	}
}

