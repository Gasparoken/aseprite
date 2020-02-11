// Aseprite
// Copyright (C) 2020  Igara Studio S.A.
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_UI_MAP_ALGORITHM_SELECTOR_H_INCLUDED
#define APP_UI_MAP_ALGORITHM_SELECTOR_H_INCLUDED
#pragma once

#include "doc/map_algorithm.h"
#include "obs/connection.h"
#include "ui/box.h"
#include "ui/combobox.h"

namespace app {

  class MapAlgorithmSelector : public ui::ComboBox {
  public:

  MapAlgorithmSelector() : m_algorithm(MapAlgorithm::DEFAULT)
  { };

  MapAlgorithm algorithm() { return m_algorithm; };
  void algorithm(MapAlgorithm mapAlgo) { m_algorithm = mapAlgo; };

private:
  MapAlgorithm m_algorithm;
  };

} // namespace app

#endif
