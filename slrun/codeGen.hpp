#ifndef SL_CODEGEN_HPP
#define SL_CODEGEN_HPP

#include <vector>
#include <sstream>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/array.hpp>
#include "../slvm/vm.hpp"

#include "st.hpp"

namespace sl
{

vm::BytecodeBuffer generateBytecode(const st::Module& module);

void exportToAsm(const vm::BytecodeBuffer& bb, std::ostream& os);

}

#endif /* SL_CODEGEN_HPP */