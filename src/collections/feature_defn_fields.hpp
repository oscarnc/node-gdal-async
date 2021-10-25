#ifndef __NODE_GDAL_FIELD_DEFN_COLLECTION_H__
#define __NODE_GDAL_FIELD_DEFN_COLLECTION_H__

// node
#include <node.h>
#include <node_object_wrap.h>

// nan
#include "../nan-wrapper.h"

// gdal
#include <gdal_priv.h>

#include "standalone_collection.hpp"
#include "../gdal_feature.hpp"

using namespace v8;
using namespace node;

// FeatureDefn.fields : FeatureDefnFields

namespace node_gdal {

class Feature;
class FeatureDefn;
class FieldDefn;

class FeatureDefnFields
  : public StandaloneCollection<FeatureDefnFields, OGRFieldDefn *, OGRFeatureDefn *, FieldDefn, FeatureDefn> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "FeatureDefnFields";
  static void Initialize(Local<Object> target);

  static int __count(OGRFeatureDefn *parent);

  static NAN_METHOD(get);
  static NAN_METHOD(getNames);
  static NAN_METHOD(add);
  static NAN_METHOD(remove);
  static NAN_METHOD(indexOf);
  static NAN_METHOD(reorder);

  // - implement in the future -
  // static NAN_METHOD(alter);

  static NAN_GETTER(featureDefnGetter);

  FeatureDefnFields();

    private:
  ~FeatureDefnFields();
};

} // namespace node_gdal
#endif
