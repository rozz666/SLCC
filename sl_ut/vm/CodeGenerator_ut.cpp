#include <tut/tut.hpp> 
#include <sl/vm/CodeGenerator.hpp>

namespace tut
{

namespace vm = sl::vm;

struct CodeGenerator_Test_data
{
    vm::CodeGenerator cg;
};

typedef test_group<CodeGenerator_Test_data> tg;
typedef tg::object object;
tg CG_group("vm.CodeGenerator");

template <>
template <>
void object::test<1>()
{
    set_test_name("Empty");

    ensure("empty", cg.code.empty());
}

template <>
template <>
void object::test<2>()
{
    set_test_name("Emit opcodes");

    vm::CodeAddr addr1 = cg.emit(vm::ADDI);
    vm::CodeAddr addr2 = cg.emit(vm::CONST4);
    vm::CodeAddr addr3 = cg.emit(vm::CALL);
    vm::CodeAddr addr4 = cg.emit(vm::JUMP);
    vm::CodeAddr addr5 = cg.emit(vm::LTI);

    ensure_equals("5 bytes", cg.code.size(), 5u);
    ensure_equals("addr1", addr1, 0u);
    ensure_equals("addr2", addr2, 1u);
    ensure_equals("addr3", addr3, 2u);
    ensure_equals("addr4", addr4, 3u);
    ensure_equals("addr5", addr5, 4u);
    ensure_equals("ADDI", cg.code[addr1], vm::ADDI);
    ensure_equals("CONST4", cg.code[addr2], vm::CONST4);
    ensure_equals("CALL", cg.code[addr3], vm::CALL);
    ensure_equals("JUMP", cg.code[addr4], vm::JUMP);
    ensure_equals("LTI", cg.code[addr5], vm::LTI);

    cg.emit(addr3, vm::ADDF);

    ensure_equals("5 bytes", cg.code.size(), 5u);
    ensure_equals("ADDI", cg.code[addr1], vm::ADDI);
    ensure_equals("CONST4", cg.code[addr2], vm::CONST4);
    ensure_equals("ADDF", cg.code[addr3], vm::ADDF);
    ensure_equals("JUMP", cg.code[addr4], vm::JUMP);
    ensure_equals("LTI", cg.code[addr5], vm::LTI);
}

template <>
template <>
void object::test<3>()
{
    set_test_name("Emit values");

    const sl::int_t value1 = 192837465;
    const sl::float_t value2 = 2343.554654f;
    const sl::int_t value3 = -2983;
    const sl::float_t value4 = -45.348f;

    vm::CodeAddr addr1 = cg.emit(value1);
    vm::CodeAddr addr2 = cg.emit(value2);
    vm::CodeAddr addr3 = cg.emit(value3);

    ensure_equals("# bytes", cg.code.size(), sizeof(value1) + sizeof(value2) + sizeof(value3));
    ensure_equals("addr1", addr1, 0u);
    ensure_equals("addr2", addr2, sizeof(value1));
    ensure_equals("addr3", addr3, addr2 + sizeof(value2));
    ensure("value1", std::memcmp(&cg.code[addr1], &value1, sizeof(value1)) == 0);
    ensure("value2", std::memcmp(&cg.code[addr2], &value2, sizeof(value2)) == 0);
    ensure("value3", std::memcmp(&cg.code[addr3], &value3, sizeof(value3)) == 0);

    cg.emit(addr2, value4);

    ensure_equals("# bytes", cg.code.size(), sizeof(value1) + sizeof(value2) + sizeof(value3));
    ensure("value1", std::memcmp(&cg.code[addr1], &value1, sizeof(value1)) == 0);
    ensure("value4", std::memcmp(&cg.code[addr2], &value4, sizeof(value4)) == 0);
    ensure("value3", std::memcmp(&cg.code[addr3], &value3, sizeof(value3)) == 0);
}

template <>
template <>
void object::test<4>()
{
    set_test_name("Emit assertion failed");

    try
    {
        cg.emit(vm::ADDF);
        cg.emit<sl::int_t>(0, 5);
        fail("No assertion for overflow");
    }
    catch (const sl::AssertionFailed& ) { }
}

template <>
template <>
void object::test<5>()
{
    set_test_name("Read");

    const sl::int_t value1 = 93878172;
    const sl::float_t value2 = -459.234f;
    const vm::Opcode value3 = vm::DIVF;

    vm::CodeAddr addr1 = cg.emit(value1);
    vm::CodeAddr addr2 = cg.emit(value2);
    vm::CodeAddr addr3 = cg.emit(value3);

    ensure_equals("int", cg.read<sl::int_t>(addr1), value1);
    ensure_equals("float", cg.read<sl::float_t>(addr2), value2);
    ensure_equals("opcode", cg.read<sl::byte>(addr3), value3);
}

template <>
template <>
void object::test<6>()
{
    set_test_name("Read assertion failed");

    try
    {
        cg.emit(vm::ADDF);
        cg.read<sl::int_t>(0);
        fail("No assertion for overflow");
    }
    catch (const sl::AssertionFailed& ) { }
}


}
