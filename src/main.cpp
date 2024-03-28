#include "request.h"
#include "response.h"
#include "sub.h"
#include "pub.h"
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./build/bin/run req/req\n";
    exit(1);
  }
  std::string h1{"req"};
  std::string h2{"rep"};
  std::string h3{"pub"};
  std::string h4{"sub"};

  for (int i = 1; i < argc; ++i) {
    if (h1.compare(argv[i]) == 0) {
      std::shared_ptr<vsomeip_demo::Request> request_node = std::make_shared<vsomeip_demo::Request>();
      request_node->Init();
      request_node->Start();
    } else if (h2.compare(argv[i]) == 0) {
      std::shared_ptr<vsomeip_demo::Response> response_node = std::make_shared<vsomeip_demo::Response>();
      response_node->Init();
      response_node->Start();
      response_node->Stop();
    } else if (h3.compare(argv[i]) == 0) {
      std::shared_ptr<vsomeip_demo::Pub> pub_node = std::make_shared<vsomeip_demo::Pub>();
      pub_node->Init();
      pub_node->Start();
    } else if (h4.compare(argv[i]) == 0) {
      std::shared_ptr<vsomeip_demo::Sub> sub_node = std::make_shared<vsomeip_demo::Sub>();
      sub_node->Init();
      sub_node->Start();
    }
  }
}