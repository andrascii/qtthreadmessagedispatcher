#pragma once

#include "irequest.h"
#include "iresponse.h"
#include "meta_helpers.h"
#include "requester.h"

DEFINE_METHOD_CHECK(handle_request)
DEFINE_METHOD_CHECK(stop_request_handling)

namespace MessageDispatcher {

template <typename T>
struct RequestTypeHelper {
  static constexpr bool value = std::is_same<T, RequestType>::value;
};


class HandlerRegistry {
 public:
  struct Subscription {
    QPointer<QObject> subscriber;
    QMetaMethod method;
  };

  static HandlerRegistry& instance();

  template <typename ObjectType, typename... RequestTypes>
  void registrate_handler(ObjectType* handler, RequestTypes&&... request_types) {
    static_assert(sizeof...(request_types) > 0, "Passed empty request types argument list");

    static_assert(MetaHelpers::is_all_of<RequestTypeHelper, RequestTypes...>(),
      "Each request type parameter must be of RequestType enumeration type");

    static_assert(std::is_base_of_v<QObject, ObjectType>, "Passed object must be derived from QObject");

    static_assert(MetaHelpers::HasMethod_handle_request<ObjectType, RequesterSharedPtr>::value,
      "Passed object does not contain handleRequest function");

    static_assert(MetaHelpers::HasMethod_stop_request_handling<ObjectType, RequesterSharedPtr>::value,
      "Passed object does not contain stopRequestHandling function");

    std::lock_guard<std::mutex> locker(mutex_);

    const auto condition_checker_functor = [this](RequestType requestType) {
      return handlers_.find(requestType) == handlers_.end();
    };

    ASSERT(MetaHelpers::is_each_argument(condition_checker_functor, std::forward<RequestTypes>(request_types)...));

    const auto functor = [this, handler](RequestType requestType) {
      handlers_[requestType] = static_cast<QObject*>(handler);
    };

    MetaHelpers::call_for_all_arguments_in_pack(functor, std::forward<RequestTypes>(request_types)...);
  }

  void add_subscription(Subscription subscriber, QObject* handler, ResponseType response_type);
  bool has_subscriptions_for(QObject* handler, ResponseType response_type) const;
  std::vector<Subscription> subscriptions_for(QObject* handler, ResponseType response_type) const;

  void unregistrate_handler(QObject* handler);
  void unregister_all();

  QObject* handler_for_request(const IRequest& request);
  QObject* handler_for_request(RequestType request_type);
  bool is_handler_exists(QObject* handler) const;

 private:
  HandlerRegistry() = default;
  HandlerRegistry(const HandlerRegistry&) = delete;
  HandlerRegistry(HandlerRegistry&&) = delete;

  std::pair<bool, RequestType> request_type_for_value(QObject* handler) const;

 private:
  mutable std::mutex mutex_;

  struct SubscriptionKey {
    ResponseType response_type;
    QObject* handler;

    friend bool operator==(const SubscriptionKey& lhs, const SubscriptionKey& rhs) {
      return lhs.response_type == rhs.response_type && lhs.handler == rhs.handler;
    }
  };

  struct SubscriptionKeyHasher {
    size_t operator()(const SubscriptionKey& key) const {
      std::hash<int> int_hasher;
      std::hash<QObject*> pointer_hasher;
      return int_hasher(static_cast<int>(key.response_type)) + pointer_hasher(key.handler);
    }
  };

  std::map<RequestType, QObject*> handlers_;
  std::unordered_map<SubscriptionKey, std::vector<Subscription>, SubscriptionKeyHasher> subscriptions_;
};

}  // namespace MessageDispatcher
