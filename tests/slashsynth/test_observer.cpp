#include "../test_common.h"

#include <chrono>
#include <cmath>
#include <thread>

#include <slashsynth/observer.h>

using namespace slashsynth;

struct Observable : ObservableBase<int> {
  void emit(int value) {
    event(value);
  }
};

struct Observer1 : ObserverBase<Observer1, int> {
  int sum = 0;
  void notify(int value) {
    sum += value;
  }
};

struct Observer2 : ObserverBase<Observer2, int> {
  int sum = 0;
  void notify(int value) {
    sum += value;
  }
};

BOOST_AUTO_TEST_CASE(observation_test) {
  Observer1 observer1;
  {
    Observable observable;
    observer1.subscribe_to(observable);
    {
      observable.emit(1);
      Observer2 observer2;
      observer2.subscribe_to(observable);
      {
        auto observer3 = observer2;
        observable.emit(1);
        BOOST_TEST(observer3.sum == 1);
      }
      observable.emit(2);
      auto observer3 = observer2;
      observer3.sum = 0;
      observable.emit(3);
      observable.emit(4);
      BOOST_TEST(observer1.sum == 11);
      BOOST_TEST(observer2.sum == 10);
      BOOST_TEST(observer3.sum == 7);
    }
    observable.emit(5);
    BOOST_TEST(observer1.sum == 16);
  }
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
