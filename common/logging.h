#ifndef LOGGING_H
#define LOGGING_H

#ifndef WITHOUT_QT
  #include <QDebug>
#else
  #include <iostream>

  class QDebug {
  public:
    QDebug() : d(new Private) {}
    QDebug(const QDebug& o) : d(o.d) { d->ref_count_ ++; }
    ~QDebug() { if (--d->ref_count_ == 0) { std::cerr << '\n'; delete d; } }

    inline QDebug& space() { d->space_ = true; std::cerr << ' '; return *this; }
    inline QDebug& nospace() { d->space_ = false; return *this; }
    inline QDebug& maybeSpace() { if (d->space_) std::cerr << ' '; return *this; }

    template <typename T>
    inline QDebug& operator <<(const T& t) { std::cerr << t; return maybeSpace(); }

  private:
    struct Private {
      Private() : ref_count_(1), space_(true) {}

      int ref_count_;
      bool space_;
    };
    Private* d;
  };
#endif // WITHOUT_QT


#if defined(QT_NO_DEBUG_STREAM) && !defined(WITHOUT_QT)
#  define qLog(level) while (false) QNoDebug()
#else
#  define qLog(level) \
     logging::CreateLogger(logging::Level_##level, __PRETTY_FUNCTION__, __LINE__)
#endif

namespace logging {
  #ifndef WITHOUT_QT
    class NullDevice : public QIODevice {
    protected:
      qint64 readData(char*, qint64) { return -1; }
      qint64 writeData(const char*, qint64 len) { return len; }
    };

    void SetLevels(const QString& levels);
  #endif // WITHOUT_QT

  enum Level {
    Level_Error = 0,
    Level_Warning,
    Level_Info,
    Level_Debug,
  };

  void Init();

  QDebug CreateLogger(Level level, const char* pretty_function, int line);

  void GLog(const char* domain, int level, const char* message, void* user_data);

  extern const char* kDefaultLogLevels;
}

#endif // LOGGING_H
