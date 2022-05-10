/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"


void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


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


char* SysReadFilenameFromAddress(int addr)
{
  DEBUG(dbgSys, "Getting filename from addr: " << addr << "\n");
  const int MAX_LENGTH = 100;

  char* filename = new char[MAX_LENGTH + 1];
  int data = 0;
  int pos = 0;
  for (;pos < MAX_LENGTH && kernel->machine->ReadMem(addr + pos, 1, &data) && data; ++pos) {
    filename[pos] = (char)data;
  }

  if (pos >= MAX_LENGTH) {
    DEBUG(dbgSys, "filename can't be longer than " << MAX_LENGTH << "\n");
    delete [] filename;
    ASSERT(false);
  }
  
  filename[pos] = '\0';
  DEBUG(dbgSys, "file \"" << filename << "\" read from memory\n");

  return filename;
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
