#ifndef SL_CODEGEN_HPP
#define SL_CODEGEN_HPP

#include <map>
#include <string>
#include <ostream>
#include <sl/vm/def.hpp>
#include <sl/vm/BytecodeBuffer.hpp>
#include <sl/ast.hpp>

namespace sl
{

typedef std::map<std::string, vm::CodeAddr> FunctionAddrMap;

vm::BytecodeBuffer generateBytecode(const ast::Module& module, FunctionAddrMap& fam);

void exportToAsm(const vm::BytecodeBuffer& bb, std::ostream& os);

}

#endif /* SL_CODEGEN_HPP */