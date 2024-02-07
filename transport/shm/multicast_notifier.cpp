#include <cmw/transport/shm/multicast_notifier.h>
#include <cmw/common/global_data.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
namespace hnu{
namespace cmw{
namespace transport{

using common::GlobalData;

MulticastNotifier::MulticastNotifier() {
    if(!Init()){
        Shutdown();
    }
}

MulticastNotifier::~MulticastNotifier() { Shutdown();}



bool MulticastNotifier::Init(){
    std::string mcast_ip("239.255.0.100"); // 多播的ip
    uint16_t mcast_port = 8888;            // 多播的端口号

    notify_fd_ =  socket(AF_INET , SOCK_DGRAM , 0);  //创建udp套接字
    if (notify_fd_ == -1) {
        std::cout << "fail to create notify fd, " << strerror(errno) << std::endl;
        return false;
    }

    memset(&notify_addr_ , 0 , sizeof(notify_addr_));
    notify_addr_.sin_family = AF_INET;
    notify_addr_.sin_addr.s_addr = inet_addr(mcast_ip.c_str());
    notify_addr_.sin_port = htons(mcast_port);

    listen_fd_ = socket(AF_INET ,SOCK_DGRAM , 0);
    if (listen_fd_ == -1) {
        std::cout << "fail to create listen fd, " << strerror(errno) << std::endl;
        return false;
    }

    //设置 listen_fd_ 为非阻塞模式
    if (fcntl(listen_fd_, F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "fail to set listen fd nonblock, " << strerror(errno)<< std::endl;
        return false;
    }

    memset(&listen_addr_ , 0 , sizeof(listen_addr_)); 
    listen_addr_.sin_family = AF_INET;
    listen_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr_.sin_port = htons(mcast_port);

    //允许服务器bind一个地址，即使这个地址当前已经存在已建立的连接，允许地址复用
    int yes = 1;
    if(setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &yes , sizeof(yes)) < 0){
        std::cout  << "fail to setsockopt SO_REUSEADDR, " << strerror(errno)<< std::endl;
        return false;
    }
    //为socket fd 绑定 sockaddr
    if (bind(listen_fd_, (struct sockaddr*)&listen_addr_, sizeof(listen_addr_)) <0) {
        std::cout << "fail to bind addr, " << strerror(errno) << std::endl;
        return false;
    }
    //允许多播时本地回环
    int loop = 1;
    if (setsockopt(listen_fd_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop,sizeof(loop)) < 0) {
        std::cout << "fail to setsockopt IP_MULTICAST_LOOP, " << strerror(errno)<<std::endl;
        return false;
    }
    
    //加入多播组，多播ip为mcast_ip
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(mcast_ip.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(listen_fd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
                 sizeof(mreq)) < 0) {
        std::cout << "fail to setsockopt IP_ADD_MEMBERSHIP, " << strerror(errno)<<std::endl;
        return false;
    }

    return true;
}

//发送ReadableInfo到组播网络中
bool MulticastNotifier::Notify(const ReadableInfo& info){
    if(is_shutdown_.load()){
        return false;
    }
    std::string info_str;
    info.SerializeTo(&info_str);
    ssize_t nbytes = sendto(notify_fd_, info_str.c_str(), info_str.size() , 0,
                        (struct sockaddr*)&notify_addr_, sizeof(notify_addr_));
    return nbytes > 0;
}


bool MulticastNotifier::Listen(int timeout_ms, ReadableInfo* info){
    if(is_shutdown_.load()){
        return false;
    }
    if(info == nullptr){
        std::cout << "info nullptr" << std::endl;
        return false;
    }
    struct pollfd fds;
    fds.fd = listen_fd_;
    fds.events = POLLIN;
    int ready_num = poll(&fds, 1 , timeout_ms);
    if(ready_num > 0){
        char buf[32] = {0};
        ssize_t nbytes = recvfrom(listen_fd_, buf , 32 , 0 , nullptr, nullptr);
        if(nbytes == -1){
            std::cout << "fail to recvfrom, " << strerror(errno) << std::endl;
            return false;
        }
        return info->DeserializeFrom(buf, nbytes);
    } else if(ready_num == 0) {
        std::cout << "timeout, no readableinfo." << std::endl;
    } else {
        if(errno == EINTR){
            std::cout << "poll was interrupted." << std::endl;
        } else {
            std::cout << "fail to poll, " << strerror(errno)<< std::endl;
        }
    }

    return false;

}


void MulticastNotifier::Shutdown(){
    if(is_shutdown_.exchange(true)){
        return;
    }

    if(notify_fd_ != -1){
        close(notify_fd_);
        notify_fd_ = -1;
    }
    memset(&notify_addr_, 0 ,sizeof(notify_addr_));

    if(listen_fd_ != -1){
        close(listen_fd_);
        listen_fd_ = -1;
    }
    memset(&listen_addr_ , 0 , sizeof(listen_addr_));
}


}
}
}