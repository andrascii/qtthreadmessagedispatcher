#include "stdafx.h"
#include "message_queue.h"

namespace MessageDispatcher {

void MessageQueue::add_message(const Message& message) {
  std::lock_guard<std::mutex> locker(mutex_);
  messages_.push_back(message);
}

void MessageQueue::add_message(Message&& message) {
  std::lock_guard<std::mutex> locker(mutex_);
  messages_.emplace_back(std::move(message));
}

bool MessageQueue::is_empty() const noexcept {
  std::lock_guard<std::mutex> locker(mutex_);
  return messages_.empty();
}

Message MessageQueue::extract_message() {
  std::lock_guard<std::mutex> locker(mutex_);

  if (!messages_.size()) {
    return Message::undefined_message();
  }

  Message message = std::move(messages_.back());
  messages_.pop_back();

  return message;
}

}  // namespace MessageDispatcher
