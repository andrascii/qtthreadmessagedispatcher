#pragma once

namespace MessageDispatcher {

class IResponse;

class Delegate {
 public:
  typedef std::function<void(const IResponse&)> Callback;

  void clear();
  void add(const Callback& callback);
  void operator()(const IResponse& response) const;

 private:
  std::vector<Callback> callbacks_;
};

}  // namespace MessageDispatcher
