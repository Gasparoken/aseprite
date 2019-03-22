// Aseprite Rener Library
// Copyright (C) 2019 Igara Studio S.A.
// Copyright (c) 2001-2015, 2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef RENDER_QUANTIZATION_H_INCLUDED
#define RENDER_QUANTIZATION_H_INCLUDED
#pragma once

#include "doc/frame.h"
#include "doc/pixel_format.h"
#include "render/color_histogram.h"
#include "render/dithering_algorithm.h"

#include <vector>

namespace doc {
  class Image;
  class Palette;
  class RgbMap;
  class Sprite;
}

namespace render {
  class DitheringMatrix;
  class TaskDelegate;

  class SubtotalPixelsCount{
  public:
    SubtotalPixelsCount() : m_colorCount(0)
    , m_color(0) {};
    
    SubtotalPixelsCount(int colorCount, color_t color) : m_colorCount(colorCount)
    , m_color(color) {};
    
    void SetColorCountAndColor(int colorCount, color_t color)
    {
      m_colorCount = colorCount;
      m_color = color;
    }
    
    int GetColorCount() { return m_colorCount; }
    color_t GetColor() { return m_color; }

  private:
    int m_colorCount;
    color_t m_color;
  };
  
  class Node{
  public:
    Node(): m_nodeLevel(0)
    , m_haveChildren(false)
    , m_withAlpha(false)
    , m_children(8)
    , m_pixel_count(0)
    , m_R_AcumChannel(0)
    , m_G_AcumChannel(0)
    , m_B_AcumChannel(0)
    , m_A_AcumChannel(0)
    {
    }

    Node(int leafLevel, bool haveChildren, int levelDeep, bool withAlpha): m_nodeLevel(leafLevel)
    , m_haveChildren(haveChildren)
    , m_withAlpha(withAlpha)
    , m_pixel_count(0)
    , m_R_AcumChannel(0)
    , m_G_AcumChannel(0)
    , m_B_AcumChannel(0)
    , m_A_AcumChannel(0)
    {
      if (levelDeep > leafLevel+1) {
        for (int i=0; i<((m_withAlpha)? 16 : 8); i++)
          m_children.push_back(Node(leafLevel+1, (levelDeep == leafLevel+2)? false : true, levelDeep, withAlpha));
      }
    }

    void FillSubtotalVector(std::vector<SubtotalPixelsCount> &temp);
    color_t GetColor();
    int GetIndex(color_t c);
    void AddColor(color_t c);
    void GetLeavesCount(int &leaveCountOutput, int &last_node_count, const int &paletteSize);
    void KillLastLevel();
    bool HaveChildren() { return m_haveChildren; }

  private:
    int m_nodeLevel;
    bool m_haveChildren; // If true, current object is a node. If it is false, the object is a Leaf.
    bool m_withAlpha;
    std::vector<Node> m_children;
    int m_pixel_count;
    int m_R_AcumChannel;
    int m_G_AcumChannel;
    int m_B_AcumChannel;
    int m_A_AcumChannel;
  };

  class Octree {
    
  public:
    Octree() : m_palette(256)
    , m_root(0, true, 6, false)
    , m_levelDeep(6)
    {
    }
    
    Octree(int targetQuantity, int levelDeep, bool withAlpha) : m_palette(targetQuantity)
    , m_root(0, true, levelDeep, withAlpha)
    {
      m_levelDeep = CLAMP(1, levelDeep, 8);
    }

    void feedWithImage(Image* image, bool withAlpha);
    void AddColor(color_t c);
    void GeneratePalette(Palette* palette);

  private:
    std::vector<color_t> m_palette;
    Node m_root;
    int m_levelDeep;
  };

  class PaletteOptimizer {
  public:
    void feedWithImage(doc::Image* image, bool withAlpha);
    void feedWithRgbaColor(doc::color_t color);
    void calculate(doc::Palette* palette, int maskIndex);

  private:
    render::ColorHistogram<5, 6, 5, 5> m_histogram;
  };

  // Creates a new palette suitable to quantize the given RGB sprite to Indexed color.
  doc::Palette* create_palette_from_sprite(
    const doc::Sprite* sprite,
    const doc::frame_t fromFrame,
    const doc::frame_t toFrame,
    const bool withAlpha,
    doc::Palette* newPalette, // Can be NULL to create a new palette
    TaskDelegate* delegate,
    const bool newBlend);

  // Changes the image pixel format. The dithering method is used only
  // when you want to convert from RGB to Indexed.
  Image* convert_pixel_format(
    const doc::Image* src,
    doc::Image* dst,         // Can be NULL to create a new image
    doc::PixelFormat pixelFormat,
    render::DitheringAlgorithm ditheringAlgorithm,
    const render::DitheringMatrix& ditheringMatrix,
    const doc::RgbMap* rgbmap,
    const doc::Palette* palette,
    bool is_background,
    doc::color_t new_mask_color,
    TaskDelegate* delegate = nullptr);

} // namespace render

#endif
