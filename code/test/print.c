#include "syscall.h"


int main (void) {
	PrintStr("This is a message: " /* string literal is '\0' terminated */);
	PrintInt(123);	/* positive */
	PrintStr("\n");
	PrintInt(-123);	/* negative */
	PrintStr("\n");
	PrintInt(0);		/* zero */
	PrintStr("\n");
	Halt();
	/* not reached */
}
