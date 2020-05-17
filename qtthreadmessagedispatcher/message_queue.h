#pragma once

#include "message.h"

namespace MessageDispatcher {

class MessageQueue {
 public:
  void add_message(const Message& message);
  void add_message(Message&& message);
  Message extract_message();

  bool is_empty() const noexcept;

 private:
  mutable std::mutex mutex_;
  std::vector<Message> messages_;
};

}  // namespace MessageDispatcher
