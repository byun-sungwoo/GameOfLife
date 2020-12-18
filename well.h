#define WELL_WIDTH 56 // og is 50
#define WELL_HEIGHT 31 // og is 25

typedef struct well
{
	int upper_left_x;
	int upper_left_y;
	int width;
	int height;
	char draw_char;
	char color[3];
} well_t;

well_t *init_well(int, int, int, int);
well_t *changeWellSize(int upper_left_x, int upper_left_y, int width, int height, well_t *);

void undraw_well(well_t *);
void draw_well(well_t *);

