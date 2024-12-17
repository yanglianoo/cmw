#include <cmw/class_loader/shared_library/shared_library.h>

#include <dlfcn.h>



namespace hnu {
namespace cmw {
namespace class_loader {

SharedLibrary::SharedLibrary(const std::string& path) { Load(path, 0); }


SharedLibrary::SharedLibrary(const std::string& path, int flags) {
  Load(path, flags);
}

void SharedLibrary::Load(const std::string& path) { Load(path, 0); }

void SharedLibrary::Load(const std::string& path, int flags){

    std::lock_guard<std::mutex> lock(mutex_);
    if(handle_) throw LibraryAlreadyLoadedException(path);

    int real_flag = RTLD_LAZY;
    if(flags & SHLIB_LOCAL){
        real_flag |= SHLIB_LOCAL;
    }else{
        real_flag |= SHLIB_GLOBAL;
    }

    handle_ = dlopen(path.c_str(), real_flag);

    if(!handle_){
        const char* err = dlerror();
        throw LibraryAlreadyLoadedException(err ? std::string(err) : path);
    }

    path_ = path;
}

void SharedLibrary::Unload(){
    std::lock_guard<std::mutex> lock(mutex_);
    if(handle_) {
        dlclose(handle_);
        handle_ = nullptr;
    }
}

bool SharedLibrary::IsLoaded() {
  std::lock_guard<std::mutex> lock(mutex_);
  return handle_ != nullptr;
}

bool SharedLibrary::HasSymbol(const std::string& name) {
  return GetSymbol(name) != nullptr;
}

void* SharedLibrary::GetSymbol(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!handle_) return nullptr;

    void* result = dlsym(handle_, name.c_str());
    if (!result) {
        throw SymbolNotFoundException(name);
    }

    return result;
}
SharedLibrary::~SharedLibrary() {}


}
}
}