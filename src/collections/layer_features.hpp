#ifndef __NODE_GDAL_FEATURE_COLLECTION_H__
#define __NODE_GDAL_FEATURE_COLLECTION_H__

// node
#include <node.h>
#include <node_object_wrap.h>

// nan
#include "../nan-wrapper.h"

// gdal
#include <gdal_priv.h>

#include "dataset_collection.hpp"
#include "../async.hpp"

using namespace v8;
using namespace node;

namespace node_gdal {

class Feature;
class Layer;

class LayerFeatures : public DatasetCollection<LayerFeatures, OGRFeature *, OGRLayer *, Feature, Layer> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "LayerFeatures";
  static void Initialize(Local<Object> target);

  GDAL_ASYNCABLE_DECLARE(get);
  GDAL_ASYNCABLE_DECLARE(first);
  GDAL_ASYNCABLE_DECLARE(next);
  GDAL_ASYNCABLE_DECLARE(count);
  GDAL_ASYNCABLE_DECLARE(add);
  GDAL_ASYNCABLE_DECLARE(set);
  GDAL_ASYNCABLE_DECLARE(remove);

  LayerFeatures();

    private:
  ~LayerFeatures();
};

} // namespace node_gdal
#endif
