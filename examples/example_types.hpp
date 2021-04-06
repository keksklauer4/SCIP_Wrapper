#ifndef EXAMPLE_TYPES_HPP
#define EXAMPLE_TYPES_HPP

#include <utility>
#include <map>
#include <set>
#include <array>
#include <algorithm>
#include <array>

#include "../src/scip_wrapper.hpp"

namespace example_types
{
  using scip_wrapper::fuint32_t;
  typedef std::pair<fuint32_t, fuint32_t> EdgePair;
  typedef std::vector<fuint32_t> VariableVector;

  struct DependencyKnapsackNode
  {
    DependencyKnapsackNode(std::string _name, fuint32_t _nodeID, fuint32_t _edgeIndex, fuint32_t _nbEdges, double _weight)
      : name(_name), nodeID(_nodeID), edgeIndex(_edgeIndex), nbEdges(_nbEdges), weight(_weight) {}

    std::string name;
    fuint32_t nodeID;
    fuint32_t edgeIndex;
    fuint32_t nbEdges;
    double weight;
  };

  typedef std::vector<DependencyKnapsackNode> DependencyKnapsackItems;
  typedef std::vector<fuint32_t> DependencyVec;


  typedef std::pair<fuint32_t, fuint32_t> CoordinatePair;

  struct MaxFlowEdge
  {
    MaxFlowEdge(fuint32_t _from, fuint32_t _to, double _capacity, double _cost)
      : fromNode(_from), toNode(_to), capacity(_capacity), cost(_cost){}

    public:
      fuint32_t fromNode;
      fuint32_t toNode;
      double capacity;
      double cost;
  };

  typedef std::vector<MaxFlowEdge> MaxFlowEdgeVec;
  typedef std::vector<fuint32_t> NodeIDVec;

  struct Member
  {
    Member(fuint32_t _id, std::string _name)
      :memberID(_id), name(_name){}
    fuint32_t memberID;
    std::string name;
  };
  struct MemberPreferences
  {
    MemberPreferences(fuint32_t _memberID, fuint32_t _teamID, double _cost)
      : memberID(_memberID), teamID(_teamID), cost(_cost) {}
    fuint32_t memberID;
    fuint32_t teamID;
    double cost; // how much would a person suffer if that person had to go to that team (lower is better)
  };

  typedef std::vector<MemberPreferences> PreferenceVec;


  struct LongestPathResult
  {
    std::multimap<fuint32_t, fuint32_t> edgesContained;
    fuint32_t startNode;
    fuint32_t endNode;
    fuint32_t length;
  };


  struct EdgeWithCost
  {
    EdgeWithCost(fuint32_t _from, fuint32_t _to, double _cost)
      : fromNode(_from), toNode(_to), cost(_cost) {}
    fuint32_t fromNode;
    fuint32_t toNode;
    double cost;
  };
  typedef EdgeWithCost weight_edge_t;


  typedef struct Rectangle
  {
    Rectangle(double _x_length, double _y_length, std::string _name)
      : x_length(_x_length), y_length(_y_length), x(_x_length), y(_y_length),
        included(false), name(_name){}

    double getArea() { return x_length * y_length; }
    double x;
    double y;
    bool included;
    double x_length;
    double y_length;
    std::string name;
  } rectangle_t;
}


#endif