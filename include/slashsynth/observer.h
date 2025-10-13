#ifndef SLASHSYNTH_OBSERVER_H_INCLUDED__
#define SLASHSYNTH_OBSERVER_H_INCLUDED__

#include "utils.h"

#include <algorithm>
#include <concepts>
#include <functional>
#include <type_traits>
#include <vector>

namespace slashsynth {

using namespace std::placeholders;

template <typename EventType>
struct ObservableBase {
  using Event_type = EventType;
  using Subscription_handle = int;

  struct Subscription {
    Subscription_handle id;
    ObservableBase* observable;
  };

  ObservableBase() = default;
  ObservableBase(ObservableBase const&) : observers_{} {}
  ObservableBase(ObservableBase&& other)
      : observers_{std::move(other.observers_)} {
    update_connection_(&other, this);
  }
  ObservableBase& operator=(ObservableBase const&) {
    return *this;
  }
  ObservableBase& operator=(ObservableBase&& other) {
    observers_ = std::move(other.observers_);
    update_connection_(&other, this);
    return *this;
  };
  ~ObservableBase() {
    update_connection_(this, nullptr);
  }

  template <typename ObserverType>
  Subscription_handle subscribe(ObserverType& observer) {
    Handler handler{
        std::bind(&ObserverType::event_notification, &observer, _1, _2),
        std::bind(&ObserverType::connection_notification, &observer, _1, _2),
    };
    auto subscription = handler.subscription;
    observers_.push_back(handler);
    return subscription;
  }

  void unsubscribe(Subscription_handle subscription) {
    std::erase_if(observers_, [subscription](auto const& handler) {
      return handler.subscription == subscription;
    });
  }

protected:
  void event(Event_type event_data) {
    for (auto observer : observers_) {
      observer.event_handler(this, event_data);
    }
  }

private:
  struct Handler {
    std::function<void(ObservableBase*, EventType)> event_handler;
    std::function<void(ObservableBase*, ObservableBase*)> connection_handler;
    Subscription_handle subscription = next_subscription_++;
  };
  using Handlers = std::vector<Handler>;
  Handlers observers_{};
  static inline Subscription_handle next_subscription_ = 0;
  void update_connection_(ObservableBase* previous, ObservableBase* current) {
    for (auto& observer : observers_) {
      observer.connection_handler(previous, current);
    }
  }
};

template <typename ObserverType, typename EventType>
struct ObserverBase {
  using Observable_type = ObservableBase<EventType>;

  ObserverBase() = default;
  ObserverBase(ObserverBase const& other) : subscriptions_{} {
    subscribe_to_(other.subscriptions_);
  }
  ObserverBase(ObserverBase&& other) = delete;
  ObserverBase& operator=(ObserverBase const& other) {
    subscribe_to(other.subscriptions_);
    return *this;
  }
  ObserverBase& operator=(ObserverBase&&) = delete;
  ~ObserverBase() {
    unsubscribe_from_(subscriptions_);
  }

  void event_notification(Observable_type* observed, EventType data) {
    ObserverType& self = static_cast<ObserverType&>(*this);
    if constexpr (requires { self.notify(observed, data); }) {
      self.notify(observed, data);
    } else if constexpr (requires { self.notify(data); }) {
      self.notify(data);
    } else {
      static_assert(false, "ObserverType should have \"notify\" method");
    }
  }

  void subscribe_to(Observable_type& observable) {
    auto subscription = observable.subscribe(*this);
    subscriptions_.emplace_back(subscription, &observable);
  }

  void connection_notification(Observable_type* old_observed,
                               Observable_type* new_observed) {
    for (auto& subscription : subscriptions_) {
      if (subscription.observable == old_observed) {
        subscription.observable = new_observed;
      }
    }
    std::erase_if(subscriptions_, [](auto const& subscription) {
      return subscription.observable == nullptr;
    });
  }

private:
  using Subscriptions = std::vector<typename Observable_type::Subscription>;
  Subscriptions subscriptions_;
  void subscribe_to_(Subscriptions const& subscriptions) {
    for (auto& subscription : subscriptions) {
      if (subscription.observable != nullptr) {
        subscribe_to(*subscription.observable);
      }
    }
  }
  static void unsubscribe_from_(Subscriptions const& subscriptions) {
    for (auto& subscription : subscriptions) {
      if (subscription.observable != nullptr) {
        subscription.observable->unsubscribe(subscription.id);
      }
    }
  }
};

} // namespace slashsynth

#endif
