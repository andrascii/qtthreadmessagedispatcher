#pragma once

#include "iresponse.h"
#include "requester.h"

namespace MessageDispatcher {

class IRequest;

class Message {
 public:
  enum MessageType {
    MessageTypeUndefined,
    MessageTypeStartRequest,
    MessageTypeStopRequest,
    MessageTypePostResponse
  };

  static Message undefined_message();
  static Message start_request_message(RequesterSharedPtr requester);
  static Message stop_request_message(RequesterSharedPtr requester);
  static Message post_response_message(RequesterSharedPtr requester, IResponseSharedPtr response);

  MessageType type() const noexcept;
  QThread* target_thread() const noexcept;
  RequesterSharedPtr requester() const noexcept;
  IResponse* response() const noexcept;
  IRequest* request() const noexcept;
  QObject* handler() const noexcept;

 private:
  Message(MessageType type, QThread* target_thread, RequesterSharedPtr requester, IResponseSharedPtr response);

 private:
  MessageType type_;
  QThread* target_thread_;
  RequesterSharedPtr requester_;
  IResponseSharedPtr response_;
};

}  // namespace MessageDispatcher
