#ifndef PUB_H_
#define PUB_H_

#include <vsomeip/vsomeip.hpp>
#include "common_ids.h"

namespace vsomeip_demo {

class Pub {
 public:
  Pub();
  ~Pub();
  void Init();
  void Start();

 private:
  void on_state_cbk(vsomeip::state_type_e state);
  void on_message_cbk(std::shared_ptr<vsomeip::message> const& msg);
  void on_set_cbk(std::shared_ptr<vsomeip::message> const& msg);
  void on_get_cbk(std::shared_ptr<vsomeip::message> const& msg);

  void pub_msg();

  std::shared_ptr<vsomeip::application> app_;
  bool is_offered_;
  bool is_running_;
  int object_num_;
};

} // namespace vsomeip_demo


#endif // PUB_H_