#ifndef SL_DEPENDENCYGRAPH_HPP
#define SL_DEPENDENCYGRAPH_HPP

#include <unordered_map>
#include <unordered_set>

namespace sl
{

template <typename T>
class DependencyGraph
{
public:

    void insert(const T *o, const T *dep)
    {
        map_.insert(std::make_pair(o, dep));
    }

    template <typename F>
    void visit(const T *s, F f)
    {
        std::vector<const T *> stack;
        std::unordered_set<const T *> visited;
        std::vector<const T *> path;

        stack.push_back(s);

        while (!stack.empty())
        {
            const T *o = stack.back();
            stack.pop_back();
            path.push_back(o);
            auto r = map_.equal_range(s);

            std::for_each(r.first, r.second,
                [&stack, &visited](const Map::value_type& p)
                {
                    if (!visited.count(p.second))
                    {
                        stack.push_back(p.second);
                        visited.insert(p.second);
                    }
                });
        }
        
        std::for_each(path.rbegin(), path.rend(), f);
    }

private:

    typedef std::unordered_multimap<const T *, const T *> Map;
    Map map_;
};

}

#endif /* SL_DEPENDENCYGRAPH_HPP */