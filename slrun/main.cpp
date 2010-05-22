#include <iostream>
#include <fstream>
#include <sl/vm.hpp>

sl::vm::BytecodeBuffer readFile(const std::string& fname)
{
    using namespace sl;

    std::ifstream fin(fname.c_str());

    if (!fin.is_open()) throw std::runtime_error("Cannot open file: " + fname);

    sl::vm::BytecodeBuffer buf;
    buf.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());

    return buf;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "use: slrun <file.slbin>" << std::endl;

        return 1;
    }

    try
    {
        namespace vm = sl::vm;

        sl::vm::BytecodeBuffer buf = readFile(argv[1]);
        vm::Environment env(65536, std::cin, std::cout);

        env.execute(&buf[0], 0);
    }
    catch (const std::exception& e)
    {
        std::cout << "slrun: " << e.what() << std::endl;

        return 1;
    }
}