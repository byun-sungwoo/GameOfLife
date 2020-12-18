typedef struct cell
{
	int x;
	int y;

	/* State explanation
	 * state[0] represents previous state
	 * state[1] is the current state
	 * state[2] is the next step
	 */
	int state[3];
	char draw_char;
	char color[3];
} cell_t;

enum {LIVE, DIE, UNKNOWN};  // State of each cell
enum {OLD, CURRENT, NEW};   // Enumerations for each step
enum {SUCCESS, FAIL};       // Status of functions

cell_t * create_cell (int init_x, int init_y);
int destroy_cell (cell_t *);
int update_cell (cell_t *);
void print_cell (cell_t *);
void display_cell (cell_t *);
void undisplay_cell (cell_t *);
void update_cells (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border);
void display_cells (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border);
void update_neighbours (int c_size_x, int c_size_y, cell_t * c[c_size_x][c_size_y], int border);
void test_cell (void);

