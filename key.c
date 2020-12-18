#include <ncurses.h>
#include "key.h"

int read_escape(int *read_char)
{
	int c;
	if ((c = getch()) == ERR) {
		return (NOCHAR);
	}
	else if (c==0x1b) {
		if ((c = getch()) == '[') {
			c=getch();
			switch (c) {
				case 'A':
					return (UP);
					break;
				case 'B':
					return (DOWN);
					break;
				case 'C':
					return (RIGHT);
					break;
				case 'D':
					return (LEFT);
					break;
				default:
					return (BADESC);
			}
		}
	}
	else {
		*read_char = c;
		return (REGCHAR);
	}
}

