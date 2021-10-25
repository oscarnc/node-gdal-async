#ifndef __NODE_GDAL_LINESTRING_POINTS_H__
#define __NODE_GDAL_LINESTRING_POINTS_H__

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

// LineString.children

namespace node_gdal {

class LineString;
class Geometry;

class LineStringPoints
  : public StandaloneCollection<LineStringPoints, OGRPoint *, OGRLineString *, Geometry, LineString> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "LineStringPoints";
  static void Initialize(Local<Object> target);

  static OGRPoint *__get(OGRLineString *parent, size_t idx);
  static OGRPoint *__get(OGRLineString *parent, std::string name);
  static int __count(OGRLineString *parent);

  static NAN_METHOD(add);
  static NAN_METHOD(get);
  static NAN_METHOD(set);
  static NAN_METHOD(reverse);
  static NAN_METHOD(resize);

  LineStringPoints();

    private:
  ~LineStringPoints();
};

} // namespace node_gdal
#endif
