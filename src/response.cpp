#include "response.h"
#include <iostream>

namespace vsomeip_demo {

Response::Response(){
  app_ = vsomeip::runtime::get()->create_application();
}

Response::~Response() {

}

bool Response::Init() {
  if (!app_->init()) {
    std::cerr << "app init failed\n";
    return false;
  }

  app_->register_state_handler(std::bind(&Response::on_state_cbk, this, std::placeholders::_1));
  app_->register_message_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID,
      std::bind(&Response::on_message_cbk, this, std::placeholders::_1));

  return true;
}

void Response::Start() {
  app_->start();
}

void Response::Stop() {
  app_->stop_offer_service(SERVICE_ID, SERVICE_INSTANCE_ID);
  app_->unregister_state_handler();
  app_->unregister_message_handler(SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID);
  app_->stop();
}

void Response::on_state_cbk(vsomeip::state_type_e state) {
  if (state == vsomeip::state_type_e::ST_REGISTERED) {
    app_->offer_service(SERVICE_ID, SERVICE_INSTANCE_ID);
  }
}

void Response::on_message_cbk(std::shared_ptr<vsomeip::message> const& request) {
  std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(request);
  std::string msg{"Hello "};
  std::string request_str{(char*)request->get_payload()->get_data()};
  std::cout << "Recv request: " << request_str << std::endl;
  msg += request_str;
  std::cout << "Create response msg: " << msg << std::endl;

  std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
  payload->set_data((vsomeip::byte_t*)msg.c_str(), msg.size());
  response->set_payload(payload);

  app_->send(response);
}

} // namespace vsomeip_demo