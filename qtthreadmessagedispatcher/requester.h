#pragma once

#include "irequest.h"
#include "iresponse.h"
#include "delegate.h"

namespace MessageDispatcher {

//! A request handler can send you a response only if the requester at the moment of sending is alive
class Requester : public QObject, public std::enable_shared_from_this<Requester> {
  Q_OBJECT

 public:
  enum State {
    StateClear,
    StateWorking,
    StateStopped
  };

  Requester() = default;
  Requester(const Requester& other);
  Requester(const IRequest& request);

  QObject* handler() const noexcept;
  IRequest* request() const noexcept;

  State state() const noexcept;

  void start();
  void stop();
  void process_response(const IResponse& response) const;

  template <typename ResponseType>
  void add_callback(void(*function)(Requester*, const ResponseType&)) {
    const auto callback = [this, function](const IResponse& response) {
      if (response.type() != ResponseType::response_static_type()) {
        return;
      }

      function(this, response);
    };

    delegate_.add(callback);
  }

  template <typename ObjectType, typename ResponseType>
  void add_callback(ObjectType* object, void(ObjectType::*method)(Requester*, const ResponseType&)) {
    const auto callback = [this, object, method](const IResponse& response) {
      if (response.type() != ResponseType::response_static_type()) {
        return;
      }

      (object->*method)(this, static_cast<const ResponseType&>(response));
    };

    delegate_.add(callback);
  }

 private:
  std::shared_ptr<IRequest> request_;

  Delegate delegate_;

  QObject* handler_;

  State state_;
};

using RequesterSharedPtr = std::shared_ptr<Requester>;
using RequesterWeakPtr = std::weak_ptr<Requester>;

}  // namespace MessageDispatcher

Q_DECLARE_METATYPE(MessageDispatcher::RequesterSharedPtr);
Q_DECLARE_METATYPE(MessageDispatcher::Requester)
