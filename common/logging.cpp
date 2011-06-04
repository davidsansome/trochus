#ifndef WITHOUT_QT
  #include <QCoreApplication>
  #include <QDateTime>
  #include <QStringList>
#endif

#include <sstream>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "logging.h"


namespace logging {

const char* kDefaultLogLevels = "*:3";
static Level sDefaultLevel = Level_Debug;

#ifndef WITHOUT_QT
  static QMap<QString, Level>* sClassLevels = NULL;
  static QIODevice* sNullDevice = NULL;

  static const char* kMessageHandlerMagic = "__logging_message__";
  static const int kMessageHandlerMagicLength = strlen(kMessageHandlerMagic);
  static QtMsgHandler sOriginalMessageHandler = NULL;

  static void MessageHandler(QtMsgType type, const char* message) {
    if (strncmp(kMessageHandlerMagic, message, kMessageHandlerMagicLength) == 0) {
      fprintf(stderr, "%s\n", message + kMessageHandlerMagicLength);
      return;
    }

    Level level = Level_Debug;
    switch (type) {
      case QtFatalMsg:
      case QtCriticalMsg: level = Level_Error;   break;
      case QtWarningMsg:  level = Level_Warning; break;
      case QtDebugMsg:
      default:            level = Level_Debug;   break;
    }

    foreach (const QString& line, QString::fromLocal8Bit(message).split('\n')) {
      CreateLogger(level, "unknown", -1) << line.toLocal8Bit().constData();
    }

    if (type == QtFatalMsg) {
      abort();
    }
  }


  void Init() {
    delete sClassLevels;
    delete sNullDevice;

    sClassLevels = new QMap<QString, Level>();
    sNullDevice = new NullDevice;

    // Catch other messages from Qt
    if (!sOriginalMessageHandler) {
      sOriginalMessageHandler = qInstallMsgHandler(MessageHandler);
    }
  }

  void SetLevels(const QString& levels) {
    if (!sClassLevels)
      return;

    foreach (const QString& item, levels.split(',')) {
      const QStringList class_level = item.split(':');

      QString class_name;
      bool ok = false;
      int level = Level_Error;

      if (class_level.count() == 1) {
        level = class_level.last().toInt(&ok);
      } else if (class_level.count() == 2) {
        class_name = class_level.first();
        level = class_level.last().toInt(&ok);
      }

      if (!ok || level < Level_Error || level > Level_Debug) {
        continue;
      }

      if (class_name.isEmpty() || class_name == "*") {
        sDefaultLevel = (Level) level;
      } else {
        sClassLevels->insert(class_name, (Level) level);
      }
    }
  }
#else // WITHOUT_QT
  void Init() {
  }
#endif // WITHOUT_QT

QDebug CreateLogger(Level level, const char* pretty_function, int line) {
  // Map the level to a string
  const char* level_name = NULL;
  switch (level) {
    case Level_Debug:   level_name = " DEBUG "; break;
    case Level_Info:    level_name = " INFO  "; break;
    case Level_Warning: level_name = " WARN  "; break;
    case Level_Error:   level_name = " ERROR "; break;
  }

  // Get the class name out of the function name.
  std::string class_name(pretty_function);
  const size_t paren = class_name.find('(');
  if (paren != std::string::npos) {
    const int colons = class_name.rfind("::", paren);
    if (colons != std::string::npos) {
      class_name.erase(colons);
    } else {
      class_name.erase(paren);
    }
  }

  const size_t space = class_name.find(' ');
  if (space != std::string::npos) {
    class_name.erase(0, space);
  }

  // Check the settings to see if we're meant to show or hide this message.
  Level threshold_level = sDefaultLevel;
  #ifndef WITHOUT_QT
    if (sClassLevels && sClassLevels->contains(class_name)) {
      threshold_level = sClassLevels->value(class_name);
    }

    if (level > threshold_level) {
      return QDebug(sNullDevice);
    }
  #endif // WITHOUT_QT

  std::string function_line(class_name);
  if (line != -1) {
    std::ostringstream stream;
    stream << class_name << ":" << line;
    function_line = stream.str();
  }

  if (function_line.size() < 32) {
    function_line = function_line + std::string(32 - function_line.size(), ' ');
  }

  timeval tv;
  gettimeofday(&tv, NULL);

  tm timeinfo;
  localtime_r(&tv.tv_sec, &timeinfo);

  char time_str[128];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);

  int milliseconds = tv.tv_usec / 1000;

  #ifdef WITHOUT_QT
    QDebug ret;
    ret.nospace()
  #else
    QDebug ret(QtDebugMsg);
    ret.nospace() << kMessageHandlerMagic
  #endif
      << time_str << '.' << milliseconds << level_name << function_line;

  return ret.space();
}

} // namespace logging
