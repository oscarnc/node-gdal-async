#include <memory>
#include "dataset_bands.hpp"
#include "../gdal_common.hpp"
#include "../gdal_dataset.hpp"
#include "../gdal_rasterband.hpp"
#include "../utils/string_list.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> DatasetBands::constructor;

void DatasetBands::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  DatasetCollection<DatasetBands, GDALRasterBand *, GDALDataset *, RasterBand, Dataset>::Initialize(target);
  Local<FunctionTemplate> lcons = Nan::New(constructor);

  Nan__SetPrototypeAsyncableMethod(lcons, "create", create);

  // This is a vicious V8/Node/Nan pitfall:
  // lcons is not a function object, it is a function template
  // this means that it would be instantiated at this very moment,
  // once this happens, further modifications of the prototype template
  // won't have an effect on the actual prototype object in our context
  // -> so this statement can't be in the base class
  Nan::Set(target, Nan::New(_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());
}

DatasetBands::DatasetBands() : DatasetCollection<DatasetBands, GDALRasterBand *, GDALDataset *, RasterBand, Dataset>() {
}

DatasetBands::~DatasetBands() {
}

/**
 * An encapsulation of a {{#crossLink "gdal.Dataset"}}Dataset{{/crossLink}}'s
 * raster bands.
 *
 * ```
 * var bands = dataset.bands;```
 *
 * @class gdal.DatasetBands
 */

/**
 * Returns the band with the given ID.
 *
 * @method get
 * @param {number} id
 * @throws Error
 * @return {gdal.RasterBand}
 */

/**
 * Returns the band with the given ID.
 * {{{async}}}
 *
 * @method getAsync
 *
 * @param {number} id
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @throws Error
 * @return {Promise<gdal.RasterBand>}
 */
GDALRasterBand *DatasetBands::__get(GDALDataset *parent, size_t idx) {
  if (idx > static_cast<size_t>(__count(parent)) || idx == 0) return nullptr;
  return parent->GetRasterBand(idx);
}

GDALRasterBand *DatasetBands::__get(GDALDataset *parent, std::string const &name) {
  throw "index must be a number";
}

/**
 * Adds a new band.
 *
 * @method create
 * @throws Error
 * @param {string} dataType Type of band ({{#crossLink "Constants (GDT)"}}see GDT constants{{/crossLink}}).
 * @param {object|string[]} [options] Creation options
 * @return {gdal.RasterBand}
 */

/**
 * Adds a new band.
 * {{{async}}}
 *
 * @method createAsync
 * @throws Error
 * @param {string} dataType Type of band ({{#crossLink "Constants (GDT)"}}see GDT constants{{/crossLink}}).
 * @param {object|string[]} [options] Creation options
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.RasterBand>}
 */

GDAL_ASYNCABLE_DEFINE(DatasetBands::create) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

  GDALDataset *raw = ds->get();
  GDALDataType type;
  std::shared_ptr<StringList> options(new StringList);

  // NODE_ARG_ENUM(0, "data type", GDALDataType, type);
  if (info.Length() < 1) {
    Nan::ThrowError("data type argument needed");
    return;
  }
  if (info[0]->IsString()) {
    std::string type_name = *Nan::Utf8String(info[0]);
    type = GDALGetDataTypeByName(type_name.c_str());
  } else if (info[0]->IsNull() || info[0]->IsUndefined()) {
    type = GDT_Unknown;
  } else {
    Nan::ThrowError("data type must be string or undefined");
    return;
  }

  if (info.Length() > 1 && options->parse(info[1])) {
    return; // error parsing creation options, options->parse does the throwing
  }

  GDALAsyncableJob<GDALRasterBand *> job(ds->uid);
  job.persist(parent);
  job.main = [raw, type, options](const GDALExecutionProgress &) {
    CPLErrorReset();
    CPLErr err = raw->AddBand(type, options->get());
    if (err != CE_None) { throw CPLGetLastErrorMsg(); }
    return raw->GetRasterBand(raw->GetRasterCount());
  };
  job.rval = [raw](GDALRasterBand *r, GetFromPersistentFunc) { return RasterBand::New(r, raw); };
  job.run(info, async, 2);
}

/**
 * Returns the number of bands.
 *
 * @method count
 * @return {number}
 */

/**
 *
 * Returns the number of bands.
 * {{{async}}}
 *
 * @method countAsync
 *
 * @param {callback<number>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */
int DatasetBands::__count(GDALDataset *parent) {
  return parent->GetRasterCount();
}

/**
 * Parent dataset
 *
 * @readOnly
 * @attribute ds
 * @type {gdal.Dataset}
 */

} // namespace node_gdal
