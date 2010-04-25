#include <tut/tut.hpp> 
#include <sl/vm.hpp>
#include <boost/type_traits/make_signed.hpp>

namespace tut
{

namespace vm = sl::vm;

struct VM_Test_data
{
    static const std::size_t memorySize = 1024;
    vm::Environment env;
    vm::Environment origEnv;
    vm::CodeGenerator cg;
    
    VM_Test_data() : env(memorySize), origEnv(env) { }

    void run()
    {
        origEnv = env;
        cg.emit(vm::EXIT);
        env.execute(&cg.code().front(), 0);
    }

    template <typename T>
    void push(T value)
    {
        env.sp(env.sp() - sizeof(T));
        std::memcpy(&env.memory()[env.sp()], &value, sizeof(value));
    }

    template <typename T>
    void store(vm::DataAddr addr, T value)
    {
        std::memcpy(&env.memory()[addr], &value, sizeof(value));
    }

    template <typename Msg, typename T>
    void ensure_equals_on_stack(const Msg& msg, vm::BPAddr addr, const T& expected) const
    {
        T tmp;
        std::memcpy(&tmp, &env.memory()[env.sp() + addr], sizeof(T));
        
        ensure_equals(msg, tmp, expected);
    }

    template <typename Msg, typename T>
    void ensure_equals_in_memory(const Msg& msg, vm::DataAddr addr, const T& expected) const
    {
        T tmp;
        std::memcpy(&tmp, &env.memory()[addr], sizeof(T));
        
        ensure_equals(msg, tmp, expected);
    }

    void ensure_pushed(std::size_t size)
    {
        ensure_equals("pushed", env.sp(), origEnv.sp() - size);
    }

    void ensure_popped(std::size_t size)
    {
        ensure_equals("popped", env.sp(), origEnv.sp() + size);
    }

    void ensure_no_memory_corruption(std::size_t ignoreOnStack = 0)
    {
        assert(origEnv.sp() + ignoreOnStack <= origEnv.memory().size());

        ensure("sp", env.sp() <= env.memory().size());
        ensure("no change before", std::equal(env.memory().begin(), env.memory().begin() + env.sp(), origEnv.memory().begin()));
        ensure("no change after", std::equal(env.memory().begin() + origEnv.sp() + ignoreOnStack, env.memory().end(), origEnv.memory().begin() + origEnv.sp() + ignoreOnStack));
    }

    void ensure_no_memory_corruption_except(vm::DataAddr addr, std::size_t size)
    {
        ensure("no memory corruption except before", std::equal(env.memory().begin(), env.memory().begin() + addr, origEnv.memory().begin()));
        ensure("no memory corruption except after", std::equal(env.memory().begin() + addr + size, env.memory().end(), origEnv.memory().begin() + addr + size));
    }

    void ensure_no_bp_change()
    {
        ensure_equals("no bp change", env.bp(), origEnv.bp());
    }

    template <typename T, typename Tr>
    void test_binary_op(vm::Opcode opcode, T left, T right, Tr result)
    {
        cg.emit(opcode);

        push(left);
        push(right);

        run();

        ensure_popped(sizeof(T));
        ensure_equals_on_stack("result", 0, result);
        ensure_no_memory_corruption_except(env.sp(), sizeof(T));
        ensure_no_bp_change();
    }

