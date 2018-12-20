//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// connection_handle_factory.cpp
//
// Identification: src/network/connection_handle_factory.cpp
//
// Copyright (c) 2015-2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <utility>
#include "network/connection_handle_factory.h"
#include "common/macros.h"

namespace terrier {
namespace network {
ConnectionHandle &ConnectionHandleFactory::NewConnectionHandle(int conn_fd, ConnectionHandlerTask *task) {
  auto it = reusable_handles_.find(conn_fd);
  if (it == reusable_handles_.end()) {
    auto ret = reusable_handles_.emplace(std::piecewise_construct,
                                         std::forward_as_tuple(conn_fd),
                                         std::forward_as_tuple(conn_fd, task));
    TERRIER_ASSERT(ret.second, "ret.second false");
    return ret.first->second;
  }

  auto &reused_handle= it->second;
  reused_handle.conn_handler_ = task;
  reused_handle.network_event_ = nullptr;
  reused_handle.workpool_event_ = nullptr;
  /*reused_handle.io_wrapper_.reset(new PosixSocketIoWrapper(std::move(
      reused_handle.io_wrapper_.release())));*/
  //reused_handle.protocol_interpreter_.reset(new PostgresProtocolInterpreter(task->Id()));
  reused_handle.state_machine_= ConnectionHandle::StateMachine();
  TERRIER_ASSERT(reused_handle.network_event_ == nullptr, "network_event_ != nullptr");
  TERRIER_ASSERT(reused_handle.workpool_event_ == nullptr, "network_event_ != nullptr");
  return reused_handle;
}
}  // namespace network
}  // namespace terrier
