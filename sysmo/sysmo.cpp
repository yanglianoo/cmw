#include <cmw/sysmo/sysmo.h>
#include <cmw/common/environment.h>


namespace hnu {
namespace cmw {

SysMo::SysMo() { Start(); }

void SysMo::Start() {
    auto sysmo_start = common::GetEnv("sysmo_start");
    if (sysmo_start != "" && std::stoi(sysmo_start)) {
        start_ = true;
        sysmo_ = std::thread(&SysMo::Checker, this);
    }
}

void SysMo::Checker() {
    while (cyber_unlikely(!shut_down_.load())) {
    scheduler::Instance()->CheckSchedStatus();
    std::unique_lock<std::mutex> lk(lk_);
    cv_.wait_for(lk, std::chrono::milliseconds(sysmo_interval_ms_));
  }
}

void SysMo::Shutdown() {
  if (!start_ || shut_down_.exchange(true)) {
    return;
  }

  cv_.notify_all();
  if (sysmo_.joinable()) {
    sysmo_.join();
  }
}


}
}