// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <iostream>

int main() {
  try {
    TimedDoor tDoor(2000);
    std::cout << "Door created with 2000ms timeout" << std::endl;

    tDoor.lock();
    std::cout << "Door locked" << std::endl;
    std::cout << "Is door opened: " << 
      (tDoor.isDoorOpened() ? "yes" : "no") << std::endl;
    std::cout << "Unlocking door for 2 seconds..." << std::endl;
    tDoor.unlock();
    std::cout << "Door unlocked" << std::endl;
  } catch (const std::exception& e) {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  return 0;
}
