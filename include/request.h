#ifndef REQUEST_H_
#define REQUEST_H_

#include <vsomeip/vsomeip.hpp>
#include "common_ids.h"

namespace vsomeip_demo {

class Request {
 public:
  Request();
  ~Request();

  bool Init();
  void Start();

 private:
  // vsomeip runtime可用后触发
  void on_state_cbk(vsomeip::state_type_e state);

  // service上线后触发回调
  void on_avaliable_cbk(vsomeip::service_t service_id, vsomeip::instance_t instance_id, bool is_avaliable);

  // 有消息到达是触发回调
  void on_message_cbk(std::shared_ptr<vsomeip::message> const& mest);

  std::shared_ptr<vsomeip::application> app_;
};

}// vsomeip_demo

#endif // REQUEST_H_