#ifndef __NODE_GDAL_POLYGON_RINGS_H__
#define __NODE_GDAL_POLYGON_RINGS_H__

// node
#include <node.h>
#include <node_object_wrap.h>

// nan
#include "../nan-wrapper.h"

// gdal
#include <gdal_priv.h>

#include "standalone_collection.hpp"

using namespace v8;
using namespace node;

// Polygon.rings

namespace node_gdal {

class Geometry;
class Polygon;

class PolygonRings : public StandaloneCollection<PolygonRings, OGRLinearRing *, OGRPolygon *, Geometry, Polygon> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "PolygonRings";
  static void Initialize(Local<Object> target);

  static NAN_METHOD(get);
  static NAN_METHOD(count);
  static NAN_METHOD(add);
  static NAN_METHOD(remove);

  PolygonRings();

    private:
  ~PolygonRings();
};

} // namespace node_gdal
#endif
