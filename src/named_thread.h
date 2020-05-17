#pragma once

namespace MessageDispatcher {

class NamedThread : public QThread {
 public:
  NamedThread(const QByteArray& name, QObject* parent = nullptr);

 protected:
  void run() override;
};

}  // namespace MessageDispatcher
