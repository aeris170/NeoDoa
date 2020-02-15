/*
Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
http://code.google.com/p/poly2tri/

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

Neither the name of Poly2Tri nor the names of its contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

namespace P2T {

    // Inital triangle factor, seed triangle will extend 30% of
    // PointSet width to both left and right.
    const double kAlpha = 0.3;

    struct Point;
    class Triangle;
    struct Node;
    struct Edge;
    class AdvancingFront;

    class SweepContext {
    public:
        std::vector<Triangle*> GetTriangles();
        std::list<Triangle*> GetMap();

        std::vector<Edge*> edge_list;

        SweepContext(std::vector<Point*> polyline);
        ~SweepContext();

        inline void set_head(Point* p1) { head_ = p1; }
        inline Point* head() { return head_; }
        inline void set_tail(Point* p1) { tail_ = p1; }
        inline Point* tail() { return tail_; }
        inline int point_count() { return points_.size(); }
        Node& LocateNode(Point& point);
        void RemoveNode(Node* node);
        void CreateAdvancingFront(std::vector<Node*> nodes);

        void MapTriangleToNodes(Triangle& t);
        void AddToMap(Triangle* triangle);
        Point* GetPoint(const int& index);
        Point* GetPoints();
        void RemoveFromMap(Triangle* triangle);
        void AddHole(std::vector<Point*> polyline);
        void AddPoint(Point* point);

        inline AdvancingFront* front() { return front_; }
        void MeshClean(Triangle& triangle);

        struct Basin {
            Node* left_node;
            Node* bottom_node;
            Node* right_node;
            double width;
            bool left_highest;

            Basin() : left_node(NULL), bottom_node(NULL), right_node(NULL), width(0.0), left_highest(false) {}

            void Clear() {
                left_node = NULL;
                bottom_node = NULL;
                right_node = NULL;
                width = 0.0;
                left_highest = false;
            }
        };

        struct EdgeEvent {
            Edge* constrained_edge;
            bool right;

            EdgeEvent() : constrained_edge(NULL), right(false) {}
        };

        Basin basin;
        EdgeEvent edge_event;

    private:
        std::vector<Triangle*> triangles_;
        std::list<Triangle*> map_;
        std::vector<Point*> points_;

        AdvancingFront* front_;
        Point* head_;
        Point* tail_;

        Node *af_head_, *af_middle_, *af_tail_;

        void InitTriangulation();
        void InitEdges(std::vector<Point*> polyline);

        friend class Sweep;
    };
}