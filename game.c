#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include "cell.h"
#include "game.h"
#include "well.h"
#include "key.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_game(void)
{
	int x,y;
}

/**
  Print menu options
  */
void display_game_options(well_t *w)
{
	mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "[P] to pause");
	mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "[Q] to quit");
	mvprintw(w->upper_left_y + 2, w->upper_left_x - 25, "[R] to reset");
	mvprintw(w->upper_left_y + 3, w->upper_left_x - 25, "[C] to set frequency");
	mvprintw(w->upper_left_y + 4, w->upper_left_x - 25, "[L] to load .life file");
	mvprintw(w->upper_left_y + 5, w->upper_left_x - 25, "[B] for C291 <Kills All>");
	mvprintw(w->upper_left_y + 6, w->upper_left_x - 25, "[*] for frequency * 1.2");
	mvprintw(w->upper_left_y + 7, w->upper_left_x - 25, "[/] for frequency / 1.2");
	mvprintw(w->upper_left_y + 8, w->upper_left_x - 25, "[-] for board size - 2");
	mvprintw(w->upper_left_y + 9, w->upper_left_x - 25, "[+] for board size + 2");
	mvprintw(w->upper_left_y + 10, w->upper_left_x - 25, "[S] to save game");
}

/**
  Remove menu options
  */
void clear_game_options(well_t *w)
{
	int i;
	for(i = 0; i < 11; i++)
		mvprintw(w->upper_left_y + i, w->upper_left_x - 25, "                        ");
}

/**
  Prompt user for frequncy
  */
void display_c(int *freq, well_t *w)
{
	nodelay(stdscr, FALSE);
	echo();

	clear_game_options(w);
	int shift = 4;

	mvprintw(w->upper_left_y + 1, w->upper_left_x + w->width + shift, "<Select Value From %d - %d>", MIN_FREQ, MAX_FREQ);
	curs_set(1);
	do
	{
		mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "                           ");
		mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "Input Frequency : ");
		scanw(" %d", freq);
		if(!(*freq >= MIN_FREQ && *freq <= MAX_FREQ))
			mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "<Please Select Value In Range>");
	}
	while(!(*freq >= MIN_FREQ && *freq <= MAX_FREQ));
	curs_set(0);
	mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "                                    ");
	mvprintw(w->upper_left_y + 1, w->upper_left_x + w->width + shift, "                                    ");
	mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "                                    ");

	nodelay(stdscr, TRUE);
	noecho();
}

/**
  Print frequncy refresh rate and current countdown until next refresh.
  */
void display_frequency(int timeout, int counter, well_t *w)
{
	mvprintw(w->upper_left_y - 3, w->upper_left_x, "Game Refresh Frequency        : %d\n", timeout);
	mvprintw(w->upper_left_y - 2, w->upper_left_x, "Millisecond until next update : %d\n", timeout - counter);
}

/**
  Print end screen
  */
void display_end(well_t *w, int border)
{
	int shift = WELL_WIDTH/2 - 18 + (14-border)/2;
	mvprintw(w->upper_left_y + 5, w->upper_left_x + shift, "<Press any key to exit>");
	mvprintw(w->upper_left_y + 7, w->upper_left_x + shift - 3, "All life has been terminated.");
	mvprintw(w->upper_left_y + 8, w->upper_left_x + shift - 3, "Why did you have to do this?...");
}

/**
  Places an oscillator at x,y with respect to the border in c
  If any cell will be out of bounds of c's actual size, it will not be placed.
  */
void place_oscillator(int x, int y, int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int coord_x[] = {0, 1, 2};
	int coord_y[] = {0, 0, 0};

	int array_size = sizeof(coord_x)/sizeof(int);
	x += border/2;
	y += border/2;
	x--;
	y--;

	int i;
	for(i = 0; i < array_size; i++)
	{
		if(x >= 0 && x+coord_x[i] < c_size_x && y >= 0 && y+coord_y[i] < c_size_y)
			c[x+coord_x[i]][y+coord_y[i]]->state[CURRENT] = LIVE;
	}
}

/**
  Places a glider at x,y with respect to the border in c
  If any cell will be out of bounds of c's actual size, it will not be placed.
  */
void place_glider(int x, int y, int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int coord_x[] = {2, 0, 2, 1, 2};
	int coord_y[] = {0, 1, 1, 2, 2};

	int array_size = sizeof(coord_x)/sizeof(int);
	x += border/2;
	y += border/2;
	x--;
	y--;

	int i;
	for(i = 0; i < array_size; i++)
	{
		if(x >= 0 && x+coord_x[i] < c_size_x && y >= 0 && y+coord_y[i] < c_size_y)
			c[x+coord_x[i]][y+coord_y[i]]->state[CURRENT] = LIVE;
	}
}

