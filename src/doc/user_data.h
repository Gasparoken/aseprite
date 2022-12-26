// Aseprite Document Library
// Copyright (c) 2001-2015 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DOC_USER_DATA_H_INCLUDED
#define DOC_USER_DATA_H_INCLUDED
#pragma once

#include "doc/color.h"
#include "doc/object.h"

#include <map>
#include <string>

namespace doc {

  class Tileset;
  class Layer;

  class PropertyMap : public Object {
  public:
    PropertyMap();
    PropertyMap(Tileset* tileset);
    PropertyMap(Layer* layer);

    void setProperty(std::string& propName, int propValue) {
      propMap.insert(std::pair<std::string, int>(propName, propValue));
    }
    const int at(const std::string& propName) const {
      return propMap.at(propName);
    }

    ObjectId tsetId() { return tilesetId; }
    ObjectId layId() { return layerId; }

  private:
    std::map<std::string, int> propMap;
    ObjectId tilesetId = 0;
    ObjectId layerId = 0;
  };

  class UserData {
  public:
    UserData();
    ~UserData() {
      if (m_propertyMap) {
        delete m_propertyMap;
        m_propertyMap = nullptr;
      }
    };

    size_t size() const { return m_text.size(); }
    bool isEmpty() const {
      return m_text.empty() && !doc::rgba_geta(m_color);
    }

    const std::string& text() const { return m_text; }
    color_t color() const { return m_color; }
    PropertyMap* propertyMap() { return m_propertyMap; }
    const int getProperty(const std::string& propName);

    void setText(const std::string& text) { m_text = text; }
    void setColor(color_t color) { m_color = color; }
    void setProperty(std::string propName, int propValue);

    bool operator==(const UserData& other) const {
      return (m_text == other.m_text &&
              m_color == other.m_color);
              // TO DO: add PropertyMap
    }

    bool operator!=(const UserData& other) const {
      return !operator==(other);
    }

  private:
    std::string m_text;
    color_t m_color;
    PropertyMap* m_propertyMap = nullptr;
  };

} // namespace doc

#endif
