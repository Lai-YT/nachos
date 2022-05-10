#include "syscall.h"


int main(void) {
  char* FILENAME = "my-test-file.c";
  
  Create(FILENAME);

  Halt();
}
