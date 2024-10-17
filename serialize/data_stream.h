#ifndef CMW_SERIALIZE_DATA_STREADM_H_
#define CMW_SERIALIZE_DATA_STREADM_H_

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstring>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmw/serialize/serializable.h>
#include <cmw/common/log.h>
#include <cmw/time/time.h>

using namespace std;
using std::ifstream;
using std::ofstream;
using std::stringstream;
namespace hnu{
namespace cmw{
namespace serialize{

class DataStream
{
public:
    enum DataType
    {
        BOOL = 0,  
        CHAR,
        INT32,
        INT64,
        UINT32,
        UINT64,
        FLOAT,
        DOUBLE,
        ENUM,
        STRING,
        VECTOR,
        LIST,
        MAP,
        SET,
        CUSTOM
    };

    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    DataStream();
    DataStream(const string & data);
    DataStream(const char* ptr, size_t size);
    ~DataStream();

    void show() const;
    void write(const char * data, int len);
    void write(bool value);
    void write(char value);
    void write(int32_t value);
    void write(uint32_t value);
    void write(uint64_t value);
    void write(int64_t value);
    void write(float value);
    void write(double value);
    void write(const char * value);
    void write(const string & value);
    void write(const Serializable & value);
 
    template <typename T>
    void write(const std::vector<T> & value);

    template <typename T>
    void write(const std::list<T> & value);

    template <typename K, typename V>
    void write(const std::map<K, V> & value);

    template <typename T>
    void write(const std::set<T> & value);

    //采用SFINAE特性保证T为模板类型
    template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
    void write(const T& value);

    template <typename T, typename ...Args>
    void write_args(const T & head, const Args&... args);

    void write_args();

    bool read(char * data, int len);
    bool read(bool & value);
    bool read(char & value);
    bool read(int32_t & value);
    bool read(uint32_t& value);
    bool read(uint64_t& value);
    bool read(int64_t & value);
    bool read(float & value);
    bool read(double & value);
    bool read(string & value);
    bool read(Serializable & value);

    template <typename T>
    bool read(std::vector<T> & value);

    template <typename T>
    bool read(std::list<T> & value);

    template <typename K, typename V>
    bool read(std::map<K, V> & value);

    template <typename T>
    bool read(std::set<T> & value);

    //采用SFINAE特性保证T为枚举类型
    template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
    bool read(T& value);

    template <typename T, typename ...Args>
    bool read_args(T & head, Args&... args);

    bool read_args();

    const char * data() const;
    int size() const;
    size_t ByteSize();
    void clear();
    void reset();
    void save(const string & filename);
    void load(const string & filename);

    DataStream & operator << (bool value);
    DataStream & operator << (char value);
    DataStream & operator << (int32_t value);
    DataStream & operator << (int64_t value);
    DataStream & operator << (uint32_t value);
    DataStream & operator << (uint64_t value);
    DataStream & operator << (float value);
    DataStream & operator << (double value);
    DataStream & operator << (const char * value);
    DataStream & operator << (const string & value);
    DataStream & operator << (const Serializable & value);

    template <typename T>
    DataStream & operator << (const std::vector<T> & value);

    template <typename T>
    DataStream & operator << (const std::list<T> & value);

    template <typename K, typename V>
    DataStream & operator << (const std::map<K, V> & value);

    template <typename T>
    DataStream & operator << (const std::set<T> & value);

    DataStream & operator >> (bool & value);
    DataStream & operator >> (char & value);
    DataStream & operator >> (int32_t & value);
    DataStream & operator >> (int64_t & value);
    DataStream & operator >> (uint32_t & value);
    DataStream & operator >> (uint64_t & value);
    DataStream & operator >> (float & value);
    DataStream & operator >> (double & value);
    DataStream & operator >> (string & value);
    DataStream & operator >> (Serializable & value);

    template <typename T>
    DataStream & operator >> (std::vector<T> & value);

    template <typename T>
    DataStream & operator >> (std::list<T> & value);

