#include "request.h"
#include <iostream>

namespace vsomeip_demo {

Request::Request(): app_{nullptr} {
  app_ = vsomeip::runtime::get()->create_application();
}

Request::~Request() {
  app_->unregister_state_handler();
  app_->unregister_availability_handler(SERVICE_ID, SERVICE_INSTANCE_ID);
  app_->unregister_message_handler(SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID);
}

bool Request::Init() {
  bool ret{false};
  if (!app_->init()) {
    return ret;
  }

  app_->register_state_handler(
      std::bind(&Request::on_state_cbk, this, std::placeholders::_1));
  app_->register_availability_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID,
      std::bind(&Request::on_avaliable_cbk, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  app_->register_message_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID,
      std::bind(&Request::on_message_cbk, this, std::placeholders::_1));

  return true;
}

void Request::Start() {
  app_->start();
}

void Request::on_state_cbk(vsomeip::state_type_e state) {
  if (state == vsomeip::state_type_e::ST_REGISTERED) {
    std::cout << "Request Service\n";
    app_->request_service(SERVICE_ID, SERVICE_INSTANCE_ID);
  } else {
    std::cerr << "State not ST_REGISTERED\n";
  }
}

void Request::on_avaliable_cbk(vsomeip::service_t service_id, vsomeip::instance_t instance_id, bool is_avaliable) {
  if (service_id == SERVICE_ID && instance_id == SERVICE_INSTANCE_ID && is_avaliable) {
    // todo 发送request请求到service
    std::shared_ptr<vsomeip::message> request_ptr = vsomeip::runtime::get()->create_request();
    request_ptr->set_service(SERVICE_ID);
    request_ptr->set_instance(SERVICE_INSTANCE_ID);
    request_ptr->set_method(METHOD_ID);

    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::string msg{"World"};
    payload->set_data((vsomeip::byte_t*)msg.c_str(), msg.size());

    request_ptr->set_payload(payload);

    app_->send(request_ptr);
    std::cout << "Send Request\n";
  }
}

void Request::on_message_cbk(std::shared_ptr<vsomeip::message> const& response) {
  // todo 解析Server的response
  if (response->get_service() == SERVICE_ID &&
      response->get_instance() == SERVICE_INSTANCE_ID &&
      response->get_method() == METHOD_ID) {
    auto payload = response->get_payload();
    std::string rep_msg{(char*)payload->get_data()};
    std::cout << "Recv Server Rep msg: " << rep_msg << std::endl;
  } else {
    std::cerr << "Recv server rep not match, service id: " << response->get_service()
              << ", instance id: " << response->get_instance()
              << ", method id: " << response->get_method() << std::endl;
  }
}

} // namespace vsomeip_demo