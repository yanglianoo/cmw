#include <cmw/class_loader/class_loader_manager.h>

namespace hnu {
namespace cmw {
namespace class_loader {


ClassLoaderManager::ClassLoaderManager() {}

ClassLoaderManager::~ClassLoaderManager() {}

ClassLoader* ClassLoaderManager::GetClassLoaderByLibPath(
    const std::string& library_path) {
  return libpath_loader_map_[library_path];
}

std::vector<ClassLoader*> ClassLoaderManager::GetAllValidClassLoaders(){
    std::vector<ClassLoader*> class_loaders;
    for (auto& lib_class_loader : libpath_loader_map_) {
        class_loaders.emplace_back(lib_class_loader.second);
    }
    return class_loaders;
}

std::vector<std::string> ClassLoaderManager::GetAllValidLibPath() {
    std::vector<std::string> libpath;
    for (auto& lib_class_loader : libpath_loader_map_) {
    if (lib_class_loader.second != nullptr) {
        libpath.emplace_back(lib_class_loader.first);
        }
    }
    return libpath;
}

bool ClassLoaderManager::IsLibraryValid(const std::string& library_name){
    std::vector<std::string> valid_libraries = GetAllValidLibPath();
    return (valid_libraries.end() != std::find(valid_libraries.begin(),
                                             valid_libraries.end(),
                                             library_name));
}

bool ClassLoaderManager::LoadLibrary(const std::string& library_path) {
    std::lock_guard<std::mutex> lck(libpath_loader_map_mutex_);
    if (!IsLibraryValid(library_path)) {
        libpath_loader_map_[library_path] =
        new class_loader::ClassLoader(library_path);
    }
    return IsLibraryValid(library_path);
}

int ClassLoaderManager::UnloadLibrary(const std::string& library_path) {
    int num_remain_unload = 0;
    if (IsLibraryValid(library_path)) {
        ClassLoader* class_loader = GetClassLoaderByLibPath(library_path);
        if ((num_remain_unload = class_loader->UnloadLibrary()) == 0){
            libpath_loader_map_[library_path] = nullptr;
            delete class_loader;
        }
    }
    return num_remain_unload;
}

void ClassLoaderManager::UnloadAllLibraries() {
    std::vector<std::string> valid_libraries = GetAllValidLibPath();
    for (auto& lib : valid_libraries) {
        UnloadLibrary(lib);
    }
}



}
}
}