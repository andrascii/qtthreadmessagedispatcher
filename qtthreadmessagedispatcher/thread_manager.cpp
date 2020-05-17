#include "stdafx.h"
#include "thread_manager.h"
#include "named_thread.h"

namespace {

MessageDispatcher::ThreadManager* s_threadManager = nullptr;

}  // namespace

namespace MessageDispatcher {

ThreadManager::~ThreadManager() {
  foreach(const ObjectsThreadAffinity& objectsThreadAffinity, objects_thread_affinity_) {
    //
    // delete objects in another thread
    // and wait their destruction
    //
    delete_all_later(objectsThreadAffinity.objects);
    qApp->processEvents();

    objectsThreadAffinity.thread->quit();
    objectsThreadAffinity.thread->wait();

    delete objectsThreadAffinity.thread;
  }
}

void ThreadManager::move_object_to_thread(QObject* object, const QByteArray& thread_name) {
  if (objects_thread_affinity_.contains(thread_name)) {
    if (objects_thread_affinity_[thread_name].objects.contains(object)) {
      return;
    }

    object->moveToThread(objects_thread_affinity_[thread_name].thread);
    objects_thread_affinity_[thread_name].objects.append(object);
    return;
	}

  objects_thread_affinity_[thread_name].thread = new MessageDispatcher::NamedThread(thread_name);
  object->moveToThread(objects_thread_affinity_[thread_name].thread);
  objects_thread_affinity_[thread_name].objects.append(object);
  objects_thread_affinity_[thread_name].thread->start();
}

void ThreadManager::delete_all_later(const QList<QObject*>& objects) {
  foreach(QObject* qobject, objects) {
    qobject->deleteLater();
  }
}

MessageDispatcher::ThreadManager& ThreadManager::instance() {
  if (!s_threadManager) {
    s_threadManager = new ThreadManager;
  }

  return *s_threadManager;
}

void ThreadManager::destroy() {
  delete s_threadManager;
  s_threadManager = nullptr;
}

}  // namespace MessageDispatcher
