#include "astar.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

bool AStar::Vec2i::operator==(const Vec2i& coord_) {
    return (x == coord_.x && y == coord_.y);
}

AStar::Vec2i operator+(const AStar::Vec2i& coord_1, const AStar::Vec2i& coord_2) {
    return {coord_1.x + coord_2.x, coord_1.y + coord_2.y};
}

AStar::Node::Node(Vec2i coordinates_, Node* parent_) {
    parent = parent_;
    coordinates = coordinates_;
    G = H = 0;
}

int AStar::Node::getScore(){
    return G + H; 
}

AStar::Generator::Generator(Vec2i worldsize_) {
    worldsize = worldsize_;
    directions = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
        { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
    };
}

void AStar::Generator::addCollision(Vec2i coord_) {
    walls.push_back(coord_);
}

int AStar::Generator::heuristic(Vec2i source_, Vec2i target_) {
    Vec2i delta = {target_.x-source_.x, target_.y-source_.y};
    return static_cast<int>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

bool AStar::Generator::detectCollision(Vec2i coord_) {
    if (coord_.x < 0 || coord_.x >= worldsize.x ||
        coord_.y < 0 || coord_.y >= worldsize.y ||
        std::find(walls.begin(), walls.end(), coord_) != walls.end()) {
        return true;
    }
    return false;
}

AStar::Node* AStar::Generator::findNodeOnList(NodeList& nodes_, Vec2i coordinates_)
{
    for (auto node : nodes_) {
        if (node->coordinates == coordinates_) {
            return node;
        }
    }
    return nullptr;
}

void AStar::Generator::releaseNodes(NodeList& nodes_) {
    for (int i = 0; i < nodes_.size(); i++)
        delete nodes_[i];
}

AStar::CoordinateList AStar::Generator::findPath(Vec2i start_, Vec2i goal_, int distance) {
    Node* current = nullptr;
    NodeList openList, closeList;
    openList.reserve(100);
    closeList.reserve(100);
    openList.push_back(new Node(start_));

    while (!openList.empty()) {
        auto current_it = openList.begin();
        current = *current_it;
        for (auto it = openList.begin(); it != openList.end(); it++) {
            auto node = *it;
            if (node->getScore() < current->getScore()) {
                current = node;
                current_it = it;
            }
        }

        if (current->coordinates == goal_) break;

        closeList.push_back(current);
        openList.erase(current_it);

        for (int i = 0; i < directions.size(); i++) {
            Vec2i newCoord(current->coordinates + directions[i]);
            if (detectCollision(newCoord) || findNodeOnList(closeList, newCoord)) continue;
            int totalCost = current->G + ((i < 4) ? 10 : 14);

            Node* successor = findNodeOnList(openList, newCoord);
            if (successor == nullptr) {
                successor = new Node(newCoord, current);
                successor->G = totalCost;
                successor->H = heuristic(successor->coordinates, goal_);
                openList.push_back(successor);
            } else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost;
            }
        }
    }

    CoordinateList path;
    while(current != nullptr) {
        path.push_back(current->coordinates);
        current = current->parent;
    }

    releaseNodes(openList);
    releaseNodes(closeList);

    return path;
}