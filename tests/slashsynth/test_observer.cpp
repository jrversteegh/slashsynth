#include "../test_common.h"

#include <chrono>
#include <cmath>
#include <thread>

#include <slashsynth/observer.h>

using namespace slashsynth;

struct Observable: ObservableBase {
};

struct Observer1: ObserverBase {
  int sum = 0;
  void notify(int value) {
    sum += value;
  }
};

struct Observer2: ObserverBase {
  int sum = 0;
  void notify(int value) {
    sum += value;
  }
};


BOOST_AUTO_TEST_CASE(observation_test) {
  Observer1 observer1;
  {
    Observable observable;
    Observation<Observable, Observer1> observation1{observable, observer1};
    {
      Observer2 observer2;
      Observation<Observable, Observer2> observation2{observable, observer2};
      {
        Observation<Observable, Observer2> observation3{observable, observer2};
        observable.event(1);
      }
      observable.event(2);
      auto observer3 = observer2;
      observable.event(3);
      static_cast<ObservationBase*>(&observation2)->set_observer(&observer3);
      observable.event(4);
      BOOST_TEST(observer1.sum == 10);
      BOOST_TEST(observer2.sum == 7);
      BOOST_TEST(observer3.sum == 8);
    }
    observable.event(5);
    BOOST_TEST(observer1.sum == 15);
  }
  BOOST_TEST(observer1.observations.size() == 0);
}

ut::test_suite* init_unit_test_suite(int, char*[]) {
  return 0;
}
