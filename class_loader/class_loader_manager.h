#ifndef CMW_CLASS_LOADER_CLASS_LOADER_MANAGER_H_
#define CMW_CLASS_LOADER_CLASS_LOADER_MANAGER_H_


#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <cmw/class_loader/class_loader.h>

namespace hnu {
namespace cmw {
namespace class_loader {

class ClassLoaderManager {

public:
    ClassLoaderManager();
    virtual ~ClassLoaderManager();

    bool LoadLibrary(const std::string& library_path);
    void UnloadAllLibraries();

    bool IsLibraryValid(const std::string& library_path);

    template <typename Base>
    std::shared_ptr<Base> CreateClassObj(const std::string& class_name);

    template <typename Base>
    std::shared_ptr<Base> CreateClassObj(const std::string& class_name,
                                       const std::string& library_path);
    
    template <typename Base>
    bool IsClassValid(const std::string& class_name);

    template <typename Base>
    std::vector<std::string> GetValidClassNames();

private:
    ClassLoader* GetClassLoaderByLibPath(const std::string& library_path);
    std::vector<ClassLoader*> GetAllValidClassLoaders();
    std::vector<std::string> GetAllValidLibPath();
    int UnloadLibrary(const std::string& library_path);

private:
    std::mutex libpath_loader_map_mutex_;
    std::map<std::string, ClassLoader*> libpath_loader_map_;

};

template <typename Base>
std::shared_ptr<Base> ClassLoaderManager::CreateClassObj(
        const std::string& class_name) {
    std::vector<ClassLoader*> class_loaders = GetAllValidClassLoaders();
    for (auto class_loader : class_loaders){
        if(class_loader->IsClassValid<Base>(class_name)){
            
        }
    }
}

}
}
}
#endif