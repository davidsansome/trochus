#include "messages.pb.h"
#include "messagingobject.h"

MessagingObject::MessagingObject(MessagingProcess* process)
  : process_(process),
    id_(process->CreateObjectId())
{
  process_->RegisterObject(this);
}

MessagingObject::~MessagingObject() {
  process_->UnregisterObject(this);
}

void MessagingObject::SendMessage(const protobuf::Address& destination,
                                  protobuf::Message* message) {
  *(message->mutable_to()) = destination;
  SendMessage(*message);
}

void MessagingObject::SendMessage(IdType destination_process,
                                  IdType destination_object,
                                  protobuf::Message* message) {
  message->mutable_to()->set_process(destination_process);
  message->mutable_to()->set_object(destination_object);
  SendMessage(*message);
}

void MessagingObject::SendMessage(const protobuf::Message& message) {
  message.PrintDebugString();
}