    template <typename T, typename Tr>
    void test_unary_op(vm::Opcode opcode, T arg, Tr result)
    {
        cg.emit(opcode);

        push(arg);

        run();

        ensure_popped(0);
        ensure_equals_on_stack("result", 0, result);
        ensure_no_memory_corruption(sizeof(T));
        ensure_no_bp_change();
    }
};

static bool operator==(const vm::Environment& left, const vm::Environment& right)
{
    return left.memory() == right.memory() && left.sp() == right.sp() && left.bp() == right.bp();
}

typedef test_group<VM_Test_data, 200> tg;
typedef tg::object object;
tg BPath_group("SL_VM_UT");

template <>
template <>
void object::test<1>()
{
    set_test_name("Clear environment");

    ensure_equals("memory size", env.memory().size(), memorySize);
    ensure_equals("sp", env.sp(), memorySize);
}

template <>
template <>
void object::test<2>()
{
    set_test_name("Move");

    vm::Environment env2(std::move(env));

    ensure("memory", env2.memory() == origEnv.memory());
    ensure_equals("sp", env2.sp(), origEnv.sp());
    ensure_equals("bp", env2.bp(), origEnv.bp());
}

template <>
template <>
void object::test<3>()
{
    set_test_name("Accessors");

    env.sp(20u);
    ensure_equals("sp", env.sp(), 20u);
    env.bp(30u);
    ensure_equals("bp", env.bp(), 30u);
}

template <>
template <>
void object::test<4>()
{
    set_test_name("EXIT");

    run();

    ensure("no change", env == origEnv);
}

template <>
template <>
void object::test<5>()
{
    set_test_name("NOP");

    cg.emit(vm::NOP);

    run();

    ensure("no change", env == origEnv);
}

template <>
template <>
void object::test<6>()
{
    set_test_name("CONST4");

    cg.emit(vm::CONST4);
    const sl::int_t value = 0x12345678;
    const vm::BPAddr valueSize = sizeof(value);
    cg.emit(value);

    run();

    ensure_pushed(valueSize);
    ensure_equals_on_stack("value", 0, value);
    ensure_no_memory_corruption();
    ensure_no_bp_change();
}

template <>
template <>
void object::test<7>()
{
    set_test_name("LADDR");

    cg.emit(vm::LADDR);
    const vm::DataAddr bp = 123;
    const std::int16_t localAddr = -48;
    const vm::DataAddr value = boost::make_signed<vm::DataAddr>::type(bp) + localAddr;
    const vm::BPAddr valueSize = sizeof(value);
    cg.emit(localAddr);

    env.bp(bp);

    run();

    ensure_pushed(valueSize);
    ensure_equals_on_stack("value", 0, value);
    ensure_no_memory_corruption();
    ensure_no_bp_change();
}

template <>
template <>
void object::test<8>()
{
    set_test_name("LOAD4");

    cg.emit(vm::LOAD4);
    const sl::int_t value = 0x12345678;
    const sl::int_t addr = 0x50;
    
    store(addr, value);
    push(addr);

    run();

    ensure_pushed(0);
    ensure_equals_on_stack("value", 0, value);
    ensure_no_memory_corruption(sizeof(value));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<9>()
{
    set_test_name("LLOAD4");

    cg.emit(vm::LLOAD4);
    const std::int16_t addr = 0x8;
    cg.emit(addr);
    const sl::int_t value = 0x12345678;
    const sl::int_t bp = 0x12;
    env.bp(bp);
    store(bp + addr, value);

    run();

    ensure_pushed(sizeof(value));
    ensure_equals_on_stack("value", 0, value);
    ensure_no_memory_corruption();
    ensure_no_bp_change();
}

template <>
template <>
void object::test<10>()
{
    set_test_name("STORE4");

    cg.emit(vm::STORE4);
    const sl::int_t addr = 0x8;
    const sl::int_t value = 0x12345678;

    push(addr);
    push(value);

    run();

    ensure_popped(sizeof(addr) + sizeof(value));
    ensure_equals_in_memory("value", addr, value);
    ensure_no_memory_corruption_except(addr, sizeof(value));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<11>()
{
    set_test_name("LSTORE4");

    cg.emit(vm::LSTORE4);
    const std::int16_t addr = 0x8;
    cg.emit(addr);
    const sl::int_t value = 0x12345678;
    const sl::int_t bp = 0x12;
    env.bp(bp);

    push(value);

    run();

    ensure_popped(sizeof(value));
    ensure_equals_in_memory("value", bp + addr, value);
    ensure_no_memory_corruption_except(bp + addr, sizeof(value));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<12>()
{
    set_test_name("ADDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ADDI, 12352, 85487, 12352 + 85487);
}
template <>
template <>
void object::test<13>()
{
    set_test_name("ADDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ADDI, -82468, 1346347, -82468 + 1346347);
}

template <>
template <>
void object::test<14>()
{
    set_test_name("ADDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ADDI, 0, 2, 2);
}

template <>
template <>
void object::test<15>()
{
    set_test_name("ADDF");

    test_binary_op<sl::float_t, sl::float_t>(vm::ADDF, 123.52f, 854.87f, 123.52f + 854.87f);
}

template <>
template <>
void object::test<16>()
{
    set_test_name("ADDF");

    test_binary_op<sl::float_t, sl::float_t>(vm::ADDF, -824.68f, 13463.47f, -824.68f + 13463.47f);
}

template <>
template <>
void object::test<17>()
{
    set_test_name("ADDF");

    test_binary_op<sl::float_t, sl::float_t>(vm::ADDF, 0.0f, 0.2f, 0.2f);
}

template <>
template <>
void object::test<18>()
{
    set_test_name("SUBI");

    test_binary_op<sl::int_t, sl::int_t>(vm::SUBI, 12352, 85487, 12352 - 85487);
}

template <>
template <>
void object::test<19>()
{
    set_test_name("SUBI");

    test_binary_op<sl::int_t, sl::int_t>(vm::SUBI, -82468, 1346347, -82468 - 1346347);
}

template <>
template <>
void object::test<20>()
{
    set_test_name("SUBI");

    test_binary_op<sl::int_t, sl::int_t>(vm::SUBI, 0, 2, -2);
}

template <>
template <>
void object::test<21>()
{
    set_test_name("SUBF");

    test_binary_op<sl::float_t, sl::float_t>(vm::SUBF, 123.52f, 854.87f, 123.52f - 854.87f);
}

template <>
template <>
void object::test<22>()
{
    set_test_name("SUBF");

    test_binary_op<sl::float_t, sl::float_t>(vm::SUBF, -824.68f, 13463.47f, -824.68f - 13463.47f);
}

template <>
template <>
void object::test<23>()
{
    set_test_name("SUBF");

    test_binary_op<sl::float_t, sl::float_t>(vm::SUBF, 0.0f, 0.2f, -0.2f);
}

template <>
template <>
void object::test<24>()
{
    set_test_name("MULI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MULI, 12352, 85487, 12352 * 85487);
}

template <>
template <>
void object::test<25>()
{
    set_test_name("MULI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MULI, -82468, 13467, -82468 * 13467);
}

template <>
template <>
void object::test<26>()
{
    set_test_name("MULI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MULI, 0, 2, 0);
}

template <>
template <>
void object::test<27>()
{
    set_test_name("MULF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MULF, 123.52f, 854.87f, 123.52f * 854.87f);
}

template <>
template <>
void object::test<28>()
{
    set_test_name("MULF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MULF, -824.68f, 13463.47f, -824.68f * 13463.47f);
}

template <>
template <>
void object::test<29>()
{
    set_test_name("MULF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MULF, 0.0f, 0.2f, 0.0f);
}

template <>
template <>
void object::test<30>()
{
    set_test_name("DIVI");

    test_binary_op<sl::int_t, sl::int_t>(vm::DIVI, 12352, 85487, 12352 / 85487);
}

template <>
template <>
void object::test<31>()
{
    set_test_name("DIVI");

    test_binary_op<sl::int_t, sl::int_t>(vm::DIVI, -82468, 13467, -82468 / 13467);
}

template <>
template <>
void object::test<32>()
{
    set_test_name("DIVI");

    test_binary_op<sl::int_t, sl::int_t>(vm::DIVI, 0, 2, 0);
}

template <>
template <>
void object::test<33>()
{
    set_test_name("DIVF");

    test_binary_op<sl::float_t, sl::float_t>(vm::DIVF, 123.52f, 854.87f, 123.52f / 854.87f);
}

template <>
template <>
void object::test<34>()
{
    set_test_name("DIVF");

    test_binary_op<sl::float_t, sl::float_t>(vm::DIVF, -824.68f, 13463.47f, -824.68f / 13463.47f);
}

template <>
template <>
void object::test<35>()
{
    set_test_name("DIVF");

    test_binary_op<sl::float_t, sl::float_t>(vm::DIVF, 0.0f, 0.2f, 0.0f);
}

template <>
template <>
void object::test<36>()
{
    set_test_name("MODI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MODI, 12352, 85487, 12352 % 85487);
}

template <>
template <>
void object::test<37>()
{
    set_test_name("MODI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MODI, -82468, 13467, -82468 % 13467);
}

template <>
template <>
void object::test<38>()
{
    set_test_name("MODI");

    test_binary_op<sl::int_t, sl::int_t>(vm::MODI, 0, 2, 0);
}

template <>
template <>
void object::test<39>()
{
    set_test_name("MODF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MODF, 123.52f, 854.87f, std::fmod(123.52f, 854.87f));
}

template <>
template <>
void object::test<40>()
{
    set_test_name("MODF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MODF, -824.68f, 13463.47f, std::fmod(-824.68f, 13463.47f));
}

template <>
template <>
void object::test<41>()
{
    set_test_name("MODF");

    test_binary_op<sl::float_t, sl::float_t>(vm::MODF, 0.0f, 0.2f, 0.0f);
}

template <>
template <>
void object::test<42>()
{
    set_test_name("NEGI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NEGI, 234, -234);
}

template <>
template <>
void object::test<43>()
{
    set_test_name("NEGI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NEGI, -34, 34);
}

template <>
template <>
void object::test<44>()
{
    set_test_name("NEGI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NEGI, 0, 0);
}

template <>
template <>
void object::test<45>()
{
    set_test_name("NEGF");

    test_unary_op<sl::float_t, sl::float_t>(vm::NEGF, 23.4f, -23.4f);
}

template <>
template <>
void object::test<46>()
{
    set_test_name("NEGF");

    test_unary_op<sl::float_t, sl::float_t>(vm::NEGF, -3.4f, 3.4f);
}

template <>
template <>
void object::test<47>()
{
    set_test_name("NEGF");

    test_unary_op<sl::float_t, sl::float_t>(vm::NEGF, 0.0f, 0.0f);
}

template <>
template <>
void object::test<48>()
{
    set_test_name("ANDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ANDI, 12352, 85487, 12352 & 85487);
}
template <>
template <>
void object::test<49>()
{
    set_test_name("ANDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ANDI, -82468, 1346347, -82468 & 1346347);
}

template <>
template <>
void object::test<50>()
{
    set_test_name("ANDI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ANDI, 0, 2, 0);
}

template <>
template <>
void object::test<51>()
{
    set_test_name("ORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ORI, 12352, 85487, 12352 | 85487);
}

template <>
template <>
void object::test<52>()
{
    set_test_name("ORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ORI, -82468, 1346347, -82468 | 1346347);
}

template <>
template <>
void object::test<53>()
{
    set_test_name("ORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::ORI, 0, 2, 2);
}

template <>
template <>
void object::test<54>()
{
    set_test_name("XORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::XORI, 12352, 85487, 12352 ^ 85487);
}

template <>
template <>
void object::test<55>()
{
    set_test_name("XORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::XORI, -82468, 1346347, -82468 ^ 1346347);
}

template <>
template <>
void object::test<56>()
{
    set_test_name("XORI");

    test_binary_op<sl::int_t, sl::int_t>(vm::XORI, 0, 2, 2);
}

template <>
template <>
void object::test<57>()
{
    set_test_name("NOTI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NOTI, 4567, ~sl::int_t(4567));
}

template <>
template <>
void object::test<58>()
{
    set_test_name("NOTI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NOTI, -24567, ~sl::int_t(-24567));
}

template <>
template <>
void object::test<59>()
{
    set_test_name("NOTI");

    test_unary_op<sl::int_t, sl::int_t>(vm::NOTI, 0, ~sl::int_t(0));
}

template <>
template <>
void object::test<60>()
{
    set_test_name("I2F");

    test_unary_op<sl::int_t, sl::float_t>(vm::I2F, 12352, 12352.0f);
}

template <>
template <>
void object::test<61>()
{
    set_test_name("I2F");

    test_unary_op<sl::int_t, sl::float_t>(vm::I2F, -12352, -12352.0f);
}

template <>
template <>
void object::test<62>()
{
    set_test_name("I2F");

    test_unary_op<sl::int_t, sl::float_t>(vm::I2F, 0, 0.0f);
}

template <>
template <>
void object::test<63>()
{
    set_test_name("F2I");

    test_unary_op<sl::float_t, sl::int_t>(vm::F2I, 12352.0f, 12352);
}

template <>
template <>
void object::test<64>()
{
    set_test_name("F2I");

    test_unary_op<sl::float_t, sl::int_t>(vm::F2I, 123.52f, 123);
}

template <>
template <>
void object::test<65>()
{
    set_test_name("F2I");

    test_unary_op<sl::float_t, sl::int_t>(vm::F2I, -12.11f, -12);
}

template <>
template <>
void object::test<66>()
{
    set_test_name("JUMP");

    cg.emit(vm::JUMP);
    cg.emit(vm::NOTI);
    const sl::int_t ip = cg.emit(vm::NOTI);
    const sl::int_t a = 12352;
    const sl::int_t result = ~a;

    push(a);
    push(ip);

    run();

    ensure_popped(sizeof(ip));
    ensure_equals_on_stack("value", 0, result);
    ensure_no_memory_corruption_except(env.sp(), sizeof(result));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<67>()
{
    set_test_name("CALL");

    const sl::int_t goodValue = 12452;
    const sl::int_t badValue = 43574;

    cg.emit(vm::NOP);
    cg.emit(vm::NOP);
    cg.emit(vm::NOP);
    cg.emit(vm::CALL);
    const sl::int_t retIp = cg.emit(vm::CONST4);
    cg.emit(badValue);
    cg.emit(vm::EXIT);
    const sl::int_t ip = cg.emit(vm::CONST4);
    cg.emit(goodValue);

    push(ip);

    run();

    ensure_pushed(sizeof(goodValue));
    ensure_equals_on_stack("value", 0, goodValue);
    ensure_equals_on_stack("ret address", sizeof(goodValue), retIp);
    ensure_no_memory_corruption(sizeof(retIp));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<68>()
{
    set_test_name("ENTER");

    cg.emit(vm::ENTER);
    const std::uint16_t locals = 24;
    cg.emit(locals);
    const sl::int_t oldBp = 384;
    env.bp(oldBp);

    run();

    ensure_equals("bp = old sp - 4", env.bp(), origEnv.sp() - sizeof(oldBp));
    ensure_equals("sp = bp - locals", env.sp(), env.bp() - locals);
    ensure_equals_in_memory("old bp", env.bp(), oldBp);
    ensure_no_memory_corruption_except(env.bp(), sizeof(oldBp));
}

template <>
template <>
void object::test<69>()
{
    set_test_name("LEAVE");
    cg.emit(vm::LEAVE);
    const std::uint16_t locals = 24;
    const sl::int_t oldBp = 384;
    const sl::int_t oldSp = env.sp();

    push(oldBp);
    env.bp(env.sp());
    env.sp(env.sp() - locals);

    run();

    ensure_equals("bp = old bp", sl::int_t(env.bp()), oldBp);
    ensure_equals("sp = old sp", sl::int_t(env.sp()), oldSp);
    ensure_no_memory_corruption();
}

template <>
template <>
void object::test<70>()
{
    set_test_name("RET");

    cg.emit(vm::RET);
    cg.emit(vm::NOTI);
    const sl::int_t ip = cg.emit(vm::NOTI);
    const sl::int_t a = 12352;
    const sl::int_t result = ~a;

    push(a);
    push(ip);

    run();

    ensure_popped(sizeof(ip));
    ensure_equals_on_stack("value", 0, result);
    ensure_no_memory_corruption_except(env.sp(), sizeof(result));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<71>()
{
    set_test_name("POP");

    cg.emit(vm::POP);
    const std::uint8_t n = 24;
    cg.emit(n);

    env.sp(128);
    
    run();

    ensure_popped(n);
    ensure_no_memory_corruption();
    ensure_no_bp_change();
}

template <>
template <>
void object::test<72>()
{
    set_test_name("CJUMP 0");

    cg.emit(vm::CJUMP);
    cg.emit(vm::NOTI);
    const sl::int_t ip = cg.emit(vm::NOTI);
    const sl::int_t a = 12352;
    const sl::int_t result = ~~a;
    const sl::int_t cond = 0;

    push(a);
    push(ip);
    push(cond);

    run();

    ensure_popped(sizeof(ip) + sizeof(cond));
    ensure_equals_on_stack("value", 0, result);
    ensure_no_memory_corruption_except(env.sp(), sizeof(result));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<73>()
{
    set_test_name("CJUMP 1");

    cg.emit(vm::CJUMP);
    cg.emit(vm::NOTI);
    const sl::int_t ip = cg.emit(vm::NOTI);
    const sl::int_t a = 12352;
    const sl::int_t result = ~a;
    const sl::int_t cond = 1;

    push(a);
    push(ip);
    push(cond);

    run();

    ensure_popped(sizeof(ip) + sizeof(cond));
    ensure_equals_on_stack("value", 0, result);
    ensure_no_memory_corruption_except(env.sp(), sizeof(result));
    ensure_no_bp_change();
}

template <>
template <>
void object::test<74>()
{
    set_test_name("LTI");

    test_binary_op<sl::int_t, sl::int_t>(vm::LTI, -1252, 8578, 1);
}


template <>
template <>
void object::test<75>()
{
    set_test_name("LTI");
    test_binary_op<sl::int_t, sl::int_t>(vm::LTI, 3572, 578, 0);
}

template <>
template <>
void object::test<76>()
{
    set_test_name("LTI");
    test_binary_op<sl::int_t, sl::int_t>(vm::LTI, 322, 322, 0);
}

template <>
template <>
void object::test<77>()
{
    set_test_name("LTF");

    test_binary_op<sl::float_t, sl::int_t>(vm::LTF, -12.52f, 85.78f, 1);
}

template <>
template <>
void object::test<78>()
{
    set_test_name("LTF");
    test_binary_op<sl::float_t, sl::int_t>(vm::LTF, 35.72f, 5.78f, 0);
}

template <>
template <>
void object::test<79>()
{
    set_test_name("LTF");
    test_binary_op<sl::float_t, sl::int_t>(vm::LTF, 3.22f, 3.22f, 0);
}

template <>
template <>
void object::test<80>()
{
    set_test_name("LEI");

    test_binary_op<sl::int_t, sl::int_t>(vm::LEI, -1252, 8578, 1);
}

template <>
template <>
void object::test<81>()
{
    set_test_name("LEI");
    test_binary_op<sl::int_t, sl::int_t>(vm::LEI, 3572, 578, 0);
}

template <>
template <>
void object::test<82>()
{
    set_test_name("LEI");
    test_binary_op<sl::int_t, sl::int_t>(vm::LEI, 322, 322, 1);
}

template <>
template <>
void object::test<83>()
{
    set_test_name("LEF");

    test_binary_op<sl::float_t, sl::int_t>(vm::LEF, -12.52f, 85.78f, 1);
}

template <>
template <>
void object::test<84>()
{
    set_test_name("LEF");
    test_binary_op<sl::float_t, sl::int_t>(vm::LEF, 35.72f, 5.78f, 0);
}

template <>
template <>
void object::test<85>()
{
    set_test_name("LEF");
    test_binary_op<sl::float_t, sl::int_t>(vm::LEF, 3.22f, 3.22f, 1);
}

template <>
template <>
void object::test<86>()
{
    set_test_name("GTI");

    test_binary_op<sl::int_t, sl::int_t>(vm::GTI, -1252, 8578, 0);
}

template <>
template <>
void object::test<87>()
{
    set_test_name("GTI");
    test_binary_op<sl::int_t, sl::int_t>(vm::GTI, 3572, 578, 1);
}

template <>
template <>
void object::test<88>()
{
    set_test_name("GTI");
    test_binary_op<sl::int_t, sl::int_t>(vm::GTI, 322, 322, 0);
}

template <>
template <>
void object::test<89>()
{
    set_test_name("GTF");

    test_binary_op<sl::float_t, sl::int_t>(vm::GTF, -12.52f, 85.78f, 0);
}


template <>
template <>
void object::test<90>()
{
    set_test_name("GTF");
    test_binary_op<sl::float_t, sl::int_t>(vm::GTF, 35.72f, 5.78f, 1);
}


template <>
template <>
void object::test<91>()
{
    set_test_name("GTF");
    test_binary_op<sl::float_t, sl::int_t>(vm::GTF, 3.22f, 3.22f, 0);
}

template <>
template <>
void object::test<92>()
{
    set_test_name("EQI");

    test_binary_op<sl::int_t, sl::int_t>(vm::EQI, -1252, 8578, 0);
}

template <>
template <>
void object::test<93>()
{
    set_test_name("EQI");
    test_binary_op<sl::int_t, sl::int_t>(vm::EQI, 3572, 578, 0);
}

template <>
template <>
void object::test<94>()
{
    set_test_name("EQI");
    test_binary_op<sl::int_t, sl::int_t>(vm::EQI, 322, 322, 1);
}

template <>
template <>
void object::test<95>()
{
    set_test_name("EQF");

    test_binary_op<sl::float_t, sl::int_t>(vm::EQF, -12.52f, 85.78f, 0);
}


template <>
template <>
void object::test<96>()
{
    set_test_name("EQF");
    test_binary_op<sl::float_t, sl::int_t>(vm::EQF, 35.72f, 5.78f, 0);
}


template <>
template <>
void object::test<97>()
{
    set_test_name("EQF");
    test_binary_op<sl::float_t, sl::int_t>(vm::EQF, 3.22f, 3.22f, 1);
}

template <>
template <>
void object::test<98>()
{
    set_test_name("NEQI");

    test_binary_op<sl::int_t, sl::int_t>(vm::NEQI, -1252, 8578, 1);
}

template <>
template <>
void object::test<99>()
{
    set_test_name("NEQI");
    test_binary_op<sl::int_t, sl::int_t>(vm::NEQI, 3572, 578, 1);
}

template <>
template <>
void object::test<100>()
{
    set_test_name("NEQI");
    test_binary_op<sl::int_t, sl::int_t>(vm::NEQI, 322, 322, 0);
}

template <>
template <>
void object::test<101>()
{
    set_test_name("NEQF");

    test_binary_op<sl::float_t, sl::int_t>(vm::NEQF, -12.52f, 85.78f, 1);
}

template <>
template <>
void object::test<102>()
{
    set_test_name("NEQF");
    test_binary_op<sl::float_t, sl::int_t>(vm::NEQF, 35.72f, 5.78f, 1);
}

template <>
template <>
void object::test<103>()
{
    set_test_name("NEQF");
    test_binary_op<sl::float_t, sl::int_t>(vm::NEQF, 3.22f, 3.22f, 0);
}

}