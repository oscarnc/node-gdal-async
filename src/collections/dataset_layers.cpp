#include <memory>
#include "dataset_layers.hpp"
#include "../gdal_common.hpp"
#include "../gdal_dataset.hpp"
#include "../gdal_layer.hpp"
#include "../gdal_spatial_reference.hpp"
#include "../utils/string_list.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> DatasetLayers::constructor;

void DatasetLayers::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  DatasetCollection<DatasetLayers, OGRLayer *, GDALDataset *, Layer, Dataset>::Initialize(target);
  Local<FunctionTemplate> lcons = Nan::New(constructor);

  Nan__SetPrototypeAsyncableMethod(lcons, "create", create);
  Nan__SetPrototypeAsyncableMethod(lcons, "copy", copy);
  Nan__SetPrototypeAsyncableMethod(lcons, "remove", remove);

  Nan::Set(target, Nan::New(_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());

  constructor.Reset(lcons);
}

DatasetLayers::DatasetLayers() : DatasetCollection<DatasetLayers, OGRLayer *, GDALDataset *, Layer, Dataset>() {
}

DatasetLayers::~DatasetLayers() {
}

/**
 * An encapsulation of a {{#crossLink "gdal.Dataset"}}Dataset{{/crossLink}}'s
 * vector layers.
 *
 * ```
 * var layers = dataset.layers;```
 *
 * @class gdal.DatasetLayers
 */

/**
 * Returns the layer with the given name or identifier.
 *
 * @method get
 * @param {string|number} key Layer name or ID.
 * @throws Error
 * @return {gdal.Layer}
 */

/**
 * Returns the layer with the given name or identifier.
 * {{{async}}}
 *
 * @method getAsync
 * @param {string|number} key Layer name or ID.
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @throws Error
 * @return {Promise<gdal.Layer>}
 */
OGRLayer *DatasetLayers::__get(GDALDataset *parent, size_t idx) {
  return parent->GetLayer(idx);
}

OGRLayer *DatasetLayers::__get(GDALDataset *parent, std::string const &name) {
  return parent->GetLayerByName(name.c_str());
}

/**
 * Adds a new layer.
 *
 * @example
 * ```
 * dataset.layers.create('layername', null, gdal.Point);
 * ```
 *
 * @method create
 * @throws Error
 * @param {string} name Layer name
 * @param {gdal.SpatialReference|null} srs Layer projection
 * @param {number|Function} geomType Geometry type or constructor ({{#crossLink
 * "Constants (wkbGeometryType)"}}see geometry types{{/crossLink}})
 * @param {string[]|object} [creation_options] driver-specific layer creation
 * options
 * @return {gdal.Layer}
 */

/**
 * Adds a new layer.
 * {{{async}}}
 *
 * @example
 * ```
 * await dataset.layers.createAsync('layername', null, gdal.Point);
 * dataset.layers.createAsync('layername', null, gdal.Point, (e, r) => console.log(e, r));
 * ```
 *
 * @method createAsync
 * @throws Error
 * @param {string} name Layer name
 * @param {gdal.SpatialReference|null} srs Layer projection
 * @param {number|Function} geomType Geometry type or constructor ({{#crossLink
 * "Constants (wkbGeometryType)"}}see geometry types{{/crossLink}})
 * @param {string[]|object} [creation_options] driver-specific layer creation
 * options
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.Layer>}
 */

