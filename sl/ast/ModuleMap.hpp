#ifndef SL_AST_MODULEMAP_HPP
#define SL_AST_MODULEMAP_HPP

#include <unordered_map>
#include <memory>
#include <sl/cst/Module.hpp>

namespace sl
{
namespace ast
{

class ModuleMap
{
    typedef std::unordered_map<std::string, cst::Module> C;

public:

    ModuleMap() { }
    ModuleMap(ModuleMap&& right) : map_(std::move(right.map_)) { }

    bool insert(const std::string& name, cst::Module&& module)
    {
        return map_.insert(std::make_pair(std::string(name), module)).second;
    }

    bool empty() const { return map_.empty(); }

    C::size_type size() const { return map_.size(); }

    bool contains(const std::string& name) const { return map_.find(name) != map_.end(); }

private:

    std::unordered_map<std::string, cst::Module> map_;
};

}
}

#endif /* SL_AST_MODULEMAP_HPP */