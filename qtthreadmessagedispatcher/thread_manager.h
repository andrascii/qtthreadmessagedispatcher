#pragma once

namespace MessageDispatcher {

//
// Deleting objects stored in ThreadManager is in responsibilities of ThreadManager
//

class ThreadManager final {
 public:
  static ThreadManager& instance();
  static void destroy();

  void move_object_to_thread(QObject* object, const QByteArray& thread_name);

 private:
  ThreadManager() = default;
  ~ThreadManager();
  static void delete_all_later(const QList<QObject*>& objects);

 private:
  struct ObjectsThreadAffinity {
    QList<QObject*> objects;
    QThread* thread;
  };

  QMap<QByteArray, ObjectsThreadAffinity> objects_thread_affinity_;
};

}  // namespace MessageDispatcher
