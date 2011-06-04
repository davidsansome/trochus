#ifndef MESSAGINGPROCESS_H
#define MESSAGINGPROCESS_H

#include <boost/noncopyable.hpp>
#include <map>

namespace protobuf {
  class Address;
  class Message;
}

class MessagingObject;


typedef unsigned int IdType;

class MessagingProcess : private boost::noncopyable {
  friend class MessagingObject;

public:
  MessagingProcess(IdType id = 0);

  IdType process_id() const { return process_id_; }

  void RouteMessage(const protobuf::Message& message);

private:
  // To be used by MessagingObject
  IdType CreateObjectId();

  void RegisterObject(MessagingObject* object);
  void UnregisterObject(MessagingObject* object);

private:
  IdType process_id_;
  IdType next_object_id_;

  typedef std::map<IdType, MessagingObject*> ObjectMap;
  ObjectMap local_objects_;
};

#endif // MESSAGINGPROCESS_H
