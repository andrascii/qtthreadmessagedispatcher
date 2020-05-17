#include "stdafx.h"
#include "requester.h"
#include "handler_registry.h"
#include "thread_message_dispatcher.h"

namespace MessageDispatcher
{

Requester::Requester(const IRequest& request) : request_(request.clone()), state_(StateClear) {
  HandlerRegistry& handler_registry = HandlerRegistry::instance();
  handler_ = handler_registry.handler_for_request(request);

  ASSERT(handler_ && "Handler for this request was not registered");

  qRegisterMetaType<Requester>("Requester");
  qRegisterMetaType<RequesterSharedPtr>("RequesterSharedPtr");
}

Requester::Requester(const Requester& other)
  : request_(other.request_),
    delegate_(other.delegate_),
    handler_(other.handler_),
    state_(other.state_) {
}

QObject* Requester::handler() const noexcept {
  return handler_;
}

IRequest* Requester::request() const noexcept {
  return request_.get();
}

Requester::State Requester::state() const noexcept {
  return state_;
}

void Requester::start() {
  state_ = StateWorking;
  ThreadMessageDispatcher::for_current_thread()->start_request(shared_from_this());
}

void Requester::stop() {
  state_ = StateStopped;
  ThreadMessageDispatcher::for_current_thread()->stop_request(shared_from_this());
}

void Requester::process_response(const IResponse& response) const {
  if (state_ == StateStopped) {
    return;
  }

  delegate_(response);

  HandlerRegistry& handler_registry = HandlerRegistry::instance();

  const std::vector<HandlerRegistry::Subscription> subscriptions =
    handler_registry.subscriptions_for(handler_, response.type());

  for (const HandlerRegistry::Subscription& subscription : subscriptions) {
    if (!subscription.subscriber) {
      continue;
    }

    const Qt::ConnectionType connectionType = subscription.subscriber->thread() == QThread::currentThread() ?
      Qt::DirectConnection : Qt::QueuedConnection;

    VERIFY(subscription.method.invoke(subscription.subscriber, connectionType, Q_ARG(const IResponse&, response)));
  }
}

}  // namespace MessageDispatcher
