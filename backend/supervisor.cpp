#include "logging.h"
#include "messages.pb.h"
#include "supervisor.h"
#include "supervisormanager.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include <fcntl.h>
#include <linux/user.h>
#include <sys/ptrace.h>


Supervisor::Supervisor(const std::list<std::string>& args,
                       SupervisorManager* manager, MessagingProcess* process)
  : MessagingObject(process),
    manager_(manager),
    first_process_starting_(false),
    args_(args)
{
  manager_->RegisterSupervisor(this);
}

Supervisor::~Supervisor() {
  manager_->UnregisterSupervisor(this);
}

void Supervisor::Start() {
  first_process_starting_ = true;
  pids_.clear();
  pid_t child = fork();
  pids_.push_back(child);

  if (child == 0) {
    // Create an argv array for the child
    char** argv = new char*[args_.size() + 1];

    // Fill the array
    char** p = argv;
    std::list<std::string>::const_iterator arg = args_.begin();
    for ( ; arg != args_.end() ; ++arg, ++p) {
      *p = strdup(arg->c_str());
    }
    *p = NULL;

    // Start the child process
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[0], argv);
    _exit(0);
  }

  // HandleStop will be called when the child gets to PTRACE_TRACEME
}

bool Supervisor::IsChildPid(int pid) const {
  return std::find(pids_.begin(), pids_.end(), pid) != pids_.end();
}

void Supervisor::HandleExit(int pid) {
  // Send the exit message
  protobuf::Message msg;

  protobuf::CommandProcessExit* exit_msg = msg.mutable_command_process_exit();
  exit_msg->set_pid(pid);

  SendMessage(msg);

  // Remove the PID from our list
  std::list<int>::iterator it = std::find(pids_.begin(), pids_.end(), pid);
  if (it != pids_.end()) {
    pids_.erase(it);
  }
}

void Supervisor::HandleStop(int pid, int status) {
  const int signal = WSTOPSIG(status);

  if (signal == SIGTRAP) {
    const int ptrace_event = (status & 0xf0000) >> 16;

    if (first_process_starting_) {
      // This is the very first process reaching the PTRACE_TRACEME call.  Set
      // some options on it to trace subsequent forks and execs.
      if (ptrace(PTRACE_SETOPTIONS, pid, NULL,
                 PTRACE_O_TRACEEXEC | PTRACE_O_TRACEFORK |
                 PTRACE_O_TRACEVFORK) < 0) {
        qLog(Warning) << "SETOPTIONS failed on child PID" << pid;
      }
    }

    switch (ptrace_event) {
    case PTRACE_EVENT_EXEC: {
      // Create the exec message
      protobuf::Message msg;
      protobuf::CommandProcessExec* exec_msg = msg.mutable_command_process_exec();
      exec_msg->set_pid(pid);

      // Read the /proc/pid/cmdline file to get the list of arguments
      std::ostringstream filename;
      filename << "/proc/" << pid << "/cmdline";
      const int fd = open(filename.str().c_str(), 0);

      if (fd == -1) {
        qLog(Warning) << "Failed to open" << filename;
        break;
      }

      std::string contents;
      char buf[512];
      while (true) {
        const int bytes_read = read(fd, buf, sizeof(buf));
        if (bytes_read == 0) {
          break;
        }
        contents.append(buf, bytes_read);
      }

      close(fd);

      // Split on \0
      for (int i=0, tok_begin=0 ; i<contents.size() ; ++i) {
        if (contents[i] == '\0') {
          exec_msg->add_arg(contents.substr(tok_begin, i - tok_begin));
          tok_begin = i + 1;
        }
      }

      SendMessage(msg);
      break;
    }

    case PTRACE_EVENT_FORK:
    case PTRACE_EVENT_VFORK: {
      // Get the PID of the child
      unsigned long new_pid = -1;
      ptrace(PTRACE_GETEVENTMSG, pid, 0, &new_pid);

      // The child starts in a stopped state - continue it now.
      if (ptrace(PTRACE_CONT, new_pid, NULL, NULL) < 0) {
        qLog(Warning) << "Continuing child PID" << new_pid << "after stop failed";
      }

      // Record the child's PID
      pids_.push_back(new_pid);

      // Send the fork message
      protobuf::Message msg;
      protobuf::CommandProcessFork* fork_msg = msg.mutable_command_process_fork();
      fork_msg->set_parent_pid(pid);
      fork_msg->set_pid(new_pid);

      SendMessage(msg);
      break;
    }
    }
  }

  // Continue the stopped process.
  if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
    qLog(Warning) << "Continuing PID" << pid << "after stop failed";
  }
}

void Supervisor::HandleMessage(const protobuf::Message& message) {

}
