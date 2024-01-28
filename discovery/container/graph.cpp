


#include <cmw/discovery/container/graph.h>

#include <queue>

namespace hnu {
namespace cmw {
namespace discovery{ 

Vertice::Vertice(const std::string& val) : value_(val) {}

Vertice::Vertice(const Vertice& other) { this->value_ = other.value_;}

Vertice::~Vertice() {}


Vertice& Vertice::operator=(const Vertice& rhs){
    if(this != &rhs) {
        this->value_ = rhs.value_;
    }
    return *this; 
}

bool Vertice::operator!=(const Vertice& rhs) const {
    return this->value_ != rhs.value_;
}

bool Vertice::operator==(const Vertice& rhs) const {
    return this->value_ == rhs.value_;
}

bool Vertice::IsDummy() const { return value_.empty();}

const std::string& Vertice::GetKey() const { return value_; }

Edge::Edge() {}

Edge::Edge(const Edge& other) {
  this->src_ = other.src_;
  this->dst_ = other.dst_;
  this->value_ = other.value_;
}

Edge::Edge(const Vertice& src, const Vertice& dst, const std::string& val)
    : src_(src), dst_(dst), value_(val) {}

Edge::~Edge() {}

Edge& Edge::operator=(const Edge& rhs) {
  if (this != &rhs) {
    this->src_ = rhs.src_;
    this->dst_ = rhs.dst_;
    this->value_ = rhs.value_;
  }
  return *this;
}

bool Edge::operator==(const Edge& rhs) const {
  return this->src_ == rhs.src_ && this->dst_ == rhs.dst_ &&
         this->value_ == rhs.value_;
}

bool Edge::IsValid() const {
  if (value_.empty()) {
    return false;
  }
  if (src_.IsDummy() && dst_.IsDummy()) {
    return false;
  }
  return true;
}

//边的值加上目标点的值
std::string Edge::GetKey() const { return value_ + "_" + dst_.GetKey(); }


Graph::Graph() {}

Graph::~Graph(){
    edges_.clear();
    list_.clear();
}

void Graph::Insert(const Edge& e){
    //判断此边是否为空
    if(!e.IsValid()){
        return;
    }
    WriteLockGuard<AtomicRWLock> lock(rw_lock_);
    auto& e_v = e.value();
    //为此边新建两个顶点的邻接表
    if(edges_.find(e_v) == edges_.end()){
        edges_[e_v] = RelatedVertices();
    }

    //如果此边的源顶点不为空，则为源顶点插入一条出边
    if(!e.src().IsDummy()){
        InsertOutgoingEdge(e);
    }
    //如果此边的目标顶点不为空，则为目标顶点插入一条入边
    if(!e.dst().IsDummy()){
        InsertIncomingEdge(e);
    }
}

void Graph::Delete(const Edge& e) {
  if (!e.IsValid()) {
    return;
  }
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  auto& e_v = e.value();
  if (edges_.find(e_v) == edges_.end()) {
    return;
  }

  //如果此边的源顶点不为空，则为源顶点删除一条出边
  if (!e.src().IsDummy()) {
    DeleteOutgoingEdge(e);
  }
  //如果此边的目标顶点不为空，则为目标顶点删除一条入边
  if (!e.dst().IsDummy()) {
    DeleteIncomingEdge(e);
  }
}

//获取边的数量
uint32_t Graph::GetNumOfEdge() {
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  uint32_t num = 0;
  for (auto& item : list_) {
    num += static_cast<int>(item.second.size());
  }
  return num;
}


FlowDirection Graph::GetDirectionOf(const Vertice& lhs, const Vertice& rhs) {
  if (lhs.IsDummy() || rhs.IsDummy()) {
    return UNREACHABLE;
  }
  ReadLockGuard<AtomicRWLock> lock(rw_lock_);
  if (list_.count(lhs.GetKey()) == 0 || list_.count(rhs.GetKey()) == 0) {
    return UNREACHABLE;
  }
  if (LevelTraverse(lhs, rhs)) {
    return UPSTREAM;
  }
  if (LevelTraverse(rhs, lhs)) {
    return DOWNSTREAM;
  }
  return UNREACHABLE;
}

//插入一条完整的边
void Graph::InsertCompleteEdge(const Edge& e)
{
    //拿到插入边的出发顶点的value
    auto& src_v_k = e.src().GetKey();
    //如果没有找到此顶点的邻接表，则新建一个新的邻接表
    if(list_.find(src_v_k) == list_.end()){
        list_[src_v_k] = VerticeSet();
    }

    //拿到插入边的终点的value
    auto& dst_v_k = e.dst().GetKey();
    //如果没有找到此顶点的邻接表，则新建一个新的邻接表，如果邻接表存在，则不管
    if(list_.find(dst_v_k) == list_.end()){
        list_[dst_v_k] = VerticeSet();
    }

    //
    list_[src_v_k][e.GetKey()] = e.dst();
}

//删除一条完整的边
void Graph::DeleteCompleteEdge(const Edge& e){
    auto& src_v_k = e.src().GetKey();
    list_[src_v_k].erase(e.GetKey());
}

//插入一条入边
void Graph::InsertIncomingEdge(const Edge& e){
    //拿到入边的value
    auto& e_v = e.value();
    //拿到入边的目标顶点的连接表
    auto& dst_v_set = edges_[e_v].dst;
    //拿到入边的目标顶点
    auto& dst_v = e.dst();
    //拿到入边的目标顶点的value
    auto& v_k = dst_v.GetKey();

    if(dst_v_set.find(v_k) != dst_v_set.end()){
        return;
    }
    //代表指向自己
    dst_v_set[v_k] = dst_v;

    //拿到入边的出发顶点
    auto& src_v_set = edges_[e_v].src;
    //插入一条完整的边
    Edge insert_e;
    insert_e.set_dst(dst_v);
    insert_e.set_value(e.value());
    for(auto& item : src_v_set){
        insert_e.set_src(item.second);
        InsertCompleteEdge(insert_e);
    }
}

//插入一条出边
void Graph::InsertOutgoingEdge(const Edge& e) {
  
  //拿到出边的value
  auto& e_v = e.value();
  //拿到出边的源顶点的邻接表
  auto& src_v_set = edges_[e_v].src;
  //拿到出边的源顶点
  auto& src_v = e.src();
  //拿到出边源顶点的value
  auto& v_k = src_v.GetKey();
  if (src_v_set.find(v_k) != src_v_set.end()) {
    return;
  }
  //代表指向自己
  src_v_set[v_k] = src_v;

  auto& dst_v_set = edges_[e_v].dst;
  Edge insert_e;
  insert_e.set_src(src_v);
  insert_e.set_value(e.value());
  for (auto& item : dst_v_set) {
    insert_e.set_dst(item.second);
    InsertCompleteEdge(insert_e);
  }
}

//删除一条出边
void Graph::DeleteOutgoingEdge(const Edge& e) {
  

  auto& e_v = e.value();
  auto& src_v_set = edges_[e_v].src;
  auto& src_v = e.src();
  auto& v_k = src_v.GetKey();
  if (src_v_set.find(v_k) == src_v_set.end()) {
    return;
  }
  //从出发顶点的邻接表中删除自己
  src_v_set.erase(v_k);

  //从全局的邻接表map中删除
  auto& dst_v_set = edges_[e_v].dst;
  Edge delete_e;
  delete_e.set_src(src_v);
  delete_e.set_value(e.value());
  for (auto& item : dst_v_set) {
    delete_e.set_dst(item.second);
    DeleteCompleteEdge(delete_e);
  }
}

//删除一条入边
void Graph::DeleteIncomingEdge(const Edge& e) {
  auto& e_v = e.value();
  auto& dst_v_set = edges_[e_v].dst;
  auto& dst_v = e.dst();
  auto& v_k = dst_v.GetKey();
  if (dst_v_set.find(v_k) == dst_v_set.end()) {
    return;
  }
  //从入边的目标顶点的邻接表中删除自己
  dst_v_set.erase(v_k);
  
  //从全局的邻接表map中删除
  auto& src_v_set = edges_[e_v].src;
  Edge delete_e;
  delete_e.set_dst(dst_v);
  delete_e.set_value(e.value());
  for (auto& item : src_v_set) {
    delete_e.set_src(item.second);
    DeleteCompleteEdge(delete_e);
  }
}


//图的广度优先搜索（BFS）算法
bool Graph::LevelTraverse(const Vertice& start, const Vertice& end) {
  std::unordered_map<std::string, bool> visited;
  visited[end.GetKey()] = false;
  std::queue<Vertice> unvisited;
  unvisited.emplace(start);
  while (!unvisited.empty()) {
    auto curr = unvisited.front();
    unvisited.pop();
    if (visited[curr.GetKey()]) {
      continue;
    }
    visited[curr.GetKey()] = true;
    if (curr == end) {
      break;
    }
    for (auto& item : list_[curr.GetKey()]) {
      unvisited.push(item.second);
    }
  }
  return visited[end.GetKey()];
}

}
}
}