/**
  Places a large oscillator at x,y with respect to the border in c
  If any cell will be out of bounds of c's actual size, it will not be placed.
  */
void place_random(int x, int y, int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	// int coord_x[] = {2, 7, 0, 1, 3, 4, 5, 6, 8, 9, 2, 7};
	// int coord_y[] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};

	// int coord_x[] = {0, 1, 0, 1, 6, 7, 6, 7, 6, 7, 8, 8, 10, 10, 10, 10, 20, 21, 20, 21};
	// int coord_y[] = {3, 3, 4, 4, 2, 2, 3, 3, 4, 4, 1, 5,  0,  1,  5,  6,  3,  3,  4,  4};

	int coord_x[] = {0, 1, 2, 0, 1, 2, 0, 1, 2};	// 3x3 grid
	int coord_y[] = {0, 0, 0, 1, 1, 1, 2, 2, 2};

	int array_size = sizeof(coord_x)/sizeof(int);
	x += border/2;
	y += border/2;
	x--;
	y--;

	srand(time(NULL));

	int i;
	for(i = 0; i < array_size; i++)
	{
		if(x >= 0 && x+coord_x[i] < c_size_x && y >= 0 && y+coord_y[i] < c_size_y && rand()%10 >= 5)	// 50% chance
			c[x+coord_x[i]][y+coord_y[i]]->state[CURRENT] = LIVE;
	}
}

/**
  Places a large oscillator at x,y with respect to the border in c
  If any cell will be out of bounds of c's actual size, it will not be placed.
  */
void place_C291(int x, int y, int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border)
{
	int coord_x[] = {	1, 2, 3, 6, 7, 11, 12, 17,
		0, 5, 8, 10, 13, 16, 17,
		0, 8, 10, 13, 17,
		0, 7, 11, 12, 13, 17,
		0, 6, 13, 17,
		1, 2, 3, 5, 6, 7, 8, 13, 16, 17, 18};
	int coord_y[] = {	0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

	int array_size = sizeof(coord_x)/sizeof(int);
	x += border/2;
	y += border/2;
	x--;
	y--;

	int i, j;
	for(i = 0; i < WELL_WIDTH-1; i++)
		for(j = 0; j < WELL_HEIGHT-1; j++)
			c[i][j]->state[CURRENT] = DIE;
	for(i = 0; i < array_size; i++)
	{
		if(x >= 0 && x+coord_x[i] < c_size_x && y >= 0 && y+coord_y[i] < c_size_y)
			c[x+coord_x[i]][y+coord_y[i]]->state[CURRENT] = LIVE;
	}
}

/**
  Saves the current state of the cell_t pointer to a file.
  Can only save up to 10 files. If user attempts to save
  another when there is already 10, the user will be notified
  with a message saying there is no more room.
  */
void save_cells(int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border, well_t *w)
{
	FILE *fp;

	char *filePath = "saves/";
	char *fileName = "save_";
	char *fileType = ".life";
	char *openFile = malloc(sizeof(char)*20);

	int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
	int shifty = WELL_HEIGHT + (14-border)/2 - 5;

	int saved = 0;

	int n;
	for(n = 0; n < 10; n++)
	{
		char *num = malloc(sizeof(char)*2);
		sprintf(num, "%d", n+1);

		strcpy(openFile, filePath);
		strcat(openFile, fileName);
		strcat(openFile, num);
		strcat(openFile, fileType);

		if(fopen(openFile, "r") == NULL)
		{
			fp = fopen(openFile, "w");
			fprintf(fp, "%d\n", border);

			int i, j;
			for(i = 0; i < c_size_y; i++)
			{
				for(j = 0; j < c_size_x; j++)
				{
					// fprintf(fp, "%d", c[i][j]->state[CURRENT] == LIVE ? 1 : 0);
					if(c[j][i]->state[CURRENT] == LIVE)
						fprintf(fp, "@");
					else
						fprintf(fp, "-");
				}
				fprintf(fp, "\n");
			}
			mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
			mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-2, "-saved game to %d-", n+1);
			saved = 1;

			fclose(fp);
			break;
		}
	}
	if(!saved)	// No room means overwrite
	{
		mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-2, "                         ");
		// mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx, " -no space- ");
		nodelay(stdscr, FALSE);
		echo();

		clear_game_options(w);
		int file_num;
		int shift = 4;

		mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "<No More Space>");
		mvprintw(w->upper_left_y + 1, w->upper_left_x + w->width + shift, "<Select File From 1 - 10 To Overwrite>");
		curs_set(1);
		do
		{
			mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "                           ");
			mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "Input File Number : ");
			scanw(" %d", &file_num);
			if(!(file_num >= 1 && file_num <= 10))
				mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "<Please Select Value In Range>");
		}
		while(!(file_num >= 1 && file_num <= 10));
		curs_set(0);
		mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "                                      ");
		mvprintw(w->upper_left_y + 1, w->upper_left_x + w->width + shift, "                                      ");
		mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "                                      ");

		nodelay(stdscr, TRUE);
		noecho();

		// Overwrite the specified file.
		char *num = malloc(sizeof(char)*2);
		sprintf(num, "%d", file_num);
		strcpy(openFile, filePath);
		strcat(openFile, fileName);
		strcat(openFile, num);
		strcat(openFile, fileType);

		fp = fopen(openFile, "w");
		fprintf(fp, "%d\n", border);

		int i, j;
		for(i = 0; i < c_size_y; i++)
		{
			for(j = 0; j < c_size_x; j++)
			{
				// fprintf(fp, "%d", c[i][j]->state[CURRENT] == LIVE ? 1 : 0);
				if(c[j][i]->state[CURRENT] == LIVE)
					fprintf(fp, "@");
				else
					fprintf(fp, "-");
			}
			fprintf(fp, "\n");
		}
		mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
		mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx -2, "-saved game to %d-", file_num);
		saved = 1;
		fclose(fp);
	}
}

