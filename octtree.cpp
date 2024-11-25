#include <iostream>
#include <vector>
#include <string>

struct Point3D {
    double x, y, z;
};

struct Node {
    double xMin, yMin, zMin, xMax, yMax, zMax; // Bounding box
    std::vector<Point3D> points;                // Points in this node (leaf node)
    Node* children[8];                         // Pointers to children (nullptr if leaf)

    Node(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) :
        xMin(xmin), yMin(ymin), zMin(zmin), xMax(xmax), yMax(ymax), zMax(zmax) {
        for (int i = 0; i < 8; ++i) children[i] = nullptr;
    }
    ~Node() { // Destructor to free memory recursively
        for (int i = 0; i < 8; ++i) {
            delete children[i];
        }
    }
};

int getOctant(const Point3D& p, const Node& node) {
    double midX = (node.xMin + node.xMax) / 2;
    double midY = (node.yMin + node.yMax) / 2;
    double midZ = (node.zMin + node.zMax) / 2;
    int octant = 0;
    if (p.x >= midX) octant |= 4;
    if (p.y >= midY) octant |= 2;
    if (p.z >= midZ) octant |= 1;
    return octant;
}

void insert(Point3D p, Node* node) {
    if (node->points.size() < 2) { // Adjust the threshold as needed
        node->points.push_back(p);
    } else {
        if (node->children[0] == nullptr) {
            double midX = (node->xMin + node->xMax) / 2;
            double midY = (node->yMin + node->yMax) / 2;
            double midZ = (node->zMin + node->zMax) / 2;
            node->children[0] = new Node(node->xMin, node->yMin, node->zMin, midX, midY, midZ);
            node->children[1] = new Node(node->xMin, node->yMin, midZ, midX, midY, node->zMax);
            node->children[2] = new Node(node->xMin, midY, node->zMin, midX, node->yMax, midZ);
            node->children[3] = new Node(node->xMin, midY, midZ, midX, node->yMax, node->zMax);
            node->children[4] = new Node(midX, node->yMin, node->zMin, node->xMax, midY, midZ);
            node->children[5] = new Node(midX, node->yMin, midZ, node->xMax, midY, node->zMax);
            node->children[6] = new Node(midX, midY, node->zMin, node->xMax, node->yMax, midZ);
            node->children[7] = new Node(midX, midY, midZ, node->xMax, node->yMax, node->zMax);

            // CORRECTED: Redistribute points before clearing
            for (const auto& point : node->points) {
                node->children[getOctant(point, *node)]->points.push_back(point);
            }
            node->points.clear();
        }
        insert(p, node->children[getOctant(p, *node)]);
    }
}


void printTree(const Node* node, std::string indent) {
    if (node == nullptr) return;
    std::cout << indent << "[" << node->xMin << ", " << node->yMin << ", " << node->zMin << "] - ["
              << node->xMax << ", " << node->yMax << ", " << node->zMax << "]: ";
    if (node->children[0] == nullptr) {
        std::cout << "{";
        for (size_t i = 0; i < node->points.size(); ++i) {
            std::cout << "(" << node->points[i].x << ", " << node->points[i].y << ", " << node->points[i].z << ")";
            if (i < node->points.size() - 1) std::cout << ", ";
        }
        std::cout << "}" << std::endl;
    } else {
        std::cout << std::endl;
        for (int i = 0; i < 8; ++i) {
            printTree(node->children[i], indent + "  ");
        }
    }
}

int main() {
    Node* root = new Node(0, 0, 0, 7, 7, 7);
    std::vector<Point3D> points = {{1, 1, 1}, {6, 6, 6}, {2, 5, 3}, {3, 2, 1}, {5, 1, 4}, {7, 7, 7}, {4, 4, 4}, {1,1,2}};

    for (const auto& p : points) {
        insert(p, root);
    }

    std::cout << "Octree Structure:" << std::endl;
    printTree(root, "");

    delete root; // Delete the root node to trigger recursive deletion

    return 0;
}