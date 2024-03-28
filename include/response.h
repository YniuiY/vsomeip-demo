#ifndef RESPINSE_H_
#define RESPINSE_H_

#include <vsomeip/vsomeip.hpp>
#include "common_ids.h"

namespace vsomeip_demo {

class Response {
 public:
  Response();
  ~Response();

  bool Init();
  void Start();
  void Stop();

 private:
  void on_state_cbk(vsomeip::state_type_e state);
  void on_message_cbk(std::shared_ptr<vsomeip::message> const& msg);
  
  std::shared_ptr<vsomeip::application> app_;
};

} //vsomeip_demo

#endif // RESPINSE_H_