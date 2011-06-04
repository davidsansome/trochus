#include "logging.h"
#include "messagingobject.h"
#include "messagingprocess.h"
#include "messages.pb.h"

#include <cassert>


MessagingProcess::MessagingProcess(IdType id)
  : process_id_(id)
{
}

IdType MessagingProcess::CreateObjectId() {
  return next_object_id_ ++;
}

void MessagingProcess::RegisterObject(MessagingObject* object) {
  assert(local_objects_.find(object->object_id()) == local_objects_.end());
  local_objects_[object->object_id()] = object;

  qLog(Debug) << "Registered" << process_id() << object->object_id();
}

void MessagingProcess::UnregisterObject(MessagingObject* object) {
  ObjectMap::iterator it = local_objects_.find(object->object_id());
  assert(it != local_objects_.end());

  local_objects_.erase(it);

  qLog(Debug) << "Unregistered" << process_id() << object->object_id();
}

void MessagingProcess::RouteMessage(const protobuf::Message& message) {
  assert(message.has_to());
  assert(message.has_from());

  if (message.to().process() != process_id_) {
    qLog(Warning) << "Out of process routing not supported yet";
    return;
  }

  ObjectMap::iterator it = local_objects_.find(message.to().object());
  if (it == local_objects_.end()) {
    qLog(Warning) << "Local object" << message.to().object() << "not found";
    return;
  }

  it->second->HandleMessage(message);
}
