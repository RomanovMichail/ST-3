// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

class DoorTimeoutException : public std::exception {
 public:
  const char* what() const noexcept override {
    return "Door has timed out while open";
  }
};

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  if (door.isDoorOpened()) {
    throw DoorTimeoutException();
  }
}

TimedDoor::TimedDoor(int timeout)
    : adapter(nullptr), iTimeout(timeout), isOpened(false) {}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
  if (adapter == nullptr) {
    adapter = new DoorTimerAdapter(*this);
  }
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw DoorTimeoutException();
  }
}

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

Timer::Timer() : client(nullptr) {}

void Timer::tregister(int timeout, TimerClient* c) {
  std::thread t([timeout, c]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    if (c != nullptr) {
      try {
        c->Timeout();
      } catch (...) {
      }
    }
  });
  t.detach();
}


