#include "rasterband_overviews.hpp"
#include "../gdal_common.hpp"
#include "../gdal_rasterband.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> RasterBandOverviews::constructor;

void RasterBandOverviews::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  DatasetCollection<RasterBandOverviews, GDALRasterBand *, GDALRasterBand *, RasterBand, RasterBand>::Initialize(
    target);
  Local<FunctionTemplate> lcons = Nan::New(constructor);

  Nan__SetPrototypeAsyncableMethod(lcons, "getBySampleCount", getBySampleCount);

  Nan::Set(target, Nan::New(_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());
}

RasterBandOverviews::RasterBandOverviews()
  : DatasetCollection<RasterBandOverviews, GDALRasterBand *, GDALRasterBand *, RasterBand, RasterBand>() {
}

RasterBandOverviews::~RasterBandOverviews() {
}

/**
 * An encapsulation of a {{#crossLink
 * "gdal.RasterBand"}}RasterBand{{/crossLink}} overview functionality.
 *
 * @class gdal.RasterBandOverviews
 */

/**
 * Fetches the overview at the provided index.
 *
 * @method get
 * @throws Error
 * @param {number} index 0-based index
 * @return {gdal.RasterBand}
 */

/**
 * Fetches the overview at the provided index.
 * {{{async}}}
 *
 * @method getAsync
 * @throws Error
 * @param {number} index 0-based index
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.RasterBand>}
 */

GDALRasterBand *RasterBandOverviews::__get(GDALRasterBand *parent, size_t idx) {
  return parent->GetOverview(idx);
}

GDALRasterBand *RasterBandOverviews::__get(GDALRasterBand *parent, std::string const &name) {
  throw "index must be a number";
}
/**
 * Fetch best sampling overview.
 *
 * Returns the most reduced overview of the given band that still satisfies the
 * desired number of samples. This function can be used with zero as the number
 * of desired samples to fetch the most reduced overview. The same band as was
 * passed in will be returned if it has not overviews, or if none of the
 * overviews have enough samples.
 *
 * @method getBySampleCount
 * @param {number} samples
 * @return {gdal.RasterBand}
 */

/**
 * Fetch best sampling overview.
 * {{{async}}}
 *
 * Returns the most reduced overview of the given band that still satisfies the
 * desired number of samples. This function can be used with zero as the number
 * of desired samples to fetch the most reduced overview. The same band as was
 * passed in will be returned if it has not overviews, or if none of the
 * overviews have enough samples.
 *
 * @method getBySampleCountAsync
 * @param {number} samples
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.RasterBand>}
 */
GDAL_ASYNCABLE_DEFINE(RasterBandOverviews::getBySampleCount) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  NODE_UNWRAP_CHECK(RasterBand, parent, band);

  int n_samples;
  NODE_ARG_INT(0, "minimum number of samples", n_samples);

  GDALAsyncableJob<GDALRasterBand *> job(band->parent_uid);
  job.persist(parent);
  job.main = [band, n_samples](const GDALExecutionProgress &) {
    CPLErrorReset();
    GDALRasterBand *result = band->get()->GetRasterSampleOverview(n_samples);
    if (result == nullptr) { throw "Specified overview not found"; }
    return result;
  };
  job.rval = [band](GDALRasterBand *result, GetFromPersistentFunc) {
    return RasterBand::New(result, band->getParent());
  };
  job.run(info, async, 1);
}

/**
 * Returns the number of overviews.
 *
 * @method count
 * @return {number}
 */

/**
 * Returns the number of overviews.
 *
 * @method countAsync
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */

int RasterBandOverviews::__count(GDALRasterBand *parent) {
  return parent->GetOverviewCount();
}

} // namespace node_gdal
