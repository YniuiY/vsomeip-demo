#ifndef SUB_H_
#define SUB_H_

#include <vsomeip/vsomeip.hpp>
#include <iostream>

namespace vsomeip_demo {

class Sub {
 public:
  Sub();
  ~Sub();
  void Init();
  void Start();

 private:
  void on_state_cbk(vsomeip::state_type_e state);
  void on_message_cbk(std::shared_ptr<vsomeip::message> const& msg);
  void on_available_cbk(vsomeip::service_t _service, vsomeip::instance_t _instance, bool is_avaliable);

  std::shared_ptr<vsomeip::application> app_;

  static int recv_num;
};

} // namespace vsomeip_demo

#endif //SUB_H_