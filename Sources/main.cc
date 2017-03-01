#include <iostream>
#include "opcode.h"

using namespace Eschelle;

int
main(int argc, char** argv){
  Binary bin(1024);
  bin.Append(kOpMov, 0xA, 0xB);
  bin.Append(kOpNop, -1, -1);
  bin.Append(kOpMov, 0xCC, 0xDD);

  return 0x0;
}
