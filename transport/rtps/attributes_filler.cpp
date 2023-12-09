/**
 * @File Name: attributes_filler.cpp
 * @brief  
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2023-12-06
 * 
 */
#include <cmw/transport/rtps/attributes_filler.h>
namespace hnu    {
namespace cmw   {
namespace transport {


AttributesFiller::AttributesFiller() {}
AttributesFiller::~AttributesFiller() {}

bool AttributesFiller::FillInWriterAttr(const std::string& channel_name,
                                        const config::QosProfile& qos,
                                        RtpsWriterAttributes* writer_attr)
{
    /*配置 writer 的topic*/
    writer_attr->Tatt.topicName = channel_name;
    writer_attr->Tatt.topicDataType = "string";  
    writer_attr->Tatt.topicKind =  NO_KEY;   


    /*配置 writer 的history*/
    writer_attr->hatt.payloadMaxSize = 255;
    writer_attr->hatt.maximumReservedCaches = 50;

    /*配置 writer 的Attributes*/
    writer_attr->watt.endpoint.reliabilityKind = BEST_EFFORT;


    /* 配置writer 的qos*/
    switch (qos.durability)
    {
    case config::QosHistoryPolicy::HISTORY_KEEP_LAST:
        writer_attr->Wqos.m_durability.kind = eprosima::fastrtps::TRANSIENT_LOCAL_DURABILITY_QOS;;
        break;
    case config::QosDurabilityPolicy::DURABILITY_VOLATILE:
        writer_attr->Wqos.m_durability.kind = eprosima::fastrtps::VOLATILE_DURABILITY_QOS;
    default:
        break;
    }

    switch (qos.reliability)
    {
    case config::QosReliabilityPolicy::RELIABILITY_BEST_EFFORT:
        writer_attr->Wqos.m_reliability.kind = eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS;
        break;
    case config::QosReliabilityPolicy::RELIABILITY_RELIABLE:
        writer_attr->Wqos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
    default:
        break;
    }

    /*配置 writer 的history*/

    switch (qos.history)
    {
    case config::QosHistoryPolicy::HISTORY_KEEP_LAST:
        writer_attr->Tatt.historyQos.kind =  eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
        break;
    case config::QosHistoryPolicy::HISTORY_KEEP_ALL:
        writer_attr->Tatt.historyQos.kind = eprosima::fastrtps::KEEP_ALL_HISTORY_QOS;
    default:
        break;
    }
            
}



bool AttributesFiller::FillInReaderAttr(const std::string& channel_name,
                                        const config::QosProfile& qos,
                                        RtpsWriterAttributes* reader_attr)
{

    reader_attr->Tatt.topicName = channel_name;
    reader_attr->Tatt.topicDataType = "string";
    reader_attr->Tatt.topicKind = NO_KEY;

    reader_attr->hatt.payloadMaxSize = 255;

    reader_attr->watt.endpoint.reliabilityKind = BEST_EFFORT;
    /* 配置reader 的qos*/
    switch (qos.durability)
    {
    case config::QosHistoryPolicy::HISTORY_KEEP_LAST:
        reader_attr->Wqos.m_durability.kind = eprosima::fastrtps::TRANSIENT_LOCAL_DURABILITY_QOS;;
        break;
    case config::QosDurabilityPolicy::DURABILITY_VOLATILE:
        reader_attr->Wqos.m_durability.kind = eprosima::fastrtps::VOLATILE_DURABILITY_QOS;
    default:
        break;
    }

    switch (qos.reliability)
    {
    case config::QosReliabilityPolicy::RELIABILITY_BEST_EFFORT:
        reader_attr->Wqos.m_reliability.kind = eprosima::fastrtps::BEST_EFFORT_RELIABILITY_QOS;
        break;
    case config::QosReliabilityPolicy::RELIABILITY_RELIABLE:
        reader_attr->Wqos.m_reliability.kind = eprosima::fastrtps::RELIABLE_RELIABILITY_QOS;
    default:
        break;
    }

    /*配置 reader 的history*/

    switch (qos.history)
    {
    case config::QosHistoryPolicy::HISTORY_KEEP_LAST:
        reader_attr->Tatt.historyQos.kind =  eprosima::fastrtps::KEEP_LAST_HISTORY_QOS;
        break;
    case config::QosHistoryPolicy::HISTORY_KEEP_ALL:
        reader_attr->Tatt.historyQos.kind = eprosima::fastrtps::KEEP_ALL_HISTORY_QOS;
    default:
        break;
    }

}


}
}
}