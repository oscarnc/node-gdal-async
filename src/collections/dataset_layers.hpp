#ifndef __NODE_GDAL_LAYER_COLLECTION_H__
#define __NODE_GDAL_LAYER_COLLECTION_H__

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

class Dataset;
class Layer;

class DatasetLayers : public DatasetCollection<DatasetLayers, OGRLayer *, GDALDataset *, Layer, Dataset> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "DatasetLayers";
  static void Initialize(Local<Object> target);

  static OGRLayer *__get(GDALDataset *parent, size_t idx);
  static OGRLayer *__get(GDALDataset *parent, std::string const &name);
  static int __count(GDALDataset *parent);

  GDAL_ASYNCABLE_DECLARE(create);
  GDAL_ASYNCABLE_DECLARE(copy);
  GDAL_ASYNCABLE_DECLARE(remove);

  DatasetLayers();

    private:
  ~DatasetLayers();
};

} // namespace node_gdal
#endif
