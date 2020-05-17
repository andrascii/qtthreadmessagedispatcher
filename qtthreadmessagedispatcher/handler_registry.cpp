#include "stdafx.h"
#include "handler_registry.h"

namespace MessageDispatcher {

HandlerRegistry& HandlerRegistry::instance() {
  static std::unique_ptr<HandlerRegistry> s_handler_registry(new HandlerRegistry);
  return *s_handler_registry.get();
}

void HandlerRegistry::add_subscription(Subscription subscriber, QObject* handler, ResponseType response_type) {
  std::lock_guard<std::mutex> locker(mutex_);

  const SubscriptionKey subscription_key{ response_type, handler };
  ASSERT(subscriptions_.find(subscription_key) == subscriptions_.end());
  subscriptions_[subscription_key].push_back(subscriber);
}

bool HandlerRegistry::has_subscriptions_for(QObject* handler, ResponseType response_type) const {
  std::lock_guard<std::mutex> locker(mutex_);
  return subscriptions_.find(SubscriptionKey{ response_type, handler }) != subscriptions_.end();
}

std::vector<HandlerRegistry::Subscription>
HandlerRegistry::subscriptions_for(QObject* handler, ResponseType response_type) const {
  std::lock_guard<std::mutex> locker(mutex_);
  const auto find_iter = subscriptions_.find(SubscriptionKey{ response_type, handler });
  return find_iter == subscriptions_.end() ? std::vector<Subscription>() : find_iter->second;
}

void HandlerRegistry::unregistrate_handler(QObject* handler) {
  std::lock_guard<std::mutex> locker(mutex_);

  const auto [result, request_type] = request_type_for_value(handler);

  if (!result) {
    return;
  }

  handlers_.erase(request_type);
}

void HandlerRegistry::unregister_all() {
  std::lock_guard<std::mutex> locker(mutex_);
  handlers_.clear();
  subscriptions_.clear();
}

QObject* HandlerRegistry::handler_for_request(const IRequest& request) {
  return handler_for_request(request.request_type());
}

QObject* HandlerRegistry::handler_for_request(RequestType request_type) {
  std::lock_guard<std::mutex> locker(mutex_);
  const auto handler_iterator = handlers_.find(request_type);
  return handler_iterator == handlers_.end() ? nullptr : handler_iterator->second;
}

bool HandlerRegistry::is_handler_exists(QObject* handler) const {
  std::lock_guard<std::mutex> locker(mutex_);

  const auto&& [result, requestType] = request_type_for_value(handler);
  Q_UNUSED(requestType);

  return result;
}

std::pair<bool, RequestType> HandlerRegistry::request_type_for_value(QObject* handler) const {
  for (const auto& [key, value] : handlers_) {
    if (value == handler) {
      return std::make_pair(true, key);
    }
  }

  return std::make_pair(false, RequestType());
}

}  // namespace MessageDispatcher
