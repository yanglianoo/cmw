#include <iostream>

using namespace std;
#include "data_stream.h"
using namespace cmw::serialize;

int main()
{
    DataStream ds;
    string a = "aaa";
    int c = 100;
    ds << a << c;
    ds.show();
    string b;
    int d;
    ds >> b >> d;
    std::cout<<"b:"<<b<<"d: " <<d<< std::endl;


    return 0;
}