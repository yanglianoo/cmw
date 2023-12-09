#pragma once
#include <iostream>
#include <string>

#include <iostream>
#include <sstream>

struct data_hello {
    int index;
    std::string data;

    // 序列化为字符串流
    friend std::ostream& operator<<(std::ostream& os, const data_hello& obj) {
        os << obj.index << " " << obj.data.size() << " " << obj.data;
        return os;
    }

    // 从字符串流反序列化
    friend std::istream& operator>>(std::istream& is, data_hello& obj) {
        int dataSize;
        is >> obj.index >> dataSize;
        obj.data.resize(dataSize);
        is.ignore(1); // Ignore the space between dataSize and data
        is.read(&obj.data[0], dataSize);
        return is;
    }
};
