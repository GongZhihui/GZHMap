#include <iostream>
#include "GZHMap.h"
#include "util.h"

int main()
{
    GZHMap m;
    auto key = MakeUint32();
    Item item = { key, 5 };
    m.insert(item);
    auto item_ptr = m.peekItem(key);
    std::string s;
    if (item_ptr) 
    {
        s = item_ptr->to_string();
    }
    else
    {
        s = "not find key in the map";
    }

    std::cout << s << std::endl;
    std::cin.get();
    return 0;
}