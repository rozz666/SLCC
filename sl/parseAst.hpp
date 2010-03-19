#ifndef SL_PARSEST_HPP
#define SL_PARSEST_HPP

#include <memory>
#include <sl/ast.hpp>
#include <sl/cst.hpp>
#include <sl/ErrorLogger.hpp>

namespace sl
{

ast::Module parseModule(const cst::Module& module, ErrorLogger& errorLogger);

}

#endif /* SL_PARSEST_HPP */