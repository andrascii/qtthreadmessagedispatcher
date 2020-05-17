#include "stdafx.h"
#include "message.h"
#include "requester.h"

namespace MessageDispatcher {

Message Message::undefined_message() {
  return Message(Message::MessageTypeUndefined, QThread::currentThread(), nullptr, nullptr);
}

Message Message::start_request_message(RequesterSharedPtr requester) {
  if (requester) {
    return Message(MessageTypeStartRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
  }

  return undefined_message();
}

Message Message::stop_request_message(RequesterSharedPtr requester) {
  if (requester) {
    return Message(MessageTypeStopRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
  }

  return undefined_message();
}

Message Message::post_response_message(RequesterSharedPtr requester, IResponseSharedPtr response) {
  if (requester) {
    return Message(MessageTypePostResponse, requester->thread(), requester, response);
  }

  return undefined_message();
}

Message::MessageType Message::type() const noexcept {
  return type_;
}

QThread* Message::target_thread() const noexcept {
  return target_thread_;
}

RequesterSharedPtr Message::requester() const noexcept {
  return requester_;
}

IResponse* Message::response() const noexcept {
  return response_.get();
}

IRequest* Message::request() const noexcept {
  return requester()->request();
}

QObject* Message::handler() const noexcept {
  return requester()->handler();
}

Message::Message(MessageType type, QThread* target_thread, RequesterSharedPtr requester, IResponseSharedPtr response)
  : type_(type),
    target_thread_(target_thread),
    requester_(requester),
    response_(response) {
}

}  // namespace MessageDispatcher
