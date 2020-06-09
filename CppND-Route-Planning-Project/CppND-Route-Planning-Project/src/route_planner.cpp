#include "route_planner.h"
#include <algorithm>

using std::sort;


RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
  	
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    
	start_node = &m_Model.FindClosestNode(start_x,start_y);
  	end_node = &m_Model.FindClosestNode(end_x,end_y);
}




float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  float h;
  h = node -> distance(*end_node);
    
  return h;

}




void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  
  current_node->FindNeighbors();
  for(RouteModel::Node *n : current_node->neighbors)
  {
    if(n->visited == false)
    {
      float dist = n->distance(*current_node);
      n->parent = current_node;
      n->h_value = CalculateHValue(n);
      n->g_value = dist + current_node->g_value;
      this->open_list.push_back(n);
      n->visited = true;
    }
  }

}




bool Compare(const RouteModel::Node *first, const RouteModel::Node *second) {
  const float f1 = first->g_value + first->h_value; // f1 = g1 + h1
  const float f2 = second->g_value + second->h_value; // f2 = g2 + h2
  return f1 > f2; 
}

RouteModel::Node *RoutePlanner::NextNode() {
  sort(this->open_list.begin(), this->open_list.end(), Compare);
  auto lower = this->open_list.back();
  this->open_list.pop_back();
  return lower;
}




std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
	while(true)
    {
      if(current_node->parent == nullptr)
      {
        //path_found.insert(path_found.begin(),*current_node);
        path_found.push_back(*current_node);
        break;
      }
      distance = distance + current_node->distance(*current_node->parent);
      path_found.push_back(*current_node);
     // path_found.insert(path_found.begin(),*current_node);
      current_node = current_node->parent;
    }
  
  	std::reverse(path_found.begin(), path_found.end());
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}




void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;

    // TODO: Implement your solution here.
  open_list.push_back(start_node);
  start_node->visited = true;
  
  while(open_list.size()>0)
  {
    current_node = NextNode();
    if (!(current_node == end_node))
      AddNeighbors(current_node);
    else
    {
            this->m_Model.path = ConstructFinalPath(current_node);
            return;
     }
  }

}