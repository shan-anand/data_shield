#pragma once

#include <grpcpp/grpcpp.h>

namespace _async {
// Common abstract base for all CallData types used by async services.
// Allows poller threads to cast CQ tags to a single pointer type and invoke Proceed()/Cancel().
class CallBase
{
public:
  explicit CallBase(grpc::ServerCompletionQueue *cq) : cq_(cq), status_(CREATE) {}
  virtual ~CallBase() = default;

  // Lifecycle driver used by poller threads.
  // Implements: CREATE -> PROCESS -> FINISH -> delete
  void Proceed()
  {
    if (status_ == CREATE)
    {
      status_ = PROCESS;
      RequestRPC();
    }
    else if (status_ == PROCESS)
    {
      rearm();
      OnProcess();
      status_ = FINISH;
    }
    else
    { // FINISH
      delete this;
    }
  }

  virtual void Cancel() { delete this; }

protected:
  virtual void RequestRPC() = 0;
  virtual void OnProcess() = 0;
  virtual void rearm() {}

  grpc::ServerContext ctx_;
  grpc::ServerCompletionQueue *cq_;
  enum CallStatus
  {
    CREATE,
    PROCESS,
    FINISH
  };
  CallStatus status_;
};

} // namespace async
