#include "thread_message_dispatcher.h"
#include "common_macro_helpers.h"
#include "handler_registry.h"
#include "requester.h"

namespace MessageDispatcher {

struct ThreadMetaData : public QObjectUserData
{
  ThreadMetaData() {}

  ThreadMetaData(std::shared_ptr<ThreadMessageDispatcher> dispatcher)
    : dispatcher(dispatcher) {}

  std::shared_ptr<ThreadMessageDispatcher> dispatcher;
};

}  // namespace MessageDispatcher

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

namespace {

using namespace MessageDispatcher;

constexpr size_t c_minimum_recommended_timer_resolution = 21;

int meta_data_index() {
  static int s_metaDataIndex = QObject::registerUserData();
  return s_metaDataIndex;
}

//
// TODO: use setProperty/property instead of setUserData/userData
//

void register_thread_queue(QThread* thread, std::shared_ptr<ThreadMessageDispatcher> dispatcher) {
  thread->setUserData(meta_data_index(), new ThreadMetaData(dispatcher));
}

void unregister_thread_queue(QThread* thread) {
  delete thread->userData(meta_data_index());
  thread->setUserData(meta_data_index(), nullptr);
}

std::shared_ptr<ThreadMessageDispatcher> thread_queue(QThread* thread) {
  QObjectUserData* metaData = thread->userData(meta_data_index());

  if (metaData == nullptr) {
    return std::shared_ptr<ThreadMessageDispatcher>();
  }

  return static_cast<ThreadMetaData*>(metaData)->dispatcher;
}

std::mutex s_mutex;

}  // namespace

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace MessageDispatcher {

std::shared_ptr<ThreadMessageDispatcher> ThreadMessageDispatcher::for_thread(QThread* thread) {
  std::lock_guard locker(s_mutex);

  std::shared_ptr<ThreadMessageDispatcher> thread_queue_ptr = thread_queue(thread);

  if (!thread_queue_ptr) {
    thread_queue_ptr.reset(new ThreadMessageDispatcher(thread));
    register_thread_queue(thread, thread_queue_ptr);
  }

  return thread_queue_ptr;
}

std::shared_ptr<ThreadMessageDispatcher> ThreadMessageDispatcher::for_current_thread() {
  return ThreadMessageDispatcher::for_thread(QThread::currentThread());
}

void ThreadMessageDispatcher::start_request(RequesterSharedPtr requester) {
  message_queue().add_message(Message::start_request_message(requester));
}

void ThreadMessageDispatcher::stop_request(RequesterSharedPtr requester) {
  message_queue().add_message(Message::stop_request_message(requester));
}

void ThreadMessageDispatcher::post_response(RequesterSharedPtr requester, IResponseSharedPtr response) {
  message_queue().add_message(Message::post_response_message(requester, response));
}

ThreadMessageDispatcher::ThreadMessageDispatcher(QThread* thread) : dispatch_timer_id_(0) {
  moveToThread(thread);
  start_dispatch_timer();

  VERIFY(connect(thread, SIGNAL(finished()), this, SLOT(shutdown()), Qt::DirectConnection));

  if (thread == qApp->thread()) {
    VERIFY(connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()), Qt::DirectConnection));
  }
}

void ThreadMessageDispatcher::shutdown() {
  stop_dispatch_timer();
  unregister_thread_queue(thread());
}

void ThreadMessageDispatcher::start_dispatch_timer() {
  dispatch_timer_id_ = startTimer(c_minimum_recommended_timer_resolution);
  ASSERT(dispatch_timer_id_);
}

void ThreadMessageDispatcher::stop_dispatch_timer() {
  ASSERT(dispatch_timer_id_);
  killTimer(dispatch_timer_id_);
}

void ThreadMessageDispatcher::execute() {
  while (!message_queue().is_empty()) {
    Message message = message_queue().extract_message();
    HandlerRegistry& handler_registry = HandlerRegistry::instance();
    RequesterSharedPtr requester = message.requester();

    if (!requester) {
      WARNLOG << "Requester is dead";
      continue;
    }

    if (!handler_registry.is_handler_exists(message.handler())) {
      WARNLOG << "Handler for request type is not exists";
      continue;
    }

    switch (message.type()) {
      case Message::MessageTypeStartRequest: {
        VERIFY(QMetaObject::invokeMethod(message.handler(), "handle_request",
          Qt::QueuedConnection, Q_ARG(RequesterSharedPtr, requester)));

        break;
      }
      case Message::MessageTypeStopRequest: {
        VERIFY(QMetaObject::invokeMethod(message.handler(), "stop_request_handling",
          Qt::QueuedConnection, Q_ARG(RequesterSharedPtr, requester)));

        break;
      }
      case Message::MessageTypePostResponse: {
        ASSERT(requester->thread() == thread());
        requester->process_response(*message.response());

        break;
      }
    }
  }
}

MessageQueue& ThreadMessageDispatcher::message_queue() noexcept {
  return message_queue_;
}

void ThreadMessageDispatcher::timerEvent(QTimerEvent*) {
  execute();
}

}  // namespace MessageDispatcher
