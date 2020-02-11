// Aseprite
// Copyright (c) 2020 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DOC_OCTREEMAP_H_INCLUDED
#define DOC_OCTREEMAP_H_INCLUDED
#pragma once

#include "doc/image_impl.h"
#include "doc/palette.h"

#include <vector>
#include <array>

namespace doc {

class OctreeMap;
class OctreeNode;

class OctreeNode {
private:
  class LeafColor {
  public:
    LeafColor() :
      m_r(0),
      m_g(0),
      m_b(0),
      m_pixelCount(0)
    {}

    LeafColor(int r, int g, int b, int pixelCount) :
      m_r((double)r),
      m_g((double)g),
      m_b((double)b),
      m_pixelCount(pixelCount)
    {}

    void add(color_t c)
    {
      m_r += rgba_getr(c);
      m_g += rgba_getg(c);
      m_b += rgba_getb(c);
      m_pixelCount++;
    }

    void add(LeafColor leafColor)
    {
      m_r += leafColor.m_r;
      m_g += leafColor.m_g;
      m_b += leafColor.m_b;
      m_pixelCount += leafColor.m_pixelCount;
    }

    LeafColor normalizeColor()
    {
      int auxR = (((int)m_r) % m_pixelCount > m_pixelCount / 2)? 1 : 0;
      int auxG = (((int)m_g) % m_pixelCount > m_pixelCount / 2)? 1 : 0;
      int auxB = (((int)m_b) % m_pixelCount > m_pixelCount / 2)? 1 : 0;
      return LeafColor(m_r / m_pixelCount + auxR,
                           m_g / m_pixelCount + auxG,
                           m_b / m_pixelCount + auxB,
                           m_pixelCount);
    }

    color_t LeafColorToColor()
    {
      return 0xff000000 + (((int)m_b) << 16) + (((int)m_g) << 8) + (int)m_r;
    }

    int pixelCount() { return m_pixelCount; }

private:
    double m_r;
    double m_g;
    double m_b;
    int m_pixelCount;
  };

public:
  OctreeNode()
  {
    m_paletteIndex = 0;
    m_children.reset(nullptr);
    m_parent = nullptr;
  };

  static int getOctet(color_t c, int level)
  {
    int aux = c >> (7 - level);
    int octet = aux & 1;
    aux = aux >> (7);
    octet += (aux & 2);
    return octet + ((aux >> 7) & 4);
  }

  static color_t octetToBranchColor(int octet, int level)
  {
    int auxR = (octet & 1) << (7 - level);
    int auxG = (octet & 2) << (14 - level);
    int auxB = (octet & 4) << (21 - level);
    return auxR + auxG + auxB;
  }

  OctreeNode* parent() const { return m_parent; }
  std::array<OctreeNode, 8>* children() const { return m_children.get(); }
  LeafColor getLeafColor() const { return m_leafColor; }

  void addColor(color_t c, int level, OctreeNode* parent,
                int paletteIndex = 0, int levelDeep = 7);

  void fillOrphansNodes(const Palette* palette, color_t upstreamBranchColor, int level);

  void fillMostSignificantNodes(int level);

  int mapColor(int r, int g, int b, int level);

  void collectLeafNodes(std::vector<OctreeNode*>* leavesVector, int& paletteIndex);

  // removeLeaves(): remove leaves from a common parent
  // auxParentVector: i/o addreess of an auxiliary parent leaf Vector from outside.
  // rootLeavesVector: i/o address of the m_root->m_leavesVector
  int removeLeaves(std::vector<OctreeNode*>& auxParentVector,
                   std::vector<OctreeNode*>& rootLeavesVector,
                   int octreeDeep = 7);

private:
  bool isLeaf() { return m_leafColor.pixelCount() > 0; }
  void paletteIndex(int index) { m_paletteIndex = index; }

  LeafColor m_leafColor;
  int m_paletteIndex;
  std::unique_ptr<std::array<OctreeNode, 8>> m_children;
  OctreeNode* m_parent;
};


class OctreeMap {

public:
  OctreeMap() :
    m_root(new OctreeNode()),
    m_leavesVector(new std::vector<OctreeNode*>()),
    m_modifications(-1)
  {}

  void addColor(color_t color, int levelDeep = 7)
  {
    m_root->addColor(color, 0, m_root.get(), 0, levelDeep);
  }

  // makePalette return true if a 7 level octreeDeep is OK, and false if we can add ONE level deep.
  bool makePalette(Palette* palette, int colorCount, int leveleep = 7);

  void feedWithImage(Image* image, color_t maskColor, int levelDeep = 7);

  int mapColor(int r, int g, int b) const;

  void regenerate(const Palette* palette, int maskIndex);

  int getModifications() const { return m_modifications; };

private:
  OctreeNode* root() const { return m_root.get(); };
  void fillOrphansNodes(Palette* palette);

  std::unique_ptr<OctreeNode> m_root;
  std::unique_ptr<std::vector<OctreeNode*>> m_leavesVector;
  color_t m_maskColor;
  int m_modifications;
};

} // namespace doc
#endif
