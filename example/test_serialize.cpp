#include <iostream>

using namespace std;
#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>

using namespace hnu::cmw::serialize;

enum QosDurabilityPolicy {
  DURABILITY_SYSTEM_DEFAULT = 0,
  DURABILITY_TRANSIENT_LOCAL = 1,
  DURABILITY_VOLATILE = 2,
};


enum RoleType {
  ROLE_NODE = 1,
  ROLE_WRITER = 2,
  ROLE_READER = 3,
  ROLE_SERVER = 4,
  ROLE_CLIENT = 5,
  ROLE_PARTICIPANT = 6,
};

class Data_test : public Serializable
{
public:
    string a;
    uint32_t b;
    RoleType role;
    QosDurabilityPolicy policy = DURABILITY_SYSTEM_DEFAULT;
    int c;
    void show(){
        std::cout << "a:" << a << " b:" << b << " role:" <<(int)role << " c:" << c <<std::endl;
    }
    SERIALIZE(a,b,role,c,policy)
};


class Data_pb : public Serializable
{
public:
    string c;
    Data_test b;
    void show(){
        std::cout << "c:" << c <<std::endl;
    }
    SERIALIZE(c,b)
};

int main()
{

    DataStream ds;

    Data_test data;
    data.a = "test";
    data.role = ROLE_CLIENT;
    data.b = 5;
    data.c = 500;


    Data_pb data_test;
    data_test.b = data;
    data_test.c = "test";
    ds << data_test;

    Data_pb data1;

    ds >> data1;
    data1.b.show();
    data1.show();
    return 0;
}