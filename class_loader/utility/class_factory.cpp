#include <cmw/class_loader/utility/class_factory.h>
#include <cmw/class_loader/class_loader.h>


namespace hnu {
namespace cmw {
namespace class_loader {


namespace utility { 


void AbstractClassFactoryBase::SetLibraryPath(const std::string& library_path) {
    library_path_ = library_path;
}

void AbstractClassFactoryBase::AddOwnedClassLoader(ClassLoader* loader) {
    auto iter = std::find(relative_class_loaders_.begin(),
                          relative_class_loaders_.end(), loader);
    if(iter == relative_class_loaders_.end()){
        relative_class_loaders_.emplace_back(loader);
        if(loader){
            library_path_ = loader->GetLibraryPath();
        }
    }
}

void AbstractClassFactoryBase::RemoveOwnedClassLoader(
        const ClassLoader* loader){
    
    std::vector<ClassLoader*>::iterator itr = std::find(
        relative_class_loaders_.begin(), relative_class_loaders_.end(), loader);
    if(itr != relative_class_loaders_.end()){
        relative_class_loaders_.erase(itr);
    }
}

bool AbstractClassFactoryBase::IsOwnedBy(const ClassLoader* loader){
    std::vector<ClassLoader*>::iterator itr = std::find(
       relative_class_loaders_.begin(), relative_class_loaders_.end(), loader);   
    return itr != relative_class_loaders_.end();
}

bool AbstractClassFactoryBase::IsOwnedByAnybody() {
  return !relative_class_loaders_.empty();
}


}
}
}
}