    template <typename K, typename V>
    DataStream & operator >> (std::map<K, V> & value);

    template <typename T>
    DataStream & operator >> (std::set<T> & value);

private:
    void reserve(int len);
    ByteOrder byteorder();

private:
    std::vector<char> m_buf;
    int m_pos;
    ByteOrder m_byteorder;
};

template <typename T>
void DataStream::write(const std::vector<T> & value)
{
    char type = DataType::VECTOR;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);

    uint64_t start = Time::Now().ToMicrosecond();
    // for (int i = 0; i < len; i++)
    // {
    //     write(value[i]);
    // }
    const T* ptr = value.data();
    const char* char_ptr = reinterpret_cast<const char*>(ptr);
    write(char_ptr, value.size());

    // 记录结束时间
    uint64_t end = Time::Now().ToMicrosecond();

    // 计算耗时（以微秒为单位）
    uint64_t elapsed  = end - start;
    AINFO << "代码执行时间: " << elapsed << " 微秒" ;
}

template <typename T>
void DataStream::write(const std::list<T> & value)
{
    char type = DataType::LIST;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write((*it));
    }
}

template <typename K, typename V>
void DataStream::write(const std::map<K, V> & value)
{
    char type = DataType::MAP;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write(it->first);
        write(it->second);
    }
}

template <typename T>
void DataStream::write(const std::set<T> & value)
{
    char type = DataType::SET;
    write((char *)&type, sizeof(char));
    int len = value.size();
    write(len);
    for (auto it = value.begin(); it != value.end(); it++)
    {
        write(*it);
    }
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
void DataStream::write(const T& value) {
    write(static_cast<int32_t>(value));
}

template <typename T, typename ...Args>
void DataStream::write_args(const T & head, const Args&... args)
{
    // if constexpr (std::is_enum_v<T>){
    //     int32_t intValue = static_cast<int32_t>(head);
    //     write(intValue);
    // }else{
    //     write(head);
    // }
    write(head);
    write_args(args...);
}

template <typename T>
bool DataStream::read(std::vector<T> & value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::VECTOR)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    // for (int i = 0; i < len; i++)
    // {
    //     T v;
    //     read(v);
    //     value.emplace_back(v);
    // }
    value.resize(len);
    char* char_ptr = value.data();
    read(char_ptr, len);
    return true;
}

template <typename T>
bool DataStream::read(std::list<T> & value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::LIST)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        T v;
        read(v);
        value.push_back(v);
    }
    return true;
}

template <typename K, typename V>
bool DataStream::read(std::map<K, V> & value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::MAP)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        K k;
        read(k);

        V v;
        read(v);
        value[k] = v;
    }
    return true;
}

template <typename T>
bool DataStream::read(std::set<T> & value)
{
    value.clear();
    if (m_buf[m_pos] != DataType::SET)
    {
        return false;
    }
    ++m_pos;
    int len;
    read(len);
    for (int i = 0; i < len; i++)
    {
        T v;
        read(v);
        value.insert(v);
    }
    return true;
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
bool DataStream::read(T& value)
{
    int32_t& intValue = reinterpret_cast<int32_t&>(value);
    return read(intValue);
}

template <typename T, typename ...Args>
bool DataStream::read_args(T & head, Args&... args)
{
    read(head);
    return read_args(args...);
}

template <typename T>
DataStream & DataStream::operator << (const std::vector<T> & value)
{
    write(value);
    return *this;
}

template <typename K, typename V>
DataStream & DataStream::operator << (const std::map<K, V> & value)
{
    write(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator << (const std::set<T> & value)
{
    write(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::vector<T> & value)
{
    read(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::list<T> & value)
{
    read(value);
    return *this;
}

template <typename K, typename V>
DataStream & DataStream::operator >> (std::map<K, V> & value)
{
    read(value);
    return *this;
}

template <typename T>
DataStream & DataStream::operator >> (std::set<T> & value)
{
    read(value);
    return *this;
}


}
}
}


#endif