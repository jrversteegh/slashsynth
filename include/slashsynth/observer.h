#ifndef SLASHSYNTH_OBSERVER_H_INCLUDED__
#define SLASHSYNTH_OBSERVER_H_INCLUDED__

#include "utils.h"

namespace slashsynth {

struct ObservableBase;
struct ObserverBase;

struct ObservationBase {
  virtual void notify(int value) = 0;
  virtual void link() = 0;
  virtual void unlink() = 0;
  virtual void set_observed(ObservableBase* observed) = 0;
  virtual void set_observer(ObserverBase* observer) = 0;
  virtual ~ObservationBase() {}
};

struct ObservableBase {
  std::vector<ObservationBase*> observers{};
  ObservableBase() = default;
  ObservableBase(ObservableBase const&): observers{} {}
  ObservableBase(ObservableBase&& other): observers{} {
    for (auto& observation: other.observers) {
      observation->set_observed(this);
    }
  }
  ObservableBase& operator=(ObservableBase const&) {
    return *this;
  }
  ObservableBase& operator=(ObservableBase&& other) {
    for (auto& observation: other.observers) {
      observation->set_observed(this);
    }
    return *this;
  }
  virtual ~ObservableBase() {
    for (auto& observation: observers) {
      observation->unlink();
    }
  }
  void event(int value) {
    for (auto& observation: observers) {
      observation->notify(value);
    };
  }
};

struct ObserverBase {
  std::vector<ObservationBase*> observations{};
  ObserverBase() = default;
  ObserverBase(ObserverBase const&): observations{} { }
  ObserverBase(ObserverBase&& other): observations{} {
    for (auto& observation: other.observations) {
      observation->set_observer(this);
    }
  }
  ObserverBase& operator=(ObserverBase const&) {
    return *this;
  }
  ObserverBase& operator=(ObserverBase&& other) {
    for (auto& observation: other.observations) {
      observation->set_observer(this);
    }
    return *this;
  }
  virtual ~ObserverBase() {
    for (auto& observation: observations) {
      observation->unlink();
    }
  }
};

template <typename Observable, typename Observer>
struct Observation: ObservationBase {
  Observation() = delete;
  Observation(Observable& observable, Observer& observer): observed_(&observable), observer_(&observer) {
    link();
  }
  Observation(Observation const& other): observed_(other.observed_), observer_(other.observer_) {
    link();
  }
  Observation(Observation&& other): observed_(), observer_() {
    other.unlink();
    observed_ = std::move(other.observed_);
    observer_ = std::move(other.observer_);
    link();
  }
  Observation& operator=(Observation const& other) {
    observed_ = other.observed_;
    observer_ = other.observer_;
    link();
    return *this;
  }
  Observation& operator=(Observation&& other) {
    other.unlink();
    observed_ = std::move(other.observed_);
    observer_ = std::move(other.observer_);
    link();
    return *this;
  }
  ~Observation() override {
    unlink();
  }

  void notify(int value) override {
    if (observer_ != nullptr) {
      observer_->notify(value);
    }
  }
private:
  Observable* observed_;
  Observer* observer_;
  friend ObservableBase;
  friend ObserverBase;

  void link_observed() {
    if (observed_ != nullptr) {
      observed_->observers.push_back(this);
    }
  }
  void link_observer() {
    if (observer_ != nullptr) {
      observer_->observations.push_back(this);
    }
  }
  void link() override {
    link_observed();
    link_observer();
  }
  void unlink_observed() {
    if (observed_ != nullptr) {
      remove_one(observed_->observers, this);
    }
    observed_ = nullptr;
  }
  void unlink_observer() {
    if (observer_ != nullptr) {
      remove_one(observer_->observations, this);
    }
    observer_ = nullptr;
  }
  void unlink() override {
    unlink_observed();
    unlink_observer();
  }
  void set_observed(ObservableBase* observed) override {
    if (observed != observed_) {
      unlink_observed();
      observed_ = dynamic_cast<Observable*>(observed);
      link_observed();
    }
  }
  void set_observer(ObserverBase* observer) override {
    if (observer != observer_) {
      unlink_observer();
      observer_ = dynamic_cast<Observer*>(observer);
      link_observer();
    }
  }
};


} // namespace slashsynth

#endif
