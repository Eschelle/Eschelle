#include <iostream>
#include "eschelle.h"

int
main(int argc, char** argv){
  Eschelle::uword size = Eschelle::RoundUpPow2(10);
  std::cout << "Rounded Size: " << size << std::endl;
  return 0;
}
