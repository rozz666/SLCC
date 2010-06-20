#ifndef SL_FILEPOSITION_HPP
#define SL_FILEPOSITION_HPP

#include <iosfwd>
#include <string>
#include <boost/flyweight.hpp>

namespace sl
{

struct FilePosition
{
    boost::flyweight<std::string> file;
    int line, column;

    FilePosition() : line(0), column(0) { }
    FilePosition(const std::string& file, int line, int column) : file(file), line(line), column(column) { }
};

inline bool operator==(const FilePosition& left, const FilePosition& right)
{
    return left.file == right.file && left.line == right.line && left.column == right.column;
}

inline bool operator!=(const FilePosition& left, const FilePosition& right)
{
    return !(left == right);
}

inline bool operator<(const FilePosition& left, const FilePosition& right)
{
    return
        left.file < right.file ||
        (left.file == right.file && (left.line < right.line || 
            (left.line == right.line && left.column < right.column)));
}

std::ostream& operator<<(std::ostream& os, const FilePosition& fp);

}

#endif /* SL_FILEPOSITION_HPP */