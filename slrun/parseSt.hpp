#ifndef SL_PARSEST_HPP
#define SL_PARSEST_HPP

#include <memory>
#include "st.hpp"
#include "ast.hpp"

namespace sl
{

st::Module parseModule(const ast::Module& module);

}

#endif /* SL_PARSEST_HPP */