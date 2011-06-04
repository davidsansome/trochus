#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "messagingobject.h"

#include <list>
#include <string>

class SupervisorManager;


class Supervisor : public MessagingObject {
public:
  Supervisor(const std::list<std::string>& args,
             SupervisorManager* manager, MessagingProcess* process);
  ~Supervisor();

  void Start();

  // Called by SupervisorManager
  bool IsChildPid(int pid) const;
  void HandleStop(int pid, int status);
  void HandleExit(int pid);

protected:
  // MessagingObject
  void HandleMessage(const protobuf::Message& message);

private:
  SupervisorManager* manager_;

  std::list<std::string> args_;

  bool first_process_starting_;
  std::list<int> pids_;
};

#endif // SUPERVISOR_H
