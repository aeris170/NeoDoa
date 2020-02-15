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

    struct DLL_EXPORT Edge;

    struct DLL_EXPORT Point {

        std::vector<Edge*> edge_list;
        double x, y;

        Point() {  x = 0.0; y = 0.0; }
        Point(double x, double y) : x(x), y(y) {}

        void set_zero() {
            x = 0.0;
            y = 0.0;
        }

        void set(double x_, double y_) {
            x = x_;
            y = y_;
        }

        double Length() const {
            return sqrt(x * x + y * y);
        }

        double Normalize() {
            double len = Length();
            x /= len;
            y /= len;
            return len;
        }

        Point operator -() const {
            Point v;
            v.set(-x, -y);
            return v;
        }

        void operator +=(const Point& v) {
            x += v.x;
            y += v.y;
        }

        void operator -=(const Point& v) {
            x -= v.x;
            y -= v.y;
        }

        void operator *=(double a) {
            x *= a;
            y *= a;
        }
    };

    struct DLL_EXPORT Edge {

        Point* p, *q;

        Edge(Point& p1, Point& p2) : p(&p1), q(&p2) {
            if (p1.y > p2.y) {
                q = &p1;
                p = &p2;
            } else if (p1.y == p2.y) {
                if (p1.x > p2.x) {
                    q = &p1;
                    p = &p2;
                } else if (p1.x == p2.x) {
                    assert(false);
                }
            }
            q->edge_list.push_back(this);
        }
    };

    // Triangle-based data structures are know to have better performance than quad-edge structures
    // See: J. Shewchuk, "Triangle: Engineering a 2D Quality Mesh Generator and Delaunay Triangulator"
    // "Triangulations in CGAL"
    class DLL_EXPORT Triangle {
    public:

        Triangle(Point& a, Point& b, Point& c);

        /// Flags to determine if an edge is a Constrained edge
        bool constrained_edge[3];
        /// Flags to determine if an edge is a Delauney edge
        bool delaunay_edge[3];

        inline Point* GetPoint(const int& index) { return points_[index]; }
        Point* PointCW(Point& point);
        Point* PointCCW(Point& point);
        Point* OppositePoint(Triangle& t, Point& p);

        inline Triangle* GetNeighbor(const int& index) { return neighbors_[index]; }
        void MarkNeighbor(Point* p1, Point* p2, Triangle* t);
        void MarkNeighbor(Triangle& t);

        void MarkConstrainedEdge(const int index);
        void MarkConstrainedEdge(Edge& edge);
        void MarkConstrainedEdge(Point* p, Point* q);

        int Index(const Point* p);
        int EdgeIndex(const Point* p1, const Point* p2);

        Triangle* NeighborCW(Point& point);
        Triangle* NeighborCCW(Point& point);
        bool GetConstrainedEdgeCCW(Point& p);
        bool GetConstrainedEdgeCW(Point& p);
        void SetConstrainedEdgeCCW(Point& p, bool ce);
        void SetConstrainedEdgeCW(Point& p, bool ce);
        bool GetDelunayEdgeCCW(Point& p);
        bool GetDelunayEdgeCW(Point& p);
        void SetDelunayEdgeCCW(Point& p, bool e);
        void SetDelunayEdgeCW(Point& p, bool e);

        inline bool Contains(Point* p) { return p == points_[0] || p == points_[1] || p == points_[2]; }
        inline bool Contains(const Edge& e) { return Contains(e.p) && Contains(e.q); }
        inline bool Contains(Point* p, Point* q) { return Contains(p) && Contains(q); }
        void Legalize(Point& point);
        void Legalize(Point& opoint, Point& npoint);

        void Clear();
        void ClearNeighbor(Triangle *triangle );
        void ClearNeighbors();
        void ClearDelunayEdges();

        inline bool IsInterior() { return interior_; }
        inline void IsInterior(bool b) { interior_ = b; }

        Triangle& NeighborAcross(Point& opoint);

        void DebugPrint();

    private:

        /// Triangle points
        Point* points_[3];
        /// Neighbor list
        Triangle* neighbors_[3];

        /// Has this triangle been marked as an interior triangle?
        bool interior_;
    };

    inline bool cmp(const Point* a, const Point* b) {
        if (a->y < b->y) {
            return true;
        } else if (a->y == b->y) {
            if (a->x < b->x) {
                return true;
            }
        }
        return false;
    }

    inline Point operator +(const Point& a, const Point& b) { return Point(a.x + b.x, a.y + b.y); }
    inline Point operator -(const Point& a, const Point& b) { return Point(a.x - b.x, a.y - b.y); }
    inline Point operator *(double s, const Point& a) { return Point(s * a.x, s * a.y); }
    inline bool operator ==(const Point& a, const Point& b) {  return a.x == b.x && a.y == b.y; }
    inline bool operator !=(const Point& a, const Point& b) { return !(a.x == b.x) && !(a.y == b.y); }
    inline double Dot(const Point& a, const Point& b) { return a.x * b.x + a.y * b.y; }
    inline double Cross(const Point& a, const Point& b) { return a.x * b.y - a.y * b.x; }
    inline Point Cross(const Point& a, double s) { return Point(s * a.y, -s * a.x); }
    inline Point Cross(const double s, const Point& a) { return Point(-s * a.y, s * a.x); }
}