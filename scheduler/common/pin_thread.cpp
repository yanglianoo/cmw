#include <cmw/scheduler/common/pin_thread.h>


#include <sched.h>
#include <sys/resource.h>
#include <cstring>

namespace hnu    {
namespace cmw   {
namespace scheduler {

void ParseCpuset(const std::string& str, std::vector<int>* cpuset) {
    std::vector<std::string> lines;
    std::stringstream ss(str);

    std::string l;

    while (getline(ss, l, ','))
    {
        lines.push_back(l);
    }

    for(auto line : lines){
        std::stringstream ss(line);
        std::vector<std::string> range;
        while (getline(ss, l, '-'))
        {
            range.push_back(l);
        }

        if(range.size() == 1){
            cpuset->push_back(std::stoi(range[0]));
        } else if (range.size() == 2){
            for(int i = std::stoi(range[0]), e = std::stoi(range[1]); i <= e; i++){
                cpuset->push_back(i);
            }
        } else {
            ADEBUG << "Parsing cpuset format error.";
            exit(0);
        }
        
    }
    
}

void SetSchedAffinity(std::thread* thread, const std::vector<int>& cpus,
                      const std::string& affinity, int cpu_id){
    
    cpu_set_t set;
    CPU_ZERO(&set);

    if(cpus.size()){
        if(affinity.compare("range")){
            for(const auto cpu : cpus){
                CPU_SET(cpu, &set);
            }
            pthread_setaffinity_np(thread->native_handle(), sizeof(set), &set);
            AINFO << "thread " << thread->get_id() << " set range affinity";
        } else if(!affinity.compare("1to1")){
            if(cpu_id == -1 || (uint32_t)cpu_id >= cpus.size()){
                return;
            }
            CPU_SET(cpus[cpu_id], &set);
            pthread_setaffinity_np(thread->native_handle(), sizeof(set), &set);
            AINFO << "thread " << thread->get_id() << " set 1to1 affinity";
        }
    } 

}

void SetSchedPolicy(std::thread* thread, std::string spolicy,
                    int sched_priority, pid_t tid) {
    struct sched_param sp;
    int policy;

    memset(reinterpret_cast<void*>(&sp), 0, sizeof(sp));

    if(!spolicy.compare("SCHED_FIFO")){
        policy = SCHED_FIFO;
        pthread_setschedparam(thread->native_handle(), policy, &sp);
        AINFO << "thread " << tid << " set sched_policy: " << spolicy;
    } else if(!spolicy.compare("SCHED_RR")){
        policy = SCHED_RR;
        pthread_setschedparam(thread->native_handle(), policy, &sp);
        AINFO << "thread " << tid << " set sched_policy: " << spolicy;
    } else if (!spolicy.compare("SCHED_OTHER")) {
        setpriority(PRIO_PROCESS, tid, sched_priority);
        AINFO << "thread " << tid << " set sched_policy: " << spolicy;
  }
}


}
}
}