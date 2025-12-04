#include "server/server.h"

#include <thread>

Server::Server() :
  address("0.0.0.0:50051"),
  creds(grpc::InsecureServerCredentials()),
  poller_threads(4),
  initial_slots(4)
{
}

Server::~Server()
{
}

void Server::run_sync()
{
  auto common_helper = std::make_shared<CommonHelper>();
  auto block_helper = std::make_shared<ComponentBlockHelper>();

  auto common_service = std::make_shared<_sync::CommonService>(common_helper);
  auto comp_service = std::make_shared<_sync::ComponentBlockService>(block_helper);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(this->address, this->creds);
  builder.RegisterService(common_service.get());
  builder.RegisterService(comp_service.get());

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Synchronous server listening on " << this->address << std::endl;
  server->Wait();
}

void Server::run_async()
{
  auto common_helper = std::make_shared<CommonHelper>();
  auto block_helper = std::make_shared<ComponentBlockHelper>();

  _async::CommonService common_async(common_helper);
  _async::ComponentBlockService comp_async(block_helper);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(this->address, this->creds);

  builder.RegisterService(common_async.service());
  builder.RegisterService(comp_async.service());

  std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Asynchronous server listening on " << this->address << std::endl;

  common_async.Start(cq.get(), std::max(1, this->initial_slots / 2));
  comp_async.Start(cq.get(), this->initial_slots);

  std::vector<std::thread> pollers;
  pollers.reserve(std::max(1, this->poller_threads));
  for (int i = 0; i < this->poller_threads; ++i)
  {
    pollers.emplace_back(
      [cq_ptr = cq.get()] {
        void* tag;
        for (bool ok; cq_ptr->Next(&tag, &ok); )
        {
          auto call = static_cast<_async::CallBase*>(tag);
          ok? call->Proceed() : call->Cancel();
        }
      });
  }

  for (auto &t : pollers)
  {
    if (t.joinable())
      t.join();
  }
}
