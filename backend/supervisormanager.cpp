#include <algorithm>
#include <boost/bind.hpp>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logging.h"
#include "supervisor.h"
#include "supervisormanager.h"

SupervisorManager* SupervisorManager::sInstance = NULL;

SupervisorManager::SupervisorManager() {
  assert(!sInstance);
  sInstance = this;

  int pipefds[2];
  pipe2(pipefds, O_NONBLOCK | O_CLOEXEC);
  pipe_read_ = pipefds[0];
  pipe_write_ = pipefds[1];

  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = SignalHandler;
  sigaction(SIGCHLD, &action, &old_action_);
}

SupervisorManager::~SupervisorManager() {
  sigaction(SIGCHLD, &old_action_, NULL);

  close(pipe_read_);
  close(pipe_write_);

  sInstance = NULL;
}

void SupervisorManager::SignalHandler(int sig) {
  write(sInstance->pipe_write_, "\0", 1);

  if (sInstance->old_action_.sa_handler) {
    sInstance->old_action_.sa_handler(sig);
  }
}

void SupervisorManager::RegisterSupervisor(Supervisor* supervisor) {
  assert(std::find(supervisors_.begin(), supervisors_.end(), supervisor)
          == supervisors_.end());
  supervisors_.push_back(supervisor);
}

void SupervisorManager::UnregisterSupervisor(Supervisor* supervisor) {
  SupervisorList::iterator it = std::find(
        supervisors_.begin(), supervisors_.end(), supervisor);
  assert(it != supervisors_.end());

  supervisors_.erase(it);
}

void SupervisorManager::ProcessEvents() {
  // Read all the pending data from the pipe.  Each byte is a SIGCHLD, but
  // paying any attention to how many there are is dangerous.
  char buf = '\0';
  do {} while(read(pipe_read_, &buf, 1) == 1);

  // Now get all the pending signals.  If another one happens while we're
  // in this loop it's fine - we'll process it now , the signal handler will
  // write another byte, and this method will get called again but see nothing.
  while (true) {
    int status = 0;
    pid_t pid = waitpid(-1, &status, WNOHANG);

    if (pid <= 0) {
      // There were no more pending signals.
      break;
    }

    // Find a supervisor that owns this PID.
    SupervisorList::iterator supervisor = std::find_if(
          supervisors_.begin(), supervisors_.end(), boost::bind(
                &Supervisor::IsChildPid, _1, pid));

    if (supervisor == supervisors_.end()) {
      // Unknown PID - this is probably a newly forked child, we'll see the
      // parent's TRAP soon and resume the child then.
      continue;
    }

    // Let the supervisor handle its child.
    if (WIFEXITED(status)) {
      (*supervisor)->HandleExit(pid);
    } else if (WIFSTOPPED(status)) {
      (*supervisor)->HandleStop(pid, status);
    }
  }
}
