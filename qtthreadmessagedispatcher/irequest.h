#pragma once

namespace MessageDispatcher {

enum class RequestType {
	RequestMathOperation
};

class IRequest {
 public:
  virtual ~IRequest() = default;
  virtual std::shared_ptr<IRequest> clone() const = 0;
  virtual RequestType request_type() const noexcept = 0;
};

}  // namespace MessageDispatcher
