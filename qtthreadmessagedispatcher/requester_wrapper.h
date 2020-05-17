#pragma once

#include "irequest.h"
#include "requester.h"
#include "meta_helpers.h"

namespace MessageDispatcher {

class RequesterWrapper {
 public:
  ~RequesterWrapper() {
    reset();
  }

  Requester* get() const noexcept {
    return requester_ptr_.get();
  }

  Requester* operator->() const noexcept {
    return get();
  }

  operator bool() const noexcept {
    return static_cast<bool>(requester_ptr_);
  }

  void reset() {
    requester_ptr_.reset();
  }

  void reset(const IRequest& request) {
    requester_ptr_.reset(new Requester(request));
  }

  template <typename ObjectType, typename... ResponseTypes>
  void reset(const IRequest& request, ObjectType* object, void(ObjectType::*...callback)(Requester*, const ResponseTypes&)) {
    static_assert(sizeof...(callback), "Must be at least one callback");

    requester_ptr_.reset(new Requester(request));

    const auto binder = [this, object](auto&& callback) {
      requester_ptr_->add_callback(object, callback);
    };

    MetaHelpers::call_for_all_arguments_in_pack(binder, std::forward<decltype(callback)>(callback)...);
  }

private:
  RequesterSharedPtr requester_ptr_;
};

}  // namespace MessageDispatcher
