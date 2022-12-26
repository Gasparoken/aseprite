// Aseprite
// Copyright (C) 2022  Igara Studio S.A.
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/replace_image.h"
#include "app/cmd/set_cel_opacity.h"
#include "app/cmd/set_cel_position.h"
#include "app/doc_api.h"
#include "app/script/docobj.h"
#include "app/script/engine.h"
#include "app/script/luacpp.h"
#include "app/script/userdata.h"
#include "doc/cel.h"
#include "doc/sprite.h"
#include "doc/user_data.h"

namespace app {
namespace script {

using namespace doc;

namespace {

int Properties_get_publisher(lua_State* L) {
  return 0;

}

int Properties_index(lua_State* L) {
  auto ts = get_obj<Tileset>(L, 1);
  auto layer = get_obj<Layer>(L, 1);

  const char* propName = lua_tostring(L, 2);
  const int propValue = lua_tointeger(L, 3);
  if (!propName)
    return luaL_error(L, "propName in '...properties.publisher' must be a string");
//  if (propValue)
//    return luaL_error(L, "propName in '...properties.publisher' must be a string");

  doc::PropertyMap* props = ts->userData().propertyMap();
  int aaa = ts->userData().getProperty("test");
//  props->setProperty(propName, propValue);
//  push_docobj(L, props);
  return 1;
}

const luaL_Reg PropertyMap_methods[] = {
  { "__index", Properties_index },
  { nullptr, nullptr }
};

const Property PropertyMap_properties[] = {
  { "publisher", Properties_get_publisher, nullptr },
  { nullptr, nullptr, nullptr }
};

} // anonymous namespace

DEF_MTNAME(PropertyMap);

void register_properties_class(lua_State* L)
{
//  using PropertyMap = PropertiesMapObj;
  REG_CLASS(L, PropertyMap);
  REG_CLASS_PROPERTIES(L, PropertyMap);
}

void push_properties(lua_State* L, PropertyMap* properties)
{
  push_new<PropertyMap>(L, properties);
}

} // namespace script
} // namespace app
