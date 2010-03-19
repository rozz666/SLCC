#ifndef SL_CODEGEN_HPP
#define SL_CODEGEN_HPP

#include <vector>
#include <sstream>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/array.hpp>
#include <sl/vm.hpp>
#include <sl/ast.hpp>

namespace sl
{

typedef std::map<std::string, vm::CodeAddr> FunctionAddrMap;

vm::BytecodeBuffer generateBytecode(const ast::Module& module, FunctionAddrMap& fam);

void exportToAsm(const vm::BytecodeBuffer& bb, std::ostream& os);

}

#endif /* SL_CODEGEN_HPP */