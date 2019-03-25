// Aseprite Document Library
// Copyright (c) 2019  Igara Studio S.A.
// Copyright (c) 2001-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doc/sort_palette.h"

#include "doc/image.h"
#include "doc/palette.h"
#include "doc/remap.h"
#include "gfx/hsv.h"
#include "gfx/rgb.h"

#include <algorithm>
#include <limits>

namespace doc {

using namespace gfx;

namespace {

struct PalEntryWithIndex {
  int index;
  color_t color;
};

struct PalEntryWithIndexPredicate {
  SortPaletteBy channel;
  bool ascending;

  PalEntryWithIndexPredicate(SortPaletteBy channel, bool ascending)
    : channel(channel)
    , ascending(ascending) {
  }

  bool operator()(const PalEntryWithIndex& a, const PalEntryWithIndex& b) {
    color_t c1 = a.color;
    color_t c2 = b.color;
    int value1 = 0, value2 = 0;

    switch (channel) {

      case SortPaletteBy::RED:
        value1 = rgba_getr(c1);
        value2 = rgba_getr(c2);
        break;

      case SortPaletteBy::GREEN:
        value1 = rgba_getg(c1);
        value2 = rgba_getg(c2);
        break;

      case SortPaletteBy::BLUE:
        value1 = rgba_getb(c1);
        value2 = rgba_getb(c2);
        break;

      case SortPaletteBy::ALPHA:
        value1 = rgba_geta(c1);
        value2 = rgba_geta(c2);
        break;

      case SortPaletteBy::HUE:
      case SortPaletteBy::SATURATION:
      case SortPaletteBy::VALUE: {
        Hsv hsv1(Rgb(rgba_getr(c1),
                     rgba_getg(c1),
                     rgba_getb(c1)));
        Hsv hsv2(Rgb(rgba_getr(c2),
                     rgba_getg(c2),
                     rgba_getb(c2)));

        switch (channel) {
          case SortPaletteBy::HUE:
            value1 = hsv1.hueInt();
            value2 = hsv2.hueInt();
            break;
          case SortPaletteBy::SATURATION:
            value1 = hsv1.saturationInt();
            value2 = hsv2.saturationInt();
            break;
          case SortPaletteBy::VALUE:
            value1 = hsv1.valueInt();
            value2 = hsv2.valueInt();
            break;
          default:
            ASSERT(false);
            break;
        }
        break;
      }

      case SortPaletteBy::LIGHTNESS: {
        value1 = (std::max(rgba_getr(c1), std::max(rgba_getg(c1), rgba_getb(c1))) +
                  std::min(rgba_getr(c1), std::min(rgba_getg(c1), rgba_getb(c1)))) / 2;
        value2 = (std::max(rgba_getr(c2), std::max(rgba_getg(c2), rgba_getb(c2))) +
                  std::min(rgba_getr(c2), std::min(rgba_getg(c2), rgba_getb(c2)))) / 2;
        break;
      }

      case SortPaletteBy::LUMA: {
        value1 = rgb_luma(rgba_getr(c1), rgba_getg(c1), rgba_getb(c1));
        value2 = rgb_luma(rgba_getr(c2), rgba_getg(c2), rgba_getb(c2));
        break;
      }

    }

    if (!ascending)
      std::swap(value1, value2);

    return (value1 < value2);
  }
};

} // anonymous namespace

// Travelling Salesman Sort:
void travelling_salesman_sort(const Palette* palette,
                              std::vector<PalEntryWithIndex> &paletteWithIndex,
                              const bool ascending)
{
  if (palette->size()>2) {
    int size = palette->size();
    // To simplyfy the path search, we arbitrarily impose the path start point as:
    // "The color with the greatest luma" (as you see, this code doesn't search ALL
    // the possible paths)
    int greatest_luma = 0;
    if (ascending)
      greatest_luma = std::numeric_limits<int>::max();
    int greatest_index = 0;
    std::vector<color_t> temp(size, 0);
    for (int i=0; i<size; i++) {
      temp[i] = palette->getEntry(i);
      if (ascending) {
        if (rgba_luma(palette->getEntry(i)) < greatest_luma) {
          greatest_luma = rgba_luma(palette->getEntry(i));
          greatest_index = i;
        }
      }
      else {
        if (rgba_luma(palette->getEntry(i)) > greatest_luma) {
          greatest_luma = rgba_luma(palette->getEntry(i));
          greatest_index = i;
        }
      }
    }
    paletteWithIndex[0].color = palette->getEntry(greatest_index);
    paletteWithIndex[0].index = greatest_index;
    temp[greatest_index] = 0;
    for (int i=1; i<size; i++) {
      int minimum_distance_index = 0;
      int minimum_distance = std::numeric_limits<int>::max();
      for (int j=0; j<temp.size(); j++) {
        if (temp[j] != 0 && minimum_distance > rgb_distance(temp[j], paletteWithIndex[i-1].color)) {
          paletteWithIndex[i].color = temp[j];
          paletteWithIndex[i].index = j;
          minimum_distance = rgb_distance(temp[j], paletteWithIndex[i-1].color);
          minimum_distance_index = j;
        }
      }
      temp[minimum_distance_index] = 0;
    }
  }
}

Remap sort_palette(const Palette* palette,
                   const SortPaletteBy channel,
                   const bool ascending)
{
  std::vector<PalEntryWithIndex> tmp(palette->size());
  if (channel == SortPaletteBy::TRAVELLING_SALESMAN)
    travelling_salesman_sort(palette, tmp, ascending);
  else {
    for (int i=0; i<palette->size(); ++i) {
      tmp[i].index = i;
      tmp[i].color = palette->getEntry(i);
    }
    std::stable_sort(tmp.begin(), tmp.end(), PalEntryWithIndexPredicate(channel, ascending));
  }

  Remap remap(palette->size());
  for (int i=0; i<palette->size(); ++i)
    remap.map(tmp[i].index, i);

  return remap;
}

} // namespace doc
