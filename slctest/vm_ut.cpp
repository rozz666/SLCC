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

    template <typename T>
    void ensure_binary_op(T result)
    {
        ensure_popped(sizeof(T));
        ensure_equals_on_stack("result", 0, result);
        ensure_no_memory_corruption_except(env.sp(), sizeof(T));
        ensure_no_bp_change();
    }

    template <typename T>
    void ensure_unary_op(T result)
    {
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

typedef test_group<VM_Test_data> tg;
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

    cg.emit(vm::ADDI);
    const sl::int_t a = 12352;
    const sl::int_t b = 85487;
    const sl::int_t result = a + b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<13>()
{
    set_test_name("ADDF");

    cg.emit(vm::ADDF);
    const sl::float_t a = 12352.2f;
    const sl::float_t b = 85487.3f;
    const sl::float_t result = a + b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<14>()
{
    set_test_name("SUBI");

    cg.emit(vm::SUBI);
    const sl::int_t a = 12352;
    const sl::int_t b = 85487;
    const sl::int_t result = a - b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<15>()
{
    set_test_name("SUBF");

    cg.emit(vm::SUBF);
    const sl::float_t a = 12352.2f;
    const sl::float_t b = 85487.3f;
    const sl::float_t result = a - b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<16>()
{
    set_test_name("MULI");

    cg.emit(vm::MULI);
    const sl::int_t a = 1252;
    const sl::int_t b = 8587;
    const sl::int_t result = a * b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<17>()
{
    set_test_name("MULF");

    cg.emit(vm::MULF);
    const sl::float_t a = 12352.2f;
    const sl::float_t b = 85487.3f;
    const sl::float_t result = a * b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<18>()
{
    set_test_name("DIVI");

    cg.emit(vm::DIVI);
    const sl::int_t a = 13252;
    const sl::int_t b = 887;
    const sl::int_t result = a / b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<19>()
{
    set_test_name("DIVF");

    cg.emit(vm::DIVF);
    const sl::float_t a = 12352.2f;
    const sl::float_t b = 85487.3f;
    const sl::float_t result = a / b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<20>()
{
    set_test_name("MODI");

    cg.emit(vm::MODI);
    const sl::int_t a = 12352;
    const sl::int_t b = 857;
    const sl::int_t result = a % b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<21>()
{
    set_test_name("MODF");

    cg.emit(vm::MODF);
    const sl::float_t a = 12352.2f;
    const sl::float_t b = 857.3f;
    const sl::float_t result = std::fmod(a, b);

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<22>()
{
    set_test_name("NEGI");

    cg.emit(vm::NEGI);
    const sl::int_t a = 12352;
    const sl::int_t result = -a;

    push(a);

    run();

    ensure_unary_op(result);
}

template <>
template <>
void object::test<23>()
{
    set_test_name("NEGF");

    cg.emit(vm::NEGF);
    const sl::float_t a = 12352.2f;
    const sl::float_t result = -a;

    push(a);

    run();

    ensure_unary_op(result);
}

template <>
template <>
void object::test<24>()
{
    set_test_name("ANDI");

    cg.emit(vm::ANDI);
    const sl::int_t a = 12352;
    const sl::int_t b = 857;
    const sl::int_t result = a & b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<25>()
{
    set_test_name("ORI");

    cg.emit(vm::ORI);
    const sl::int_t a = 12352;
    const sl::int_t b = 857;
    const sl::int_t result = a | b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<26>()
{
    set_test_name("XORI");

    cg.emit(vm::XORI);
    const sl::int_t a = 12352;
    const sl::int_t b = 857;
    const sl::int_t result = a ^ b;

    push(a);
    push(b);

    run();

    ensure_binary_op(result);
}

template <>
template <>
void object::test<27>()
{
    set_test_name("NOTI");

    cg.emit(vm::NOTI);
    const sl::int_t a = 12352;
    const sl::int_t result = ~a;

    push(a);

    run();

    ensure_unary_op(result);
}


template <>
template <>
void object::test<28>()
{
    set_test_name("I2F");

    cg.emit(vm::I2F);
    const sl::int_t a = 12352;
    const sl::float_t result = sl::int_t(a);

    push(a);

    run();

    ensure_unary_op(result);
}


template <>
template <>
void object::test<29>()
{
    set_test_name("F2I");

    cg.emit(vm::F2I);
    const sl::float_t a = 12352.7f;
    const sl::int_t result = sl::int_t(a);

    push(a);

    run();

    ensure_unary_op(result);
}

template <>
template <>
void object::test<30>()
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
void object::test<31>()
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
void object::test<32>()
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
void object::test<33>()
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
void object::test<34>()
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
void object::test<35>()
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
void object::test<36>()
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
void object::test<37>()
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
void object::test<38>()
{
    set_test_name("LTI");

    cg.emit(vm::LTI);

    const sl::int_t l1 = 1252;
    const sl::int_t r1 = 8587;
    const sl::int_t result1 = 1;

    push(l1);
    push(r1);

    run();

    ensure_binary_op(result1);

    env = origEnv;

    const sl::int_t l2 = 64;
    const sl::int_t r2 = 23;
    const sl::int_t result2 = 0;

    push(l2);
    push(r2);

    run();

    ensure_binary_op(result2);

    env = origEnv;

    const sl::int_t l3 = 44;
    const sl::int_t r3 = 44;
    const sl::int_t result3 = 0;

    push(l3);
    push(r3);

    run();

    ensure_binary_op(result3);
}

}