#include <tut/tut.hpp> 
#include <sl/sha1.hpp>
#include <boost/lexical_cast.hpp>

namespace tut
{

struct sha1_Test_data
{
};

typedef test_group<sha1_Test_data> tg;
typedef tg::object object;
tg sha1_group("SHA-1 test");

template <>
template <>
void object::test<1>()
{
    using namespace sl;

    set_test_name("default value");

    std::string str;
    ensure_equals(Sha1(), Sha1(0, 0, 0, 0, 0));
}

template <>
template <>
void object::test<2>()
{
    using namespace sl;

    set_test_name("operator<<");

    ensure_equals(boost::lexical_cast<std::string>(Sha1(0x12345678, 0xabcdef01, 0x0723afda, 0xb03efab0, 0xf2eba987)), "12345678abcdef010723afdab03efab0f2eba987");
    ensure_equals(boost::lexical_cast<std::string>(Sha1(0x89aedfae, 0x7dfa0edf, 0xa9ed6f39, 0x7f23d123, 0xf0b1fbad)), "89aedfae7dfa0edfa9ed6f397f23d123f0b1fbad");
}

template <>
template <>
void object::test<3>()
{
    using namespace sl;

    set_test_name("empty");

    std::string empty;
    ensure_equals(sha1(empty.begin(), empty.end()), Sha1(0xda39a3ee, 0x5e6b4b0d, 0x3255bfef, 0x95601890, 0xafd80709));
}

template <>
template <>
void object::test<4>()
{
    using namespace sl;

    set_test_name("value");

    std::string str1 = "The quick brown fox jumps over the lazy dog";
    ensure_equals("value1", sha1(str1.begin(), str1.end()), Sha1(0x2fd4e1c6, 0x7a2d28fc, 0xed849ee1, 0xbb76e739, 0x1b93eb12));
    std::string str2 = "The quick brown fox jumps over the lazy cog";
    ensure_equals("value2", sha1(str2.begin(), str2.end()), Sha1(0xde9f2c7f, 0xd25e1b3a, 0xfad3e85a, 0x0bd17d9b, 0x100db4b3));
    std::string str3(1024, 'x');
    ensure_equals("value3", sha1(str3.begin(), str3.end()), Sha1(0xd5a3c9bd, 0x7e746c98, 0xb4aea0e9, 0x194fb955, 0x5b3c22ad));
}

template <>
template <>
void object::test<5>()
{
    set_test_name("leftRotate");

    ensure_equals(sl::detail::leftRotate(0x12345678, 0), 0x12345678u);
    ensure_equals(sl::detail::leftRotate(0x12345678, 16), 0x56781234u);
    ensure_equals(sl::detail::leftRotate(0x12345678, 4), 0x23456781u);
    ensure_equals(sl::detail::leftRotate(0x12345678, 24), 0x78123456u);
}

template <>
template <>
void object::test<6>()
{
    set_test_name("swapEndianness");

    ensure_equals(sl::detail::swapEndianness(0x12345678), 0x78563412u);
    ensure_equals(sl::detail::swapEndianness(0x87654321), 0x21436587u);
}

}
