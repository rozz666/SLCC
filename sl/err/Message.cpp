#include <sl/err/Message.hpp>

namespace sl
{

namespace err
{

std::ostream& operator<<(std::ostream& os, const Message& m)
{
    return os << m.pos << ": error " << m.id << ": " << m.text;
}

}
}