#ifndef SL_FILEPOSITION_HPP
#define SL_FILEPOSITION_HPP

namespace sl
{

struct FilePosition
{
    int line, column;

    FilePosition() : line(0), column(0) { }
    FilePosition(int line, int column) : line(line), column(column) { }
};

inline bool operator==(const FilePosition& left, const FilePosition& right)
{
    return left.line == right.line && left.column == right.column;
}

inline bool operator!=(const FilePosition& left, const FilePosition& right)
{
    return !(left == right);
}

inline bool operator<(const FilePosition& left, const FilePosition& right)
{
    return left.line < right.line || (left.line == right.line && left.column < right.column);
}

}

#endif /* SL_FILEPOSITION_HPP */