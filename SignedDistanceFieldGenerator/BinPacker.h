#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

class BinPacker {
public:
    void Pack(const std::vector<int>& rects, std::vector<std::vector<int>>& packs, int packSize, bool allowRotation = true);

private:
    struct Rect {
        Rect(int size) : x(0), y(0), w(size), h(size), ID(-1), rotated(false), packed(false) {
            children[0] = -1;
            children[1] = -1;
        }

        Rect(int x, int y, int w, int h, int ID = 1) : x(x), y(y), w(w), h(h), ID(ID), rotated(false), packed(false) {
            children[0] = -1;
            children[1] = -1;
        }

        int GetArea() const {
            return w * h;
        }

        void Rotate() {
            std::swap(w, h);
            rotated = !rotated;
        }

        bool operator<(const Rect& rect) const {
            return GetArea() < rect.GetArea();
        }

        int x;
        int y;
        int w;
        int h;
        int ID;
        int children[2];
        bool rotated;
        bool packed;
    };

    void Clear();
    void Fill(int pack, bool allowRotation);
    void Split(int pack, int rect);
    bool Fits(Rect& rect1, const Rect& rect2, bool allowRotation);

    void AddPackToArray(int pack, std::vector<int>& array) const;

    bool RectIsValid(int i) const;
    bool PackIsValid(int i) const;


    int m_packSize;
    int m_numPacked;
    std::vector<Rect> m_rects;
    std::vector<Rect> m_packs;
    std::vector<int> m_roots;
};
