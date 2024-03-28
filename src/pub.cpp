#include "pub.h"
#include <iostream>
#include <thread>

namespace vsomeip_demo {

Pub::Pub():is_offered_{false}, is_running_{true}, object_num_{0} {
  app_ = vsomeip::runtime::get()->create_application();

  std::thread([this]{
    std::string hello{"Hello Sub"};
    int i = 0;
    std::string msg;
    while (is_running_) {
      /* code */
      msg = hello + std::to_string(i++);
      std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
      payload->set_data((vsomeip::byte_t*)msg.c_str(), msg.size());
      app_->notify(SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_EVENT_ID, payload);
      std::cout << "notify msg: " << msg << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }).detach();
}

Pub::~Pub() {
  is_running_ = false;
  app_->unregister_state_handler();
  app_->unregister_message_handler(SERVICE_ID, SERVICE_INSTANCE_ID, METHOD_ID);
}

void Pub::Init() {
  if (!app_->init()) {
    std::cerr << "app init failed\n";
    exit(1);
  }

  app_->register_state_handler(std::bind(&Pub::on_state_cbk, this, std::placeholders::_1));
  app_->register_message_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID, vsomeip::ANY_METHOD,
      std::bind(&Pub::on_message_cbk, this, std::placeholders::_1));
  app_->register_message_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_GET_METHOD_ID,
      std::bind(&Pub::on_get_cbk, this, std::placeholders::_1));
  app_->register_message_handler(
      SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_SET_METHOD_ID,
      std::bind(&Pub::on_set_cbk, this, std::placeholders::_1));

  std::set<vsomeip::eventgroup_t> event_group;
  event_group.insert(DEMO_EVENTGROUP_ID);
  app_->offer_event(SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_EVENT_ID, event_group, vsomeip::event_type_e::ET_FIELD);
  app_->offer_service(SERVICE_ID, SERVICE_INSTANCE_ID);
  // is_offered_ = true;
}

void Pub::Start() {
  app_->start();
}

void Pub::on_state_cbk(vsomeip::state_type_e state) {
  if (state == vsomeip::state_type_e::ST_REGISTERED) {
    
  }
}

void Pub::on_message_cbk(std::shared_ptr<vsomeip::message> const& msg) {

}

void Pub::on_get_cbk(std::shared_ptr<vsomeip::message> const& msg) {
  // 解析getter请求
  std::shared_ptr<vsomeip::payload> payload = msg->get_payload();
  std::string getter_str{(char*)payload->get_data()};
  std::cout << "Client Getter request msg: " << getter_str << std::endl;

  // 回复Getter请求
  std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(msg);
  payload->set_data((vsomeip::byte_t*)&object_num_, sizeof(object_num_));
  response->set_payload(payload);
  app_->send(response);
}

void Pub::on_set_cbk(std::shared_ptr<vsomeip::message> const& msg) {
  // 解析setter请求
  std::shared_ptr<vsomeip::payload> payload = msg->get_payload();
  object_num_ = (int)*payload->get_data();
  std::cout << "Client Setter object num: " << object_num_ << std::endl;

  std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(msg);
  response->set_payload(payload);
  // 回馈setter方setter结果
  app_->send(response);

  // 本地有内容更新，pub更新结果
  app_->notify(SERVICE_ID, SERVICE_INSTANCE_ID, DEMO_EVENT_ID, payload);
}

} // namespace vsomeip_demo