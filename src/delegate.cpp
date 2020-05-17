#include "stdafx.h"
#include "delegate.h"

namespace MessageDispatcher {

void Delegate::clear() {
  callbacks_.clear();
}

void Delegate::add(const Callback& callback) {
  callbacks_.push_back(callback);
}

void Delegate::operator()(const IResponse& response) const {
  for (const Callback& callback : callbacks_) {
    callback(response);
  }
}

}  // namespace MessageDispatcher
