#ifndef CMW_CLASS_LOADER_UTILITY_CLASS_FACTORY_H_
#define CMW_CLASS_LOADER_UTILITY_CLASS_FACTORY_H_

#include <string>
#include <typeinfo>
#include <vector>


namespace hnu {
namespace cmw {
namespace class_loader {

class ClassLoader;

namespace utility {

class AbstractClassFactoryBase { 
public:
    AbstractClassFactoryBase(const std::string& class_name,
                           const std::string& base_class_name)
        : class_name_(class_name), base_class_name_(base_class_name) {}
    virtual ~AbstractClassFactoryBase() = default;

    void SetLibraryPath(const std::string& library_path);

    void AddOwnedClassLoader(ClassLoader* loader);
    void RemoveOwnedClassLoader(const ClassLoader* loader);
    bool IsOwnedBy(const ClassLoader* loader);
    bool IsOwnedByAnybody();

    const std::string& GetLibraryPath() const { return library_path_; }
    const std::string& GetBaseClassName() const { return base_class_name_; }
    const std::string& GetClassName() const { return class_name_; }
protected:
    std::vector<ClassLoader*> relative_class_loaders_;
    std::string library_path_;
    std::string class_name_;
    std::string base_class_name_;
};


template <typename Base>
class AbstractClassFactory : public AbstractClassFactoryBase {
public:
    AbstractClassFactory(const std::string& class_name,
                       const std::string& base_class_name)
      : AbstractClassFactoryBase(class_name, base_class_name) {}

    virtual Base* CreateObj() const = 0;
public:
  AbstractClassFactory(const AbstractClassFactory&) = delete;
  AbstractClassFactory& operator=(const AbstractClassFactory&) = delete;
};

template <typename ClassObject, typename Base>
class ClassFactory : public AbstractClassFactory<Base>{
public:
    ClassFactory(const std::string& class_name,
               const std::string& base_class_name)
      : AbstractClassFactory<Base>(class_name, base_class_name) {}
    Base* CreateObj() const { return new ClassObject; }
};



}
}
}
}

#endif