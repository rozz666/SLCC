#include <vector>
#include <cstdint>
#include "vm.hpp"

int main()
{
    namespace vm = sl::vm;
    vm::CodeGenerator cg;

    {
        using namespace vm;
        /*cg.add(CONST4, 11);
        cg.add(CALL);
        cg.add(JUMP, -1);       */

        cg.add(CONST4, -1);
        cg.add(JUMP, 10);

        cg.add(ENTER, 4);
        cg.add(CONST4, 3).add(CONST4, 2).add(CONST4, 1);
        cg.add(ADDI);
        cg.add(MULI);
        cg.add(STORE4, -4);
        cg.add(LEAVE);
        cg.add(RET);
    }

    vm::Program prog(64, std::move(cg.code()));

    prog.execute();
}