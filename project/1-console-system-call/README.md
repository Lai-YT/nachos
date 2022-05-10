# Project 1: Console System Call

We are to implement

- a `PrintInt` function, which outputs integers to the console, and
- a `PrintStr` function, which outputs char array to the console.

## Steps

1. Write tests: `code/test/print.c`.

  ```C++
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

  ```

2. Add into Nachos system call interface: `code/userprog/syscall.h`.

  ```C++
  ...

  #define SC_PrintInt 43
  #define SC_PrintStr 44

  ...

  /* Print the integer to the console. */
  void PrintInt(int number);

  /* Print the '\0' terminated char array. */
  void PrintStr(const char* str);
  ```

3. Define the console I/O system calls in assembly way: `code/test/start.S`.

  ```
  ...

    .globl PrintInt
  	.ent	PrintInt
  PrintInt:
  	addiu $2,$0,SC_PrintInt
  	syscall
  	j 	$31
  	.end PrintInt

    .globl PrintStr
  	.ent	PrintStr
  PrintStr:
  	addiu $2,$0,SC_PrintStr
  	syscall
  	j 	$31
  	.end PrintStr
  ```

4. Implement the kernel system calls for `ExceptionHandler` to use: `code/userprog/ksyscall.h`.

  ```C++
  ...

  void SysPrintInt(int number)
  {
    /* Use recursive approach instead of loop so
     * we don't print the digits reversely.
     */

    /* boundary condition */
    if (number == 0) {
      kernel->synchConsoleOut->PutChar('0');
      return;
    }

    /* sign */
    if (number < 0) {
      kernel->synchConsoleOut->PutChar('-');
      number *= -1;
    }

    /* digit by digit */
    if (number / 10) {
      SysPrintInt(number / 10);
    }

    kernel->synchConsoleOut->PutChar(number % 10 + '0');
  }
  ```

5. Handle the exceptions raised by system call: `code/userprog/exception.cc`.

  ```C++
  void
  ModifyReturnPoint();

  void
  ExceptionHandler(ExceptionType which)
  {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
      case SyscallException:
        switch (type) {
          ...

          case SC_PrintInt:
            DEBUG(dbgSys, "PrintInt " << kernel->machine->ReadRegister(4) << "\n");

            SysPrintInt(/* int number */(int)kernel->machine->ReadRegister(4));

            ModifyReturnPoint();

            return;

            ASSERTNOTREACHED();
            break;

          case SC_PrintStr:
            DEBUG(dbgSys, "PrintStr called\n");

            int addr;
            /* read start addr */
            addr = kernel->machine->ReadRegister(4);
            int data;
            while (kernel->machine->ReadMem(addr, 1, &data) && data /* not '\0' */) {
              kernel->synchConsoleOut->PutChar(data);
              ++addr;
            }

            ModifyReturnPoint();

            return;

            ASSERTNOTREACHED();
            break;

          default:
            ...
        }
        ...
    }
    ...
  }

  void
  ModifyReturnPoint()
  {
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
  }
  ```

We are now able to print integer and chars to the console!
