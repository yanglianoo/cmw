
#ifndef CMW_TRANSPORT_RTPS_ATTRIBUTES_FILLER_H_
#define CMW_TRANSPORT_RTPS_ATTRIBUTES_FILLER_H_
#include <string>
#include <fastrtps/rtps/writer/RTPSWriter.h>
#include <fastrtps/rtps/attributes/WriterAttributes.h>
#include <fastrtps/rtps/attributes/HistoryAttributes.h>
#include <fastrtps/rtps/history/WriterHistory.h>
#include <fastrtps/qos/WriterQos.h>
#include <fastrtps/rtps/history/ReaderHistory.h>
#include <fastrtps/rtps/attributes/ReaderAttributes.h>
#include <cmw/config/qos_profile.h>

using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
namespace hnu    {
namespace cmw   {
namespace transport {

struct RtpsWriterAttributes{
/* Tocpic 的配置*/
    HistoryAttributes hatt;
    WriterAttributes watt;
    WriterQos Wqos;
    TopicAttributes Tatt;
};

struct RtpsReaderAttributes{
  HistoryAttributes hatt;
  ReaderAttributes ratt;
  ReaderQos Rqos;
  TopicAttributes Tatt;
};


class AttributesFiller{

 public:
  AttributesFiller();
  virtual ~AttributesFiller();

  static bool FillInWriterAttr(const std::string& channel_name,
                               const config::QosProfile& qos,
                               RtpsWriterAttributes* writer_attr);

  static bool FillInReaderAttr(const std::string& channel_name,
                               const config::QosProfile& qos,
                               RtpsWriterAttributes* reader_attr);
};

}
}
}

#endif