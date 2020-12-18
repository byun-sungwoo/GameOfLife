typedef struct terminal_dimensions
{
	int maxx;
	int maxy;
} terminal_dimensions_t;

#ifndef DELAY_US
#define DELAY_US 100000
#define BASE_TIMEOUT 150
#define MIN_FREQ 150
#define MAX_FREQ 5000
#endif

// Game States
enum {INIT, STEP, EXIT};

void init_game(void);
void end(void);
int game(void);

