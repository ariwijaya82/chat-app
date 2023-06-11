#ifndef __ASTAR_HPP__
#define __ASTAR_HPP__

#include <vector>

namespace AStar {
    struct Vec2i {
        int x, y;
        bool operator==(const Vec2i&);
    };

    struct Node {
        int G, H;
        Vec2i coordinates;
        Node *parent;

        Node(Vec2i coord_, Node* parent_=nullptr);
        int getScore();
    };

    using CoordinateList = std::vector<Vec2i>;
    using NodeList = std::vector<Node*>;

    class Generator {
        public:
        Generator(Vec2i wordlsize_);
        void addCollision(Vec2i coord_);
        CoordinateList findPath(Vec2i start_, Vec2i goal_, int distance);

        private:
        std::vector<Vec2i> walls, directions;
        Vec2i worldsize;

        int heuristic(Vec2i source_, Vec2i target_);
        bool detectCollision(Vec2i coord_);
        Node* findNodeOnList(NodeList& nodes_, Vec2i coord_);
        void releaseNodes(NodeList& nodes_);
    };
};

#endif
