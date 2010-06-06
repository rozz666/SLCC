#ifndef SL_ERR_MESSAGE_HPP
#define SL_ERR_MESSAGE_HPP

#include <iosfwd>
#include <string>
#include <sl/FilePosition.hpp>

namespace sl
{

namespace err
{

struct Message
{
    FilePosition pos;
    std::string id;
    std::string text;    

    Message(const FilePosition& pos, const std::string& id, const std::string& text) : pos(pos), id(id), text(text) { }
};

std::ostream& operator<<(std::ostream& os, const Message& m);

}
}

#endif /* SL_ERR_MESSAGE_HPP */