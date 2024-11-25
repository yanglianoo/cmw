

#include <cmw/common/file.h>
#include <sys/stat.h>

namespace hnu {
namespace cmw {
namespace common {


std::string GetFileName(const std::string &path,
                        const bool remove_extension )
{
    // 找到字符串中最后一个 `/`的位置
    std::string::size_type start = path.rfind('/');
    if(start == std::string::npos){        //如果字符串中没有`/` ，则 start = 0
        start = 0;
    }else{
        ++ start;
    }

    std::string::size_type end = std::string::npos;
    if(remove_extension){   //remove_extension 代表是否要移除拓展名
        end  = path.rfind('.');
        //如果找到 . 且其位置在最后一个 / 之前，说明该 . 是在目录名中而不是文件名中，因此忽略它。
        if(end != std::string::npos && end < start){
            end = std::string::npos;
        }
    }

    const auto len = (end != std::string::npos) ? end - start : end;

    return path.substr(start, len);

}

std::string GetAbsolutePath(const std::string &prefix,
                            const std::string &relative_path){
    if(relative_path.empty()){
        return prefix;
    }    

    if(prefix.empty() || relative_path.front() == '/'){
        return relative_path;
    }     

    if(prefix.back() == '/'){
        return prefix + relative_path;
    }   

    return prefix + "/" + relative_path;
}

// 判断路径是否存在
bool PathExists(const std::string &path){
    struct stat info;
    return stat(path.c_str(), &info) == 0;
}

}
}
}