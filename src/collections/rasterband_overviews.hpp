#ifndef __NODE_GDAL_BAND_OVERVIEWS_H__
#define __NODE_GDAL_BAND_OVERVIEWS_H__

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
class RasterBand;

class RasterBandOverviews
  : public DatasetCollection<RasterBandOverviews, GDALRasterBand *, GDALRasterBand *, RasterBand, RasterBand> {
    public:
    public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static constexpr const char *_className = "RasterBandOverviews";
  static void Initialize(Local<Object> target);

  static GDALRasterBand *__get(GDALRasterBand *parent, size_t idx);
  static GDALRasterBand *__get(GDALRasterBand *parent, std::string const &name);
  static int __count(GDALRasterBand *parent);

  GDAL_ASYNCABLE_DECLARE(getBySampleCount);

  RasterBandOverviews();

    private:
  ~RasterBandOverviews();
};

} // namespace node_gdal
#endif
