
#include <cmw/common/global_data.h>
#include <sys/types.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <cstdlib>
#include <netdb.h>
#include <cmw/common/file.h>
#include <cmw/common/util.h>
namespace hnu {
namespace cmw {
namespace common {

//类中静态成员是类的一部分，但是不是类的实例的一部分，因此需要在此定义
AtomicHashMap<uint64_t, std::string, 256> GlobalData::channel_id_map_;
AtomicHashMap<uint64_t, std::string, 512> GlobalData::node_id_map_;

namespace{
//返回当前执行的进程的路径
const std::string& kEmptyString = "";
std::string program_path(){
    char path[4096];
    auto len = readlink("proc/self/exe", path , sizeof(path) - 1);
    if(len == -1 ){
        return kEmptyString;
    }

    path[len] = '\0';

    return std::string(path);
}
}

GlobalData::GlobalData() {
    InitHostInfo();

    //获取进程ID
    porcess_id_ = getpid();

    auto prog_path = program_path();

    if(!prog_path.empty()){
        process_group_ = GetFileName(prog_path) + "_" + std::to_string(porcess_id_);
    }else{
        process_group_ = "cmw_default_" + std::to_string(porcess_id_);
    }

}

void GlobalData::InitHostInfo() {
    char host_name[1024];
    gethostname(host_name , sizeof(host_name));
    host_name_ = host_name;

    // 使用本地回环ip地址
    host_ip_ = "127.0.0.1";

    const char* ip_env = getenv("CMW_IP");
    if(ip_env != nullptr)
    {
        std::string ip_env_str(ip_env);
        //取IP前三个字符
        std::string starts = ip_env_str.substr(0 , 3);
        if(starts != "127"){
            host_ip_ = ip_env_str;
            return;
        }
    }

    ifaddrs* ifaddr = nullptr;
    if(getifaddrs(&ifaddr) != 0)
    {
        std::cout << "getifaddrs failed, we will use 127.0.0.1 as host ip." << std::endl;
    }
    //ifaddrs 是一个链表，遍历每个节点
    for(ifaddrs* ifa = ifaddr; ifa ; ifa = ifa->ifa_next){

        if(ifa->ifa_addr == nullptr){
            continue;
        }
        int family = ifa->ifa_addr->sa_family;
        if(family  != AF_INET){
            continue;
        }

        //获取本机的IP地址
        char addr[NI_MAXHOST] = {0};
        if(getnameinfo(ifa->ifa_addr , sizeof(sockaddr_in) , addr , NI_MAXHOST , NULL , 
                       0 , NI_NUMERICHOST) != 0){
            continue;
        }

        std::string tmp_ip(addr);
        std::string starts = tmp_ip.substr(0,3);
        if(starts != "127"){
            host_ip_ = tmp_ip;
            break;
        }
    }

    //free 掉链表
    freeifaddrs(ifaddr);

}

GlobalData::~GlobalData() {}
int GlobalData::ProcessId() const { return porcess_id_; }
void GlobalData::SetProcessGroup(const std::string& process_group) {
  process_group_ = process_group;
}
const std::string& GlobalData::ProcessGroup() const { return process_group_; }


const std::string& GlobalData::HostIp() const { return host_ip_; }
const std::string& GlobalData::HostName() const { return host_name_; }


uint64_t GlobalData::RegisterChannel(const std::string& channel) {
  auto id = Hash(channel);
  while (channel_id_map_.Has(id)) {
    std::string* name = nullptr;
    channel_id_map_.Get(id, &name);
    if (channel == *name) {
      break;
    }
    ++id;
    std::cout << "Channel name hash collision: " << channel << " <=> " << *name;
  }
  channel_id_map_.Set(id, channel);
  return id;
}

uint64_t GlobalData::RegisterNode(const std::string& node_name){

    //拿到node_name的哈希值
    auto id  = Hash(node_name);
    //检查hashmap中是否含有id
    while (node_id_map_.Has(id))
    {
       //如果id存在
       std::string* name = nullptr;
       node_id_map_.Get(id, &name);
       if(node_name == *name){
        break;
       }
       //说明有其他node_name和当前的哈希值相等，出现了哈希碰撞，将id++
       ++id;
       std::cout << " Node name hash collision: " << node_name << " <=> " << *name << std::endl;
    }
    //确保node_name是一个唯一的id
    node_id_map_.Set(id , node_name);
    return id;
}

std::string  GlobalData::GetChannelById(uint64_t id)
{
    std::string* channel = nullptr;
    if(channel_id_map_.Get(id, &channel))
    {
        return *channel;
    }
    return kEmptyString;
}


}
}
}
