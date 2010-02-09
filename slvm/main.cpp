#include <vector>
#include <fstream>
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

        cg.emit(CONST4);
        cg.emit<std::int32_t>(-1);
        cg.emit(CONST4);
        cg.emit<std::int32_t>(11);
        cg.emit(JUMP);

        cg.emit(ENTER);
        cg.emit<std::uint16_t>(4);
        cg.emit(INPI);
        cg.emit<std::int16_t>(-4);
        cg.emit(CONST4);
        cg.emit<std::int32_t>(1);
        cg.emit(LOAD4);
        cg.emit<std::int16_t>(-4);
        cg.emit(DIVI);
        cg.emit(STORE4);
        cg.emit<std::int16_t>(-4);
        cg.emit(OUTI);
        cg.emit<std::int16_t>(-4);
        cg.emit(LEAVE);
        cg.emit(RET);

        std::ofstream bc("out.bin", std::ios::binary);
        bc.write((const char *) &cg.code().front(), cg.code().size());
    }

    vm::Program prog(64, std::move(cg.code()));

    prog.execute();
}