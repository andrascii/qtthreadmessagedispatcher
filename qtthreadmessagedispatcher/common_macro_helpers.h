#pragma once

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define ASSERT_WITH_DUMP(condition, ...)
#define ASSERT(condition) (void)(condition)

// Additional parameters must support << operation to print it
#ifdef QT_DEBUG
#define DEBUG_ASSERT_WITH_DUMP(condition, ...) ASSERT_WITH_DUMP(condition, __VA_ARGS__)
#else
#define DEBUG_ASSERT_WITH_DUMP(condition)
#endif

#ifdef QT_DEBUG
#define DEBUG_ASSERT(condition) ASSERT(condition)
#else
#define DEBUG_ASSERT(condition)
#endif

#if defined(QT_DEBUG)
#define VERIFY(Connection) Connection
#endif

#ifndef UNIX_TEMPORARY_LOGS_STUB
#define UNIX_TEMPORARY_LOGS_STUB

struct UnixTemporaryLogsStub {
  template <typename T>
  UnixTemporaryLogsStub& operator<<(const T&) {
      return *this;
  }
};

#endif

#define DEBUGLOG UnixTemporaryLogsStub()
#define INFOLOG UnixTemporaryLogsStub()
#define WARNLOG UnixTemporaryLogsStub()
#define ERRLOG UnixTemporaryLogsStub()