void print_pause_menu(well_t *w, int selector)
{
	init_pair(10, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(11, COLOR_YELLOW, COLOR_BLACK);

	if(selector == 0)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "Unpause");

	if(selector == 1)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "Quit");

	if(selector == 2)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 2, w->upper_left_x - 25, "Reset");

	if(selector == 3)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 3, w->upper_left_x - 25, "Place Shape");

	if(selector == 4)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 4, w->upper_left_x - 25, "Help Menu");

	if(selector == 5)
		attron(COLOR_PAIR(10));
	else
		attron(COLOR_PAIR(11));
	mvprintw(w->upper_left_y + 5, w->upper_left_x - 25, "Load Save");

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));
}

print_files(well_t *w, int file_count, int inner_selector)
{
	init_pair(10, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(11, COLOR_YELLOW, COLOR_BLACK);

	char *fileName = "save_";
	char *fileType = ".life";
	char *openFile = malloc(sizeof(char)*20);
	int n;
	for(n = 0; n < 11; n++)
	{
		char *num = malloc(sizeof(char)*2);
		sprintf(num, "%d", n);

		strcpy(openFile, fileName);
		strcat(openFile, num);
		strcat(openFile, fileType);

		if(inner_selector == n)
			attron(COLOR_PAIR(10));
		else
			attron(COLOR_PAIR(11));
		if(file_count+1 > n)
		{
			if(n == 0)
				mvprintw(w->upper_left_y + n, w->upper_left_x - 25, "current run");
			else
				mvprintw(w->upper_left_y + n, w->upper_left_x - 25, openFile);
		}
	}

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));
}

//void import_file(char *fileName)
int import_file(char *fileName, int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y])
{	// 55 by 29
	int x_counter = 0;
	int y_counter = 0;
	int line = 0;
	int fileBorder = 0;
	FILE *fp = fopen(fileName, "r");
	char ch = fgetc(fp);
	while (ch != EOF)
	{
		if(line == 0)
		{
			if(ch == 10)
				line = 1;
			else
			{
				if(fileBorder == 0)
					fileBorder += ch-48;
				else
				{
					fileBorder *= 10;
					fileBorder += ch-48;
				}
			}
		}
		else
		{
			if(ch == '-')
				c[y_counter][x_counter]->state[CURRENT] = DIE;
			else if(ch == '@')
				c[y_counter][x_counter]->state[CURRENT] = LIVE;
			if(ch != '-' && ch != '@')
			{
				y_counter = 0;
				x_counter++;
				if(x_counter == c_size_x)
					break;
			}
			else
				y_counter++;
		}
		ch = fgetc(fp);
	}
	fclose(fp);
	return fileBorder;
}

int how_many_files()
{
	char *filePath = "saves/";
	char *fileName = "save_";
	char *fileType = ".life";
	char *openFile = malloc(sizeof(char)*20);

	int numCounter = 0;

	int n;
	for(n = 0; n < 10; n++)
	{
		char *num = malloc(sizeof(char)*2);
		sprintf(num, "%d", n+1);

		strcpy(openFile, filePath);
		strcat(openFile, fileName);
		strcat(openFile, num);
		strcat(openFile, fileType);

		if(fopen(openFile, "r") != NULL)
			numCounter++;
	}

	return numCounter;
}

/**
  Main function that runs the game.
  */
