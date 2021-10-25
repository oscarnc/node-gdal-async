#ifndef __NODE_GDAL_COMPOUND_CURVES_H__
#define __NODE_GDAL_COMPOUND_CURVES_H__

// node
#include <node.h>
#include <node_object_wrap.h>

// nan
#include "../nan-wrapper.h"

// gdal
#include <gdal_priv.h>

#include "standalone_collection.hpp"
#include "../geometry/gdal_geometry.hpp"
#include "../geometry/gdal_simplecurve.hpp"

using namespace v8;
using namespace node;

// CompoundCurve.curves

namespace node_gdal {

class CompoundCurve;

class CompoundCurveCurves
  : public StandaloneCollection<CompoundCurveCurves, OGRCurve *, OGRCompoundCurve *, Geometry, CompoundCurve> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "CompoundCurveCurves";
  static void Initialize(Local<Object> target);

  static OGRCurve *__get(OGRCompoundCurve *parent, size_t idx);
  static OGRCurve *__get(OGRCompoundCurve *parent, std::string name);
  static int __count(OGRCompoundCurve *parent);
  static NAN_METHOD(add);

  CompoundCurveCurves();

    private:
  ~CompoundCurveCurves();
};

} // namespace node_gdal
#endif
