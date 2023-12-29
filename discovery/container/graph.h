#ifndef CMW_SERVICE_DISCOVERY_CONTAINER_GRAPH_H_
#define CMW_SERVICE_DISCOVERY_CONTAINER_GRAPH_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <cmw/base/atomic_rw_lock.h>

namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace base;
enum FlowDirection{
  UNREACHABLE,
  UPSTREAM,
  DOWNSTREAM,
};

//代表图中的一个点
class Vertice {
public:
    explicit Vertice(const std::string& val = "");
    Vertice(const Vertice& other);
    virtual ~Vertice();

    Vertice& operator=(const Vertice& rhs);
    bool operator==(const Vertice& rhs) const;
    bool operator!=(const Vertice& rhs) const;

    bool IsDummy() const;
    const std::string& GetKey() const;
    const std::string& value() const { return value_; }
private:
    std::string value_;
};

//代表图中的一条边
class Edge
{

public:
    Edge();
    Edge(const Edge& other);
    Edge(const Vertice& src, const Vertice& dst, const std::string& val);
    virtual ~Edge();

    Edge& operator=(const Edge& rhs);
    bool operator==(const Edge& rhs) const;

    bool IsValid() const;
    std::string GetKey() const;

    const Vertice& src() const {return src_; }
    void set_src(const Vertice& v) { src_ = v; }

    const Vertice& dst() const { return dst_; }
    void set_dst(const Vertice& v) { dst_ = v; }

    const std::string& value() const { return value_; }
    void set_value(const std::string& val) { value_ = val; }

private:
    Vertice src_;
    Vertice dst_;
    std::string value_;
};

//图
class Graph {

public:
    
    //代表一个顶点的邻接表，代表了一个顶点与之相邻的顶点
    using VerticeSet = std::unordered_map<std::string, Vertice>;
    //所有邻接表的集合，以顶点的value作为key
    using AdjacencyList = std::unordered_map<std::string,VerticeSet>;

    Graph();
    virtual ~Graph();

    void Insert(const Edge& e);
    void Delete(const Edge& e);

    uint32_t GetNumOfEdge();
    
    FlowDirection GetDirectionOf(const Vertice& lhs, const Vertice& rhs);

private:
    //描述一条边的结构体，一条边会相邻两个顶点，因此会包含两个邻接表
    struct RelatedVertices {
        RelatedVertices() {}
        VerticeSet src;
        VerticeSet dst;
    };
    using EdgeInfo = std::unordered_map<std::string , RelatedVertices>;

    void InsertOutgoingEdge(const Edge& e);
    void InsertIncomingEdge(const Edge& e);
    void InsertCompleteEdge(const Edge& e);
    void DeleteOutgoingEdge(const Edge& e);
    void DeleteIncomingEdge(const Edge& e);
    void DeleteCompleteEdge(const Edge& e);
    bool LevelTraverse(const Vertice& start, const Vertice& end);


    EdgeInfo edges_;
    AdjacencyList list_;
    AtomicRWLock rw_lock_;
};
}
}
} 

#endif