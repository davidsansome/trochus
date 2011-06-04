#ifndef MESSAGINGOBJECT_H
#define MESSAGINGOBJECT_H

#include <boost/noncopyable.hpp>

#include "messagingprocess.h"

class MessagingObject : private boost::noncopyable {
  friend class MessagingProcess;

public:
  MessagingObject(MessagingProcess* process);
  virtual ~MessagingObject();

  IdType object_id() const { return id_; }
  IdType process_id() const { return process_->process_id(); }

protected:
  void SendMessage(const protobuf::Address& destination,
                   protobuf::Message* message);
  void SendMessage(IdType destination_process, IdType destination_object,
                   protobuf::Message* message);
  void SendMessage(const protobuf::Message& message);

  virtual void HandleMessage(const protobuf::Message& message) = 0;

private:
  MessagingProcess* process_;
  IdType id_;
};

#endif // MESSAGINGOBJECT_H
