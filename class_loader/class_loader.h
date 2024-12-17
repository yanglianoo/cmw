#ifndef CMW_CLASS_LOADER_CLASS_LOADER_H_
#define CMW_CLASS_LOADER_CLASS_LOADER_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <cmw/class_loader/utility/class_loader_utility.h>
#include <cmw/class_loader/class_loader_register_macro.h>

namespace hnu {
namespace cmw {
namespace class_loader {


class ClassLoader {
public:
    explicit ClassLoader(const std::string& library_path);
    virtual ~ClassLoader();


public:
    bool LoadLibrary();
    int UnloadLibrary();
    bool IsLibraryLoaded();

    const std::string& GetLibraryPath() const;

public:
    template <typename Base>
    std::vector<std::string> GetValidClassNames();

    template <typename Base>
    std::shared_ptr<Base> CreateClassObj(const std::string& class_name);

    template <typename Base>
    bool IsClassValid(const std::string& class_name);

private:
    template <typename Base>
    void OnClassObjDeleter(Base* obj);

private:
    std::string library_path_;
    int loadlib_ref_count_;
    std::mutex loadlib_ref_count_mutex_;
    int classobj_ref_count_;
    std::mutex classobj_ref_count_mutex_;
};

template <typename Base>
std::vector<std::string> ClassLoader::GetValidClassNames() {
    return (utility::GetValidClassNames<Base>(this));
}

template <typename Base>
bool ClassLoader::IsClassValid(const std::string& class_name){
    std::vector<std::string> valid_classes = GetValidClassNames<Base>();
}

template <typename Base>
std::shared_ptr<Base> ClassLoader::CreateClassObj(const std::string& class_name){
    if (!IsLibraryLoaded()){
        LoadLibrary();
    }

    Base* class_object = utility::CreateClassObj<Base>(class_name, this);
    if (class_object == nullptr) {
        AWARN << "CreateClassObj failed, ensure class has been registered. "
            << "classname: " << class_name << ",lib: " << GetLibraryPath();
        return std::shared_ptr<Base>();
    }

    classobj_ref_count_ = classobj_ref_count_ + 1;
    std::shared_ptr<Base> classObjSharePtr(
        class_object, std::bind(&ClassLoader::OnClassObjDeleter<Base>, this,
                              std::placeholders::_1));
    
    return classObjSharePtr;
}

template <typename Base>
void ClassLoader::OnClassObjDeleter(Base* obj) {
  if (nullptr == obj) {
    return;
  }

  std::lock_guard<std::mutex> lck(classobj_ref_count_mutex_);
  delete obj;
  --classobj_ref_count_;
}

}
}
}

#endif