#ifndef SL_PARSEAST_HPP
#define SL_PARSEAST_HPP

#include <boost/function.hpp>
#include <sl/ast/Module.hpp>
#include <sl/ast/ModuleMap.hpp>
#include <sl/cst/Module.hpp>
#include <sl/ErrorLogger.hpp>

namespace sl
{

typedef boost::function<boost::optional<cst::Module>(const std::string& name, ErrorLogger& errorLogger)> ModuleLoader;

ast::Module parseModule(const cst::Module& module, ErrorLogger& errorLogger);
bool parseModules(const std::string& name, ast::ModuleMap& mm, ModuleLoader loader, ErrorLogger& errorLogger);

}

#endif /* SL_PARSEAST_HPP */