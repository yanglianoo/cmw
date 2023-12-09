/**
 * @File Name: participant.cpp
 * @brief  
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2023-12-06
 * 
 */

#include <cmw/transport/rtps/participant.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/attributes/RTPSParticipantAttributes.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>

namespace hnu    {
namespace cmw   {
namespace transport {

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

Participant::Participant(const std::string& name, int send_port,
                         eprosima::fastrtps::rtps::RTPSParticipantListener* listener)
    : shutdown_(false),
      name_(name),
      send_port_(send_port),
      listener_(listener),
      fastrtps_participant_(nullptr) {}


Participant::~Participant() {}

//移除具体的fastrtps_participant
void Participant::Shutdown() {
  if (shutdown_.exchange(true)) {
    return;
  }

  std::lock_guard<std::mutex> lk(mutex_);
  if (fastrtps_participant_ != nullptr) {
    eprosima::fastrtps::rtps::RTPSDomain::removeRTPSParticipant(fastrtps_participant_);
    fastrtps_participant_ = nullptr;
    listener_ = nullptr;
  }
}

eprosima::fastrtps::rtps::RTPSParticipant*  Participant::fastrtps_participant() {

  if (shutdown_.load()) {
    return nullptr;
  }

  std::lock_guard<std::mutex> lk(mutex_);
  if (fastrtps_participant_ != nullptr) {
    return fastrtps_participant_;
  }

  CreateFastRtpsParticipant(name_, send_port_, listener_);
  return fastrtps_participant_;
}

void CreateFastRtpsParticipant(
      const std::string& name, int send_port,
      eprosima::fastrtps::rtps::RTPSParticipantListener* listener){

      uint32_t domain_id = 80;  //默认的domain_id为80
      // cyberrt在创建RtpsParticipant时的配置是通过配置文件传进来的，我这里就按照默认设置了，后续再拓展
     
      // 默认是udp
      RTPSParticipantAttributes PParam;

      PParam.builtin.use_WriterLivelinessProtocol  = true;
      PParam.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
      PParam.builtin.discovery_config.discoveryProtocol = eprosima::fastrtps::rtps::DiscoveryProtocol::SIMPLE;
      
      PParam.setName(name.c_str());

      // 默认端口为 0
      Locator_t loc;

      loc.kind = LOCATOR_KIND_UDPv4;

      // IP地址设置需要配置
      IPLocator::setIPv4(loc,"192.168.0.1");

      //单播配置
      PParam.defaultUnicastLocatorList.push_back(loc);
      PParam.builtin.metatrafficUnicastLocatorList.push_back(loc);

      //本地多播设置
      IPLocator::setIPv4(loc,"239.255.0.1");
      PParam.builtin.metatrafficMulticastLocatorList.push_back(loc);
      PParam.defaultMulticastLocatorList.push_back(loc);

      //创建participant_
      fastrtps_participant_ = RTPSDomain::createParticipant(domain_id,PParam,listener);

}

}
}
}