int game(void)
{
	static int state = INIT;
	struct timespec tim = {0,1000000};  // Each execution of while(1) is approximately 1mS
	struct timespec tim_ret;			// timespec is part of time.h

	cell_t * cells[WELL_WIDTH-1][WELL_HEIGHT-1];

	well_t *w;

	int x_offset, y_offset;
	int x_max, y_max;
	int arrow;
	int move_counter = 0;
	int move_timeout = BASE_TIMEOUT;
	int end_counter = 0;

	int test = 0;

	int border = 8;	// Range of 2-14. 2 is largest. 14 is smallest.
	int border_width = WELL_WIDTH - border;
	int border_height = WELL_HEIGHT - border;

	int ascii;

	int init_cells = 0;

	int save_msg = -1;
	int reset_msg = -1;
	int file_msg = -1;

	int init_msg = 0;

	int pause_switch = 0;

	int MESSAGE_TIME = 1000;

	while(1) {
		int input = read_escape(&ascii);
		switch(state) {
			case INIT:	// Initialize the game, only run one time
				curs_set(0);
				if(init_msg == 0)
				{
					mvprintw(w->upper_left_y + 2, w->upper_left_x + 5, "Game of Life");
					mvprintw(w->upper_left_y + 4, w->upper_left_x + 5, "Daniel Byun");
					mvprintw(w->upper_left_y + 6, w->upper_left_x + 5, "-I recommend you play on a large window-");
				}
				mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "[S] to start");
				mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "[K] to place shape");
				initscr();
				start_color();
				nodelay(stdscr, TRUE);			// Do not wait for characters using getch.
				noecho();						// Do not echo input characters
				getmaxyx(stdscr, y_max, x_max);	// Get the screen dimensions

				x_offset = (x_max / 2) - (border_width / 2);
				y_offset = (y_max / 2) - (border_height / 2);

				if(!init_cells)
				{
					int i, j;
					for(i = 0; i < (WELL_WIDTH-1); i++)
						for(j = 0; j < (WELL_HEIGHT-1); j++)
							cells[i][j] = create_cell(x_offset + i -3, y_offset + j -3);
					init_cells++;
				}

				w = init_well(x_offset - 1, y_offset - 1, border_width, border_height);
				draw_well(w);
				if(input == REGCHAR && toupper(ascii) == 'S')	// [S] Start
				{
					clear_game_options(w);
					state = STEP;
				}
				else if(input == REGCHAR && toupper(ascii) == 'K')	// [K] Place shape
				{
					init_msg = 1;
					clear_game_options(w);
					mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "[1] for oscillator");
					mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "[2] for glider");
					mvprintw(w->upper_left_y + 2, w->upper_left_x - 25, "[3] for random");
					mvprintw(w->upper_left_y + 3, w->upper_left_x - 25, "[4] for C291 <Kills All>");

					int shape;

					while(1)
					{
						input = read_escape(&ascii);
						if(input == REGCHAR)
						{
							if(ascii == '1')
							{
								shape = 1;
								break;
							}
							if(ascii == '2')
							{
								shape = 2;
								break;
							}
							if(ascii == '3')
							{
								shape = 3;
								break;
							}
							if(ascii == '4')
							{
								shape = 4;
								break;
							}
						}
					}
					clear_game_options(w);

					int shift = 4;

					if(shape == 1)
						mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected oscillator-");
					else if(shape == 2)
						mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected glider-");
					else if(shape == 3)
						mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected random-");
					else
						mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected C291-");

					int x, y;
					x = -1;
					y = -1;

					int tempBorder = border/2;

					mvprintw(w->upper_left_y + 3, w->upper_left_x + w->width + shift, "Coordinate in form x-y");
					mvprintw(w->upper_left_y + 4, w->upper_left_x + w->width + shift, "x from 0 to %d", WELL_WIDTH-1 - tempBorder - (tempBorder-1));
					mvprintw(w->upper_left_y + 5, w->upper_left_x + w->width + shift, "y from 0 to %d", WELL_HEIGHT-1 - tempBorder - (tempBorder-1));

					nodelay(stdscr, FALSE);
					echo();
					curs_set(1);
					while(true)
					{
						mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "                                   ");
						mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "Input Coordinate : ");
						scanw(" %d-%d", &x, &y);
						if(	x+(tempBorder-1) >= tempBorder-1 && x+(tempBorder-1) <= WELL_WIDTH-1 - tempBorder &&
								y+(tempBorder-1) >= tempBorder-1 && y+(tempBorder-1) <= WELL_HEIGHT-1 - tempBorder)
							break;
						mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "<Select Valid Coord>");
					}
					curs_set(0);
					mvprintw(w->upper_left_y + 8, w->upper_left_x + w->width + shift, "You picked %d and %d", x, y);

					nodelay(stdscr, TRUE);
					noecho();

					int num;
					for(num = 0; num <= 8; num++)
						mvprintw(w->upper_left_y + num, w->upper_left_x + w->width + shift, "                            ");

					switch(shape)
					{
						case 1	: place_oscillator(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);	break;
						case 2	: place_glider(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);		break;
						case 3	: place_random(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);		break;
						default	: place_C291(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);			break;
					}
					display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
					draw_well(w);
				}
				break;
			case STEP:
				if((input == REGCHAR && toupper(ascii) == 'P') || pause_switch)	// [P] Pause
				{
					int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
					int shifty = WELL_HEIGHT + (14-border)/2 - 5;
					mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
					if(pause_switch)
						pause_switch = 0;
					int selector = 0;	// From 0 - 5
					mvprintw(w->upper_left_y - 5, w->upper_left_x + (border_width*2)/5, "<Game Paused>");
					clear_game_options(w);
					int reset = 0;

					while(1)
					{
						print_pause_menu(w, selector);

						input = read_escape(&ascii);
						if(input == REGCHAR && toupper(ascii) == 'P')
							break;
						else if(input == UP && selector > 0)
							selector--;
						else if(input == DOWN && selector < 5)
							selector++;
						else if(input == REGCHAR && ascii == 10)
						{
							if(selector == 0)		// Unpause
								break;
							else if(selector == 1)	// Quit Game
								goto exitbreak;
							else if(selector == 2)	// Reset Game
							{
								file_msg = -1;
								save_msg = -1;
								reset_msg = MESSAGE_TIME;
								reset = 1;
								int i, j;
								for(i = 0; i < WELL_WIDTH-1; i++)
									for(j = 0; j < WELL_HEIGHT-1; j++)
										cells[i][j]->state[CURRENT] = DIE;
								display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
								display_frequency(move_timeout, move_counter, w);
								draw_well(w);
								break;
							}
							else if(selector == 3)	// Pick and place shape
							{
								int inner_counter;
								for(inner_counter = 0; inner_counter < 10; inner_counter++)
									mvprintw(w->upper_left_y + inner_counter, w->upper_left_x - 25, "                        ");
								mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "                        ");
								mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "[1] for oscillator");
								mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "[2] for glider");
								mvprintw(w->upper_left_y + 2, w->upper_left_x - 25, "[3] for random");
								mvprintw(w->upper_left_y + 3, w->upper_left_x - 25, "[4] for C291 <Kills All>");

								int shape;

								while(1)
								{
									input = read_escape(&ascii);
									if(input == REGCHAR)
									{
										if(ascii == '1')
										{
											shape = 1;
											break;
										}
										if(ascii == '2')
										{
											shape = 2;
											break;
										}
										if(ascii == '3')
										{
											shape = 3;
											break;
										}
										if(ascii == '4')
										{
											shape = 4;
											break;
										}
									}
								}
								mvprintw(w->upper_left_y + 0, w->upper_left_x - 25, "                        ");
								mvprintw(w->upper_left_y + 1, w->upper_left_x - 25, "                        ");
								mvprintw(w->upper_left_y + 2, w->upper_left_x - 25, "                        ");
								mvprintw(w->upper_left_y + 3, w->upper_left_x - 25, "                        ");

								int shift = 4;

								if(shape == 1)
									mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected oscillator-");
								else if(shape == 2)
									mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected glider-");
								else if(shape == 3)
									mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected random-");
								else
									mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected C291-");

								int x, y;
								x = -1;
								y = -1;

								int tempBorder = border/2;

								mvprintw(w->upper_left_y + 3, w->upper_left_x + w->width + shift, "Coordinate in form x-y");
								mvprintw(w->upper_left_y + 4, w->upper_left_x + w->width + shift, "x from 0 to %d", WELL_WIDTH-1 - tempBorder - (tempBorder-1));
								mvprintw(w->upper_left_y + 5, w->upper_left_x + w->width + shift, "y from 0 to %d", WELL_HEIGHT-1 - tempBorder - (tempBorder-1));

								nodelay(stdscr, FALSE);
								echo();
								curs_set(1);
								while(true)
								{
									mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "                                   ");
									mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "Input Coordinate : ");
									scanw(" %d-%d", &x, &y);
									if(	x+(tempBorder-1) >= tempBorder-1 && x+(tempBorder-1) <= WELL_WIDTH-1 - tempBorder &&
											y+(tempBorder-1) >= tempBorder-1 && y+(tempBorder-1) <= WELL_HEIGHT-1 - tempBorder)
										break;
									mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "<Select Valid Coord>");
								}
								curs_set(0);
								mvprintw(w->upper_left_y + 8, w->upper_left_x + w->width + shift, "You picked %d and %d", x, y);

								nodelay(stdscr, TRUE);
								noecho();

								int num;
								for(num = 0; num <= 8; num++)
									mvprintw(w->upper_left_y + num, w->upper_left_x + w->width + shift, "                            ");

								switch(shape)
								{
									case 1	: place_oscillator(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);	break;
									case 2	: place_glider(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);		break;
									case 3	: place_random(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);		break;
									default : place_C291(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);			break;
								}
								display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
								display_frequency(move_timeout, move_counter, w);
								draw_well(w);
							}
							else if(selector == 4)	// Help Menu
							{
								clear();
								draw_well(w);
								int shift = WELL_WIDTH/2 - 18 + (14-border)/2;
								mvprintw(w->upper_left_y + 5, w->upper_left_x + shift, "<Press any key to exit>");
								mvprintw(w->upper_left_y + 7, w->upper_left_x + shift - 3, "[P] to pause");
								mvprintw(w->upper_left_y + 8, w->upper_left_x + shift - 3, "[Q] to quit");
								mvprintw(w->upper_left_y + 9, w->upper_left_x + shift - 3, "[R] to reset");
								mvprintw(w->upper_left_y + 10, w->upper_left_x + shift - 3, "[C] to set frequency");
								mvprintw(w->upper_left_y + 11, w->upper_left_x + shift - 3, "[L] to load .life file");
								mvprintw(w->upper_left_y + 12, w->upper_left_x + shift - 3, "[B] for C291 <Kills All>");
								mvprintw(w->upper_left_y + 13, w->upper_left_x + shift - 3, "[*] for frequency * 1.2");
								mvprintw(w->upper_left_y + 14, w->upper_left_x + shift - 3, "[/] for frequency / 1.2");
								mvprintw(w->upper_left_y + 15, w->upper_left_x + shift - 3, "[-] for board size - 2");
								mvprintw(w->upper_left_y + 16, w->upper_left_x + shift - 3, "[+] for board size + 2");
								mvprintw(w->upper_left_y + 17, w->upper_left_x + shift - 3, "[S] to save game");
								while(1)	// Exit screen
								{
									input = read_escape(&ascii);
									if(input == REGCHAR && ascii != '\0')
										break;
								}
								mvprintw(w->upper_left_y - 5, w->upper_left_x + (border_width*2)/5, "<Game Paused>");
								display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
								display_frequency(move_timeout, move_counter, w);
								draw_well(w);
							}
							else					// Load Game
							{
								// Selection menu for that
								// Add additional option above the files to cancel.
								// When increasing and decreasing change the current loadset.
								// For loading when decrementing have check if it is 0. If it is, load the current load file.
								FILE *fp;
								fp = fopen("saves/current/current.life", "w");
								fprintf(fp, "%d\n", border);
								int i, j;
								for(i = 0; i < WELL_HEIGHT-1; i++)
								{
									for(j = 0; j < WELL_WIDTH-1; j++)
									{
										if(cells[j][i]->state[CURRENT] == LIVE)
											fprintf(fp, "@");
										else
											fprintf(fp, "-");
									}
									fprintf(fp, "\n");
								}
								fclose(fp);
								char *openFile = malloc(sizeof(char)*20);
								int file_count = how_many_files();
								int inner_selector = 0;	// From 0-file_count
								int inner_counter;
								for(inner_counter = 0; inner_counter < 11; inner_counter++)
									mvprintw(w->upper_left_y + inner_counter, w->upper_left_x - 25, "                        ");
								while(1)
								{
									print_files(w, file_count, inner_selector);
									input = read_escape(&ascii);
									if((input == UP && inner_selector > 0) || (input == DOWN && inner_selector < file_count))
									{
										if(input == UP)
											inner_selector--;
										else if(input == DOWN)
											inner_selector++;
										int newBorder;
										if(inner_selector == 0)
											newBorder = import_file("saves/current/current.life", WELL_WIDTH - 1, WELL_HEIGHT - 1, cells);
										else
										{
											char *filePath = "saves/";
											char *fileName = "save_";
											char *fileType = ".life";
											char *num = malloc(sizeof(char)*2);
											sprintf(num, "%d", inner_selector);
											strcpy(openFile, filePath);
											strcat(openFile, fileName);
											strcat(openFile, num);
											strcat(openFile, fileType);
											newBorder = import_file(openFile, WELL_WIDTH - 1, WELL_HEIGHT - 1, cells);
										}
										int difference = newBorder - border;
										border += difference;
										border_width -= difference;
										border_height -= difference;
										w->width = border_width;
										w->height = border_height;
										w->upper_left_x = (x_max / 2) - (border_width / 2) - 1;
										w->upper_left_y = (y_max / 2) - (border_height / 2) - 1;
										clear();
										display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
										display_frequency(move_timeout, move_counter, w);
										draw_well(w);
										mvprintw(w->upper_left_y - 5, w->upper_left_x + (border_width*2)/5, "<Game Paused>");
									}
									else if(input == REGCHAR && ascii == 10)
									{
										for(inner_counter = 0; inner_counter < 11; inner_counter++)
											mvprintw(w->upper_left_y + inner_counter, w->upper_left_x - 25, "                        ");
										break;
									}
									print_files(w, file_count, inner_selector);
								}
							}
						}
						print_pause_menu(w, selector);
					}
					mvprintw(w->upper_left_y - 5, w->upper_left_x + (border_width*2)/5, "             ");
					clear();
					if(reset)
					{
						shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
						shifty = WELL_HEIGHT + (14-border)/2 - 5;
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx, "-reset game-");
					}
				}
				else if(input == REGCHAR && toupper(ascii) == 'Q')	// [Q] Quit
				{
					goto exitbreak;
				}
				else if(input == REGCHAR && toupper(ascii) == 'R')	// [R] Reset
				{
					file_msg = -1;
					save_msg = -1;
					reset_msg = MESSAGE_TIME;
					int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
					int shifty = WELL_HEIGHT + (14-border)/2 - 5;
					mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
					mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx, "-reset game-");
					int i, j;
					for(i = 0; i < WELL_WIDTH-1; i++)
						for(j = 0; j < WELL_HEIGHT-1; j++)
							cells[i][j]->state[CURRENT] = DIE;
				}
				else if(input == REGCHAR && toupper(ascii) == 'C')	// [C] Frequency Set
				{
					int freq = 0;
					display_c(&freq, w);
					move_timeout = freq;
				}
				else if(input == REGCHAR && toupper(ascii) == 'L')	// [L] Load .life file
				{
					clear_game_options(w);
					int shift = 4;
					int clear_counter;
					FILE *fp;
					char *openFile = malloc(sizeof(char)*20);
					char *filePath = "saves/";
					char *fileName = malloc(sizeof(char)*20);
					char *fileType = ".life";
					int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
					int shifty = WELL_HEIGHT + (14-border)/2 - 5;
					mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
					nodelay(stdscr, FALSE);
					echo();
					curs_set(1);
					while(1)
					{
						mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "<Enter a file name of type .life>");
						mvprintw(w->upper_left_y + 1, w->upper_left_x + w->width + shift, "<The named file must be in the \'saves\' folder>");
						mvprintw(w->upper_left_y + 3, w->upper_left_x + w->width + shift, "Input File Name : ");
						scanw(" %s", fileName);

						strcpy(openFile, filePath);
						strcat(openFile, fileName);
						strcat(openFile, fileType);
						curs_set(0);
						mvprintw(w->upper_left_y + 5, w->upper_left_x + w->width + shift, "You entered \'saves/%s.life\'.", fileName);
						mvprintw(w->upper_left_y + 6, w->upper_left_x + w->width + shift, "Is this what you wanted?");
						mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "[Y] Yes");
						mvprintw(w->upper_left_y + 8, w->upper_left_x + w->width + shift, "[N] No");
						input = read_escape(&ascii);
						if(input == REGCHAR && toupper(ascii) == 'Y')
							break;
						curs_set(1);
						for(clear_counter = 0; clear_counter < 9; clear_counter++)
							mvprintw(w->upper_left_y + clear_counter, w->upper_left_x + w->width + shift, "                                                          ");
					}
					for(clear_counter = 0; clear_counter < 9; clear_counter++)
						mvprintw(w->upper_left_y + clear_counter, w->upper_left_x + w->width + shift, "                                                          ");
					nodelay(stdscr, TRUE);
					noecho();
					curs_set(0);
					reset_msg = -1;
					save_msg = -1;
					file_msg = MESSAGE_TIME;
					if(fopen(openFile, "r") != NULL)
					{
						int newBorder = import_file(openFile, WELL_WIDTH - 1, WELL_HEIGHT - 1, cells);
						int difference = newBorder - border;
						border += difference;
						border_width -= difference;
						border_height -= difference;
						w->width = border_width;
						w->height = border_height;
						w->upper_left_x = (x_max / 2) - (border_width / 2) - 1;
						w->upper_left_y = (y_max / 2) - (border_height / 2) - 1;
						clear();
						shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
						shifty = WELL_HEIGHT + (14-border)/2 - 5;
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "-opened game %s.life-", fileName);
					}
					else
					{
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-2, "-file not found-");
					}
				}
				else if(input == REGCHAR && toupper(ascii) == 'B')	// [B] Bonus - C291
				{
					clear_game_options(w);
					int shift = 4;
					int x, y;
					x = -1;
					y = -1;

					int tempBorder = border/2;
					mvprintw(w->upper_left_y + 0, w->upper_left_x + w->width + shift, "-selected C291-");
					mvprintw(w->upper_left_y + 3, w->upper_left_x + w->width + shift, "Coordinate in form x-y");
					mvprintw(w->upper_left_y + 4, w->upper_left_x + w->width + shift, "x from 0 to %d", WELL_WIDTH-1 - tempBorder - (tempBorder-1));
					mvprintw(w->upper_left_y + 5, w->upper_left_x + w->width + shift, "y from 0 to %d", WELL_HEIGHT-1 - tempBorder - (tempBorder-1));

					nodelay(stdscr, FALSE);
					echo();
					curs_set(1);
					while(true)
					{
						mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "                                   ");
						mvprintw(w->upper_left_y + 7, w->upper_left_x + w->width + shift, "Input Coordinate : ");
						scanw(" %d-%d", &x, &y);
						if(	x+(tempBorder-1) >= tempBorder-1 && x+(tempBorder-1) <= WELL_WIDTH-1 - tempBorder &&
								y+(tempBorder-1) >= tempBorder-1 && y+(tempBorder-1) <= WELL_HEIGHT-1 - tempBorder)
							break;
						mvprintw(w->upper_left_y + 2, w->upper_left_x + w->width + shift, "<Select Valid Coord>");
					}
					curs_set(0);
					mvprintw(w->upper_left_y + 8, w->upper_left_x + w->width + shift, "You picked %d and %d", x, y);

					nodelay(stdscr, TRUE);
					noecho();

					int num;
					for(num = 0; num <= 8; num++)
						mvprintw(w->upper_left_y + num, w->upper_left_x + w->width + shift, "                            ");

					place_C291(x, y, WELL_WIDTH-1, WELL_HEIGHT-1, cells, border);
					pause_switch = 1;
				}
				else if(input == REGCHAR && ascii == '*')	// [*] Frequency * 1.2
				{
					if(move_timeout * 1.2 > MAX_FREQ)
						move_timeout = MAX_FREQ;
					else
						move_timeout *= 1.2;
				}
				else if(input == REGCHAR && ascii == '/')	// [/] Frequency / 1.2
				{			
					if(move_timeout / 1.2 < MIN_FREQ)
						move_timeout = MIN_FREQ;
					else
						move_timeout /= 1.2;
				}
				else if(input == REGCHAR && (ascii == '-' || ascii == '+'))	// [-] or [+]
				{
					if(ascii == '-' && border + 2 <= 14)
					{
						border += 2;
						border_width -= 2;
						border_height -= 2;
					}
					else if(ascii == '+' && border - 2 >= 2)
					{
						border -= 2;
						border_width += 2;
						border_height += 2;
					}
					clear();
					w->width = border_width;
					w->height = border_height;
					w->upper_left_x = (x_max / 2) - (border_width / 2) - 1;
					w->upper_left_y = (y_max / 2) - (border_height / 2) - 1;
				}
				else if(input == REGCHAR && toupper(ascii) == 'S')	// [S] Save Game
				{
					file_msg = -1;
					reset_msg = -1;
					save_msg = MESSAGE_TIME;
					save_cells(WELL_WIDTH-1, WELL_HEIGHT-1, cells, border, w);
				}

				display_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
				display_game_options(w);
				display_frequency(move_timeout, move_counter, w);
				draw_well(w);

				if (move_counter > move_timeout)
				{
					update_neighbours(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
					update_cells(WELL_WIDTH - 1, WELL_HEIGHT - 1, cells, border);
					move_counter = 0;
				}
				if(save_msg >= 0)	// Display the "-saved game-" notification until save_msg is less than 0
				{
					save_msg--;
					if(save_msg < 0)
					{
						int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
						int shifty = WELL_HEIGHT + (14-border)/2 - 5;
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-2, "                  ");
					}
				}
				if(reset_msg >= 0)
				{
					reset_msg--;
					if(reset_msg < 0)
					{
						int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
						int shifty = WELL_HEIGHT + (14-border)/2 - 5;
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-2, "                  ");
					}
				}
				if(file_msg >= 0)
				{
					file_msg--;
					if(file_msg < 0)
					{
						int shiftx = WELL_WIDTH/2 - 18 + (14-border)/2 + 6;
						int shifty = WELL_HEIGHT + (14-border)/2 - 5;
						mvprintw(w->upper_left_y + shifty, w->upper_left_x + shiftx-6, "                                        ");
					}
				}

				move_counter++;
				break;

exitbreak:;
	  state = EXIT;
	  break;
			case EXIT:
	  clear();
	  draw_well(w);
	  display_end(w, border);
	  while(1)	// Exit screen
	  {
		  input = read_escape(&ascii);
		  if(input == REGCHAR && ascii != '\0')
			  break;
	  }
	  endwin();
	  return(0);
	  break;
		}
		input = 0;
		ascii = 0;

		refresh();
		nanosleep(&tim,&tim_ret);
	}
}

void end(void)
{
	mvprintw(50, 50, "END");
}

