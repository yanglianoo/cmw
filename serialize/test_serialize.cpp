#include <iostream>

using namespace std;
#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>

using namespace hnu::cmw::serialize;



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
    int b;
    RoleType role;
    int c;
    void show(){
        std::cout << "a:" << a << " b:" << b << " role:" <<role << " c:" << c <<std::endl;
    }
    SERIALIZE(a,b,role,c)
};


class Data_pb : public Serializable
{
public:
    string c;
    Data_test b;
    void show(){
        std::cout << " c:" << c <<std::endl;
    }
    SERIALIZE(c,b)
};

int main()
{


    DataStream ds;
    Data_test data;
    data.a = "test";
    data.b = 100;
    data.role = ROLE_CLIENT;
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