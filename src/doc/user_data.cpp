// Aseprite Document Library
// Copyright (c) 2001-2015 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doc/user_data.h"

#include "doc/layer.h"
#include "doc/layer_tilemap.h"
#include "doc/tileset.h"

#include <map>
#include <string>

namespace doc {

PropertyMap::PropertyMap()
  : Object(ObjectType::PropertyMap)
  , tilesetId(0)
  , layerId(0) {
  propMap.insert(std::pair<std::string, const int>("test", 222));
}
PropertyMap::PropertyMap(Tileset* tileset)
  : Object(ObjectType::PropertyMap)
  , tilesetId(tileset->id())
  , layerId(0) {
}
PropertyMap::PropertyMap(Layer* layer)
  : Object(ObjectType::PropertyMap)
  , tilesetId(layer->isTilemap() ? static_cast<doc::LayerTilemap*>(layer)->tileset()->id() : 0)
  , layerId(layer->id()) {
}

UserData::UserData() : m_color(0) {
  m_propertyMap = new PropertyMap();
  setProperty("test", 111);
}

const int UserData::getProperty(const std::string& propName) {
  return m_propertyMap->at(propName);
}

void UserData::setProperty(std::string propName, int propValue) {
  m_propertyMap->setProperty(propName, propValue);
}

} // namespace doc
