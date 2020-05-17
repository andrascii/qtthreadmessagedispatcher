#pragma once

#include "message_queue.h"

namespace MessageDispatcher {

class IResponse;
class Requester;

class ThreadMessageDispatcher : public QObject {
  Q_OBJECT

 public:
  static std::shared_ptr<ThreadMessageDispatcher> for_thread(QThread* thread);
  static std::shared_ptr<ThreadMessageDispatcher> for_current_thread();

  void start_request(RequesterSharedPtr requester);
  void stop_request(RequesterSharedPtr requester);
  void post_response(RequesterSharedPtr requester, IResponseSharedPtr response);

 protected:
  virtual void timerEvent(QTimerEvent* event) override;

 private:
  ThreadMessageDispatcher(QThread* thread);

  void start_dispatch_timer();
  void stop_dispatch_timer();

  void execute();

  MessageQueue& message_queue() noexcept;

 private slots:
  void shutdown();

 private:
  int dispatch_timer_id_;
  MessageQueue message_queue_;
};

}  // namespace MessageDispatcher
