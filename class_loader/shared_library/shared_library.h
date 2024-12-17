#ifndef CMW_CLASS_LOADER_SHARED_LIBRARY_SHARED_LIBRARY_H_
#define CMW_CLASS_LOADER_SHARED_LIBRARY_SHARED_LIBRARY_H_

#include <mutex>
#include <string>


#include <cmw/class_loader/shared_library/exceptions.h>


namespace hnu {
namespace cmw {
namespace class_loader {



class SharedLibrary {

public:
    enum Flags {
        SHLIB_GLOBAL = 1,
        SHLIB_LOCAL = 2,
    };


    SharedLibrary() = default;
    virtual ~SharedLibrary();

    explicit SharedLibrary(const std::string& path);

    SharedLibrary(const std::string& path, int flags);

public:

    void Load(const std::string& path);
    void Load(const std::string& path, int flags);
    void Unload();
    bool IsLoaded();
    bool HasSymbol(const std::string& name);
    void* GetSymbol(const std::string& name);

    inline const std::string& GetPath() const { return path_; }

private:

    void* handle_ = nullptr;
    std::string path_;
    std::mutex mutex_;
};

}
}
}

#endif