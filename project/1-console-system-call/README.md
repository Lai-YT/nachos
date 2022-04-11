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

  void SysPrintStr(const char* str)
  {
    while (*str != '\0') {
      kernel->synchConsoleOut->PutChar(*str++);
    }
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
            /* NOTE:
              It's technically possible to call kernel->synchConsoleOut->PutChar
              here in the interrupt handler, so don't have to do all these
              allocations, but I want to consist with calling kernel syscall APIs
              and let them do the real works.
            */

            /* get the str out from the memory */
            {  /* new block to avoid "cross Initializtion" errors */

              /* read start addr */
              int addr = kernel->machine->ReadRegister(4);

              /* have the array grow dynamically to read all the chars from memory */
              size_t size = 0, capacity = 10;
              /* 1 space for '\0' */
              char* data = (char*)calloc(capacity - 1, sizeof(char));
              for (size_t i = 0;
                  kernel->machine->ReadMem(addr++, 1, (int*)(data + i)) && data[i] != '\0';
                  ++i) {
                /* dynamic allocation, 2 times bigger every growth, like a std::vector */
                if (++size == capacity) {
                  capacity *= 2;
                  char* old_data = data;
                  data = (char*)calloc(capacity - 1, sizeof(char));
                  memcpy(data, old_data, size);
                  free(old_data);
                }
              }
              data[size + 1] = '\0';
              SysPrintStr(data);
              free(data);
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
