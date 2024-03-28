#include "sub.h"
#include "common_ids.h"

namespace vsomeip_demo {

int Sub::recv_num = 0;

Sub::Sub() {
  app_ = vsomeip::runtime::get()->create_application();
}

Sub::~Sub() {
  app_->unregister_state_handler();
  app_->unregister_message_handler(SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID);
  app_->unregister_availability_handler(SERVICE_ID, SERVICE_INSTANCE_ID);
}

void Sub::Init() {
  if (!app_->init()) {
    std::cerr << "app init failed\n";
    exit(1);
  }

  app_->register_state_handler(std::bind(&Sub::on_state_cbk, this, std::placeholders::_1));
  // sub侧，注册message回调时，method_t要填入订阅的event id，或者ANY_METHOD
  app_->register_message_handler(SERVICE_ID, SERVICE_INSTANCE_ID, vsomeip::ANY_METHOD, std::bind(&Sub::on_message_cbk, this, std::placeholders::_1));
  app_->register_availability_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID,
      std::bind(&Sub::on_available_cbk, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  
  std::set<vsomeip::eventgroup_t> event_group;
  event_group.insert(DEMO_EVENTGROUP_ID);
  std::cout << "app request event\n";
  app_->request_event(SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_EVENT_ID, event_group, vsomeip::event_type_e::ET_FIELD);
  std::cout << "app subscribe event group\n";
  // sub时要填入包含要订阅event id的event_group id
  app_->subscribe(SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_EVENTGROUP_ID);
}

void Sub::Start() {
  app_->start();
}

void Sub::on_state_cbk(vsomeip::state_type_e state) {
  if (state == vsomeip::state_type_e::ST_REGISTERED) {
    app_->request_service(SERVICE_ID, SERVICE_INSTANCE_ID);
  }
}

void Sub::on_available_cbk(vsomeip::service_t service, vsomeip::instance_t instance, bool is_avaliable) {
  if (service == SERVICE_ID && instance == SERVICE_INSTANCE_ID && is_avaliable) {
    std::cout << "Service is avaliable\n";
  }
}

void Sub::on_message_cbk(std::shared_ptr<vsomeip::message> const& msg) {
  std::cout << "\nService id: " << std::hex << msg->get_service()
            << ", Method id: " << std::hex << msg->get_method()
            << ", Client id: " << std::hex << msg->get_client()
            << ", Session id: " << std::hex << msg->get_session()
            << std::endl;
  std::shared_ptr<vsomeip::payload> payload = msg->get_payload();
  std::string payload_str{(char*)payload->get_data()};
  if (msg->get_method() == DEMO_SET_METHOD_ID) {
    int setter_num = (int)*payload->get_data();
    std::cout << "Recv setter response: " << setter_num;
  } else if (msg->get_method() == DEMO_GET_METHOD_ID) {
    int getter_num = (int)*payload->get_data();
    std::cout << "Recv getter result: " << getter_num;
  } else {
    std::cout << "Sub recv service notify msg: " << payload_str << std::endl;
  }

  recv_num++;
  if (recv_num % 10 == 0) {
    std::shared_ptr<vsomeip::message> setter_msg = vsomeip::runtime::get()->create_request();
    setter_msg->set_service(SERVICE_ID);
    setter_msg->set_instance(SERVICE_INSTANCE_ID);
    setter_msg->set_method(DEMO_SET_METHOD_ID);
    int set_num = recv_num;
    std::cout << "Set num is: " << set_num << std::endl;
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    payload->set_data((vsomeip::byte_t*)&set_num, sizeof(set_num));
    setter_msg->set_payload(payload);
    app_->send(setter_msg);
    std::cout << "Send set msg, set num: " << set_num << std::endl;

  } else if (recv_num % 15 == 0) {
    std::shared_ptr<vsomeip::message> getter_msg = vsomeip::runtime::get()->create_request();
    getter_msg->set_service(SERVICE_ID);
    getter_msg->set_instance(SERVICE_INSTANCE_ID);
    getter_msg->set_method(DEMO_GET_METHOD_ID);
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::string getter_str{"Tell me object num"};
    payload->set_data((vsomeip::byte_t*)getter_str.c_str(), getter_str.size());
    getter_msg->set_payload(payload);
    app_->send(getter_msg);
    std::cout << "Send get msg, " << getter_str << std::endl;
  }
}

} // namespace vsomeip_demo