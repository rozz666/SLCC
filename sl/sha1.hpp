#ifndef SL_SHA1
#define SL_SHA1

#include <cstdint>
#include <ostream>
#include <iomanip>
#include <vector>
#include <iterator>
#include <boost/utility/enable_if.hpp>

namespace sl
{
    
struct Sha1
{
    std::uint32_t h0;
    std::uint32_t h1;
    std::uint32_t h2;
    std::uint32_t h3;
    std::uint32_t h4;

    Sha1() : h0(0), h1(0), h2(0), h3(0), h4(0) { }
    Sha1(std::uint32_t h0, std::uint32_t h1, std::uint32_t h2, std::uint32_t h3, std::uint32_t h4)
        : h0(h0), h1(h1), h2(h2), h3(h3), h4(h4) { }
};

bool operator==(const Sha1& left, const Sha1& right)
{
    return left.h0 == right.h0 && left.h1 == right.h1 && left.h2 == right.h2 && left.h3 == right.h3 && left.h4 == right.h4;
}

bool operator!=(const Sha1& left, const Sha1& right)
{
    return !(left == right);
}

std::ostream& operator<<(std::ostream& os, const Sha1& h)
{
    std::ostream::sentry ok(os);

    if (ok)
    {
        auto flags = os.flags();

        os.fill('0');
        os << std::hex
            << (h.h0 >> 28) << ((h.h0 >> 24) & 0xf) << ((h.h0 >> 20) & 0xf) << ((h.h0 >> 16) & 0xf)
            << ((h.h0 >> 12) & 0xf) << ((h.h0 >> 8) & 0xf) << ((h.h0 >> 4) & 0xf) << (h.h0 & 0xf)
            << (h.h1 >> 28) << ((h.h1 >> 24) & 0xf) << ((h.h1 >> 20) & 0xf) << ((h.h1 >> 16) & 0xf)
            << ((h.h1 >> 12) & 0xf) << ((h.h1 >> 8) & 0xf) << ((h.h1 >> 4) & 0xf) << (h.h1 & 0xf)
            << (h.h2 >> 28) << ((h.h2 >> 24) & 0xf) << ((h.h2 >> 20) & 0xf) << ((h.h2 >> 16) & 0xf)
            << ((h.h2 >> 12) & 0xf) << ((h.h2 >> 8) & 0xf) << ((h.h2 >> 4) & 0xf) << (h.h2 & 0xf)
            << (h.h3 >> 28) << ((h.h3 >> 24) & 0xf) << ((h.h3 >> 20) & 0xf) << ((h.h3 >> 16) & 0xf)
            << ((h.h3 >> 12) & 0xf) << ((h.h3 >> 8) & 0xf) << ((h.h3 >> 4) & 0xf) << (h.h3 & 0xf)
            << (h.h4 >> 28) << ((h.h4 >> 24) & 0xf) << ((h.h4 >> 20) & 0xf) << ((h.h4 >> 16) & 0xf)
            << ((h.h4 >> 12) & 0xf) << ((h.h4 >> 8) & 0xf) << ((h.h4 >> 4) & 0xf) << (h.h4 & 0xf);

        os.flags(flags);
    }

    return os;
}

namespace detail
{

std::uint32_t leftRotate(std::uint32_t w, unsigned short n)
{
#ifdef _MSC_VER
    return _rotl(w, n);
#else
    return (w << n) | (w >> (32 - n));
#endif
}

std::uint32_t swapEndianness(std::uint32_t v)
{
    return (leftRotate(v, 24) & 0xff00ff00) | (leftRotate(v, 8) & 0xff00ff);
}

void sha1Chunk(Sha1& h, std::uint32_t buf[80])
{
    // h - little-endian, w - big-endian
    for (unsigned i = 16; i != 80; ++i)
    {
        buf[i] = swapEndianness(leftRotate(swapEndianness(buf[i - 3] ^ buf[i - 8] ^ buf[i - 14] ^ buf[i - 16]), 1));
    }

    std::uint32_t a(h.h0), b(h.h1), c(h.h2), d(h.h3), e(h.h4), f, k; // little-endian

    for (unsigned i = 0; i != 80; ++i)
    {
        if (i < 20)
        {
            f  = (b & c) | (~b & d);
            k = 0x5a827999;
        }
        else if (i < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ed9eba1;
        }
        else if (i < 60)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8f1bbcdc;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xca62c1d6;
        }

        std::uint32_t tmp = leftRotate(a, 5) + f + e + k + swapEndianness(buf[i]);
        e = d;
        d = c;
        c = leftRotate(b, 30); 
        b = a;
        a = tmp;
    }

    h.h0 += a;
    h.h1 += b;
    h.h2 += c;
    h.h3 += d;
    h.h4 += e;
}

}

template <typename InIt>
typename boost::enable_if_c<sizeof(std::iterator_traits<InIt>::value_type) == 1, Sha1>::type
    sha1(InIt first, InIt last)
{
    std::vector<std::uint8_t> buf;
    std::uint32_t wbuf[80];
    std::uint64_t size = 0;
    buf.reserve(64);

    Sha1 h(0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0);

    for (; first != last; ++first, ++size)
    {
        buf.push_back(*first);

        if (buf.size() == 64)
        {
            std::memcpy(wbuf, &buf.front(), 64);
            detail::sha1Chunk(h, wbuf);
            buf.clear();
        }
    }

    size *= 8;

    buf.push_back(0x80);

    if (buf.size() >= 56)
    {
        buf.resize(64, 0);

        std::memcpy(wbuf, &buf.front(), 64);
        detail::sha1Chunk(h, wbuf);
        buf.clear();
    }

    buf.resize(56, 0);
    buf.push_back(std::uint8_t(size >> 56));
    buf.push_back(std::uint8_t(size >> 48));
    buf.push_back(std::uint8_t(size >> 40));
    buf.push_back(std::uint8_t(size >> 32));
    buf.push_back(std::uint8_t(size >> 24));
    buf.push_back(std::uint8_t(size >> 16));
    buf.push_back(std::uint8_t(size >> 8));
    buf.push_back(std::uint8_t(size));

    std::memcpy(wbuf, &buf.front(), 64);
    detail::sha1Chunk(h, wbuf);

    return h;
}


}

#endif /* SL_SHA1 */