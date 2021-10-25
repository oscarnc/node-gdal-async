#ifndef __NODE_GDAL_RASTERBAND_COLLECTION_H__
#define __NODE_GDAL_RASTERBAND_COLLECTION_H__

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

class RasterBand;
class Dataset;

class DatasetBands : public DatasetCollection<DatasetBands, GDALRasterBand *, GDALDataset *, RasterBand, Dataset> {
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "DatasetBands";
  static void Initialize(Local<Object> target);

  static GDALRasterBand *__get(GDALDataset *parent, size_t idx);
  static GDALRasterBand *__get(GDALDataset *parent, std::string const &name);
  static int __count(GDALDataset *parent);

  GDAL_ASYNCABLE_DECLARE(create);

  DatasetBands();

    protected:
  ~DatasetBands();
};

} // namespace node_gdal
#endif