GDAL_ASYNCABLE_DEFINE(DatasetLayers::create) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

  GDALDataset *raw = ds->get();

  std::string *layer_name = new std::string;
  SpatialReference *spatial_ref = NULL;
  OGRwkbGeometryType geom_type = wkbUnknown;
  StringList *options = new StringList;

  NODE_ARG_STR(0, "layer name", *layer_name);
  NODE_ARG_WRAPPED_OPT(1, "spatial reference", SpatialReference, spatial_ref);
  NODE_ARG_ENUM_OPT(2, "geometry type", OGRwkbGeometryType, geom_type);
  if (info.Length() > 3 && options->parse(info[3])) {
    return; // error parsing string list
  }

  OGRSpatialReference *srs = NULL;
  if (spatial_ref) srs = spatial_ref->get();

  GDALAsyncableJob<OGRLayer *> job(ds->uid);
  job.persist(parent);
  job.main = [raw, layer_name, srs, geom_type, options](const GDALExecutionProgress &) {
    std::unique_ptr<StringList> options_ptr(options);
    std::unique_ptr<std::string> layer_name_ptr(layer_name);
    CPLErrorReset();
    OGRLayer *layer = raw->CreateLayer(layer_name->c_str(), srs, geom_type, options->get());
    if (layer == nullptr) throw CPLGetLastErrorMsg();
    return layer;
  };

  job.rval = [raw](OGRLayer *layer, GetFromPersistentFunc) { return Layer::New(layer, raw, false); };

  job.run(info, async, 4);
}

/**
 * Returns the number of layers.
 *
 * @method count
 * @return {number}
 */

/**
 * Returns the number of layers.
 * {{{async}}}
 *
 * @method countAsync
 * @param {callback<number>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */

int DatasetLayers::__count(GDALDataset *parent) {
  return parent->GetLayerCount();
}

/**
 * Copies a layer.
 *
 * @method copy
 * @param {string} src_lyr_name
 * @param {string} dst_lyr_name
 * @param {object|string[]} [options=null] layer creation options
 * @return {gdal.Layer}
 */

/**
 * Copies a layer.
 * {{{async}}}
 *
 * @method copyAsync
 * @param {string} src_lyr_name
 * @param {string} dst_lyr_name
 * @param {object|string[]} [options=null] layer creation options
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.Layer>}
 */

GDAL_ASYNCABLE_DEFINE(DatasetLayers::copy) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

  GDALDataset *raw = ds->get();

  Layer *layer_to_copy;
  std::string *new_name = new std::string("");
  StringList *options = new StringList;

  NODE_ARG_WRAPPED(0, "layer to copy", Layer, layer_to_copy);
  NODE_ARG_STR(1, "new layer name", *new_name);
  if (info.Length() > 2 && options->parse(info[2])) { Nan::ThrowError("Error parsing string list"); }

  OGRLayer *src = layer_to_copy->get();
  GDALAsyncableJob<OGRLayer *> job(ds->uid);
  job.persist(parent, info[0].As<Object>());
  job.main = [raw, src, new_name, options](const GDALExecutionProgress &) {
    std::unique_ptr<StringList> options_ptr(options);
    std::unique_ptr<std::string> new_name_ptr(new_name);
    CPLErrorReset();
    OGRLayer *layer = raw->CopyLayer(src, new_name->c_str(), options->get());
    if (layer == nullptr) throw CPLGetLastErrorMsg();
    return layer;
  };

  job.rval = [raw](OGRLayer *layer, GetFromPersistentFunc) { return Layer::New(layer, raw); };

  job.run(info, async, 3);
}

/**
 * Removes a layer.
 *
 * @method remove
 * @throws Error
 * @param {number} index
 */

/**
 * Removes a layer.
 * {{{async}}}
 *
 * @method removeAsync
 * @throws Error
 * @param {number} index
 * @param {callback<void>} [callback=undefined] {{{cb}}}
 * @return {Promise<void>}
 */

GDAL_ASYNCABLE_DEFINE(DatasetLayers::remove) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

  GDALDataset *raw = ds->get();

  int i;
  NODE_ARG_INT(0, "layer index", i);
  GDALAsyncableJob<OGRErr> job(ds->uid);
  job.persist(parent);
  job.main = [raw, i](const GDALExecutionProgress &) {
    OGRErr err = raw->DeleteLayer(i);
    if (err) throw getOGRErrMsg(err);
    return err;
  };

  job.rval = [](int count, GetFromPersistentFunc) { return Nan::Undefined().As<Value>(); };
  job.run(info, async, 1);
}

/**
 * Parent dataset
 *
 * @readOnly
 * @attribute ds
 * @type {gdal.Dataset}
 */

} // namespace node_gdal
