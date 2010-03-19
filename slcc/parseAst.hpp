#ifndef SL_PARSEST_HPP
#define SL_PARSEST_HPP

#include <memory>
#include "ast.hpp"
#include "cst.hpp"
#include "ErrorLogger.hpp"

namespace sl
{

ast::Module parseModule(const cst::Module& module, ErrorLogger& errorLogger);

}

#endif /* SL_PARSEST_HPP */