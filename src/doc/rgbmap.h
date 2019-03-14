// Aseprite Document Library
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DOC_RGBMAP_H_INCLUDED
#define DOC_RGBMAP_H_INCLUDED
#pragma once

#include "base/debug.h"
#include "base/disable_copying.h"
#include "doc/object.h"

#include <vector>

namespace doc {

  class Palette;

  // It acts like a cache for Palette:findBestfit() calls.
  class RgbMap : public Object {
    // Bit activated on m_map entries that aren't yet calculated.
    const int INVALID = 256;

  public:
    RgbMap();

    bool match(const Palette* palette) const;
    void regenerate(const Palette* palette, int mask_index);

    int mapColor(int r, int g, int b, int a) const {
      ASSERT(r >= 0 && r < 256);
      ASSERT(g >= 0 && g < 256);
      ASSERT(b >= 0 && b < 256);
      ASSERT(a >= 0 && a < 256);
      // bits -> rrrrrgggggbbbbbaaa
      int i = (a>>5) | ((b>>3) << 3) | ((g>>3) << 8) | ((r>>3) << 13);
      int v = m_map[i];
      if (v & INVALID) {
        m_6bitTempPaletteSize++;
        return generateEntry(i, r, g, b, a);
      }
      else
        return v;
      //return (v & INVALID) ? generateEntry(i, r, g, b, a): v;
    }

    void clear6bitTempPaletteSize() const { m_6bitTempPaletteSize = 0; }
    int get6bitTempPaletteElement(int i) const {return m_6bitTempPalette[i]; }
    uint8_t getBitsRes() const {return m_bitResolution; }
    void setBitsRes(uint8_t i) const { m_bitResolution = i; }

    void mapColor6bitsPressicion(int r, int g, int b, int a) const
    {
      ASSERT(r >= 0 && r < 256);
      ASSERT(g >= 0 && g < 256);
      ASSERT(b >= 0 && b < 256);
      ASSERT(a >= 0 && a < 256);
      int truncatedColor = ((a | 0xfc)<<24) | ((b & 0xfc) << 16) | ((g & 0xfc) << 8) | (r & 0xfc);
      int maxIterations = m_6bitTempPaletteSize + 1;
      int i;
      for (i=0; i<maxIterations; i++) {
        if (truncatedColor == m_6bitTempPalette[i])
            break;
      }
      if (i>=maxIterations) {
        // No se encontró color entry que corresponda
        // Agregar elemento a m_6bitTempPalette y sumar Size de octreeSize, SOLO si existen menos de 256 elementos.
        if (m_6bitTempPaletteSize < 256) {
          m_6bitTempPalette[i] = truncatedColor;
          m_6bitTempPaletteSize++;
        }
      }
    }

    int maskIndex() const { return m_maskIndex; }
    int TempPalette6bitSize() const { return m_6bitTempPaletteSize; }
    
  private:
    int generateEntry(int i, int r, int g, int b, int a) const;

    mutable std::vector<uint16_t> m_map;
    mutable std::vector<int> m_6bitTempPalette;
    mutable int m_6bitTempPaletteSize;
    mutable uint8_t m_bitResolution;
    const Palette* m_palette;
    int m_modifications;
    int m_maskIndex;

    DISABLE_COPYING(RgbMap);
  };

} // namespace doc

#endif
