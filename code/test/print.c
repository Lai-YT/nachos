#include "syscall.h"


int main (void) {
	// Print("This is a message: ");
	PrintInt(123);	/* positive */
	PrintInt(-123);	/* negative */
	PrintInt(0);		/* zero */
	// Print("\n");
	Halt();
	/* not reached */
}
