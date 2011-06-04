#ifndef SUPERVISORMANAGER_H
#define SUPERVISORMANAGER_H

#include <boost/noncopyable.hpp>

#include <list>
#include <signal.h>

class Supervisor;


class SupervisorManager {
public:
  SupervisorManager();
  ~SupervisorManager();

  int fd() const { return pipe_read_; }
  void ProcessEvents();

  // Called by Supervisor
  void RegisterSupervisor(Supervisor* supervisor);
  void UnregisterSupervisor(Supervisor* supervisor);

private:
  static SupervisorManager* sInstance;
  static void SignalHandler(int sig);

  struct sigaction old_action_;

  int pipe_read_;
  int pipe_write_;

  typedef std::list<Supervisor*> SupervisorList;
  SupervisorList supervisors_;
};

#endif // SUPERVISORMANAGER_H
