#pragma once

namespace MessageDispatcher {

enum class ResponseType {
	ResponseTypeMathOperation
};

#define DECLARE_RESPONSE_STATIC_TYPE(Type) \
  static MessageDispatcher::ResponseType response_static_type();

#define DEFINE_RESPONSE_STATIC_TYPE(ObjectType, Type) \
  MessageDispatcher::ResponseType ObjectType::response_static_type() { \
    return Type; \
  }

#define DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Type) \
  static MessageDispatcher::ResponseType response_static_type() {\
    return Type; \
  }

class IResponse {
 public:
  virtual ~IResponse() = default;
  virtual ResponseType type() const noexcept = 0;
};

using IResponseSharedPtr = std::shared_ptr<IResponse>;

}  // namespace MessageDispatcher

Q_DECLARE_METATYPE(MessageDispatcher::IResponseSharedPtr);
