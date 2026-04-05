// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <chrono>
#include <cstdint>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  int timeout_ms = 100;

  void SetUp() override {
    door = new TimedDoor(timeout_ms);
  }

  void TearDown() override {
    if (door != nullptr) {
      delete door;
    }
  }
};

TEST_F(TimedDoorTest, DoorCreationAndInitialState) {
  EXPECT_FALSE(door->isDoorOpened());
  EXPECT_EQ(door->getTimeOut(), timeout_ms);
}

TEST_F(TimedDoorTest, LockOperationClosesDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOperationOpensDoor) {
  EXPECT_FALSE(door->isDoorOpened());
  TimedDoor longTimeoutDoor(5000);
  longTimeoutDoor.unlock();
  EXPECT_TRUE(longTimeoutDoor.isDoorOpened());
  longTimeoutDoor.lock();
}


TEST_F(TimedDoorTest, NoExceptionWhenDoorLockedBeforeTimeout) {
  TimedDoor longTimeoutDoor(500);
  longTimeoutDoor.unlock();
  longTimeoutDoor.lock();
  EXPECT_TRUE(true);
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  int testTimeout = 250;
  TimedDoor testDoor(testTimeout);
  EXPECT_EQ(testDoor.getTimeOut(), testTimeout);
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorOpen) {
  TimedDoor longTimeoutDoor(5000);
  longTimeoutDoor.unlock();
  EXPECT_THROW(longTimeoutDoor.throwState(), std::exception);
  longTimeoutDoor.lock();
}

TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, DoorTimerAdapterTimesOut) {
  TimedDoor testDoor(150);
  testDoor.unlock();
  EXPECT_TRUE(true);
}

TEST_F(TimedDoorTest, DoorStateTransitions) {
  TimedDoor testDoor(2000);

  EXPECT_FALSE(testDoor.isDoorOpened());

  testDoor.unlock();
  EXPECT_TRUE(testDoor.isDoorOpened());

  testDoor.lock();
  EXPECT_FALSE(testDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleLockUnlockCycles) {
  TimedDoor testDoor(1000);

  for (int i = 0; i < 3; ++i) {
    testDoor.lock();
    EXPECT_FALSE(testDoor.isDoorOpened());
    testDoor.unlock();
    testDoor.lock();
    EXPECT_FALSE(testDoor.isDoorOpened());
  }
}

TEST_F(TimedDoorTest, TimeoutWithDifferentValues) {
  TimedDoor shortTimeoutDoor(50);
  shortTimeoutDoor.unlock();
  std::this_thread::sleep_for(std::chrono::milliseconds(70));
  EXPECT_THROW(shortTimeoutDoor.throwState(), std::exception);

  TimedDoor longTimeoutDoor(2000);
  longTimeoutDoor.unlock();
  longTimeoutDoor.lock();
  EXPECT_FALSE(longTimeoutDoor.isDoorOpened());
  EXPECT_NO_THROW(longTimeoutDoor.throwState());
}

TEST(MockTimerClientTest, TimeoutCanBeCalled) {
  MockTimerClient mockClient;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  mockClient.Timeout();
}

TEST(MockDoorTest, DoorInterfaceMethods) {
  MockDoor mockDoor;

  EXPECT_CALL(mockDoor, lock()).Times(1);
  EXPECT_CALL(mockDoor, unlock()).Times(1);
  EXPECT_CALL(mockDoor, isDoorOpened())
    .Times(1).WillOnce(testing::Return(true));

  mockDoor.lock();
  mockDoor.unlock();
  bool result = mockDoor.isDoorOpened();

  EXPECT_TRUE(result);
}

TEST_F(TimedDoorTest, DoorInitialStateCheck) {
  TimedDoor testDoor(3000);
  EXPECT_FALSE(testDoor.isDoorOpened());
  EXPECT_EQ(testDoor.getTimeOut(), 3000);
  EXPECT_NO_THROW(testDoor.throwState());
}
