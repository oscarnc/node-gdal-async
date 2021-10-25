#include "layer_features.hpp"
#include "../gdal_common.hpp"
#include "../gdal_feature.hpp"
#include "../gdal_layer.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> LayerFeatures::constructor;

void LayerFeatures::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> lcons = Nan::New<FunctionTemplate>(New);
  lcons->InstanceTemplate()->SetInternalFieldCount(1);
  lcons->SetClassName(Nan::New(_className).ToLocalChecked());

  Nan::SetPrototypeMethod(lcons, "toString", toString);
  Nan__SetPrototypeAsyncableMethod(lcons, "count", count);
  Nan__SetPrototypeAsyncableMethod(lcons, "add", add);
  Nan__SetPrototypeAsyncableMethod(lcons, "get", get);
  Nan__SetPrototypeAsyncableMethod(lcons, "set", set);
  Nan__SetPrototypeAsyncableMethod(lcons, "first", first);
  Nan__SetPrototypeAsyncableMethod(lcons, "next", next);
  Nan__SetPrototypeAsyncableMethod(lcons, "remove", remove);

  ATTR_DONT_ENUM(lcons, "parent", parentGetter, READ_ONLY_SETTER);
  ATTR_DONT_ENUM(lcons, "layer", parentGetter, READ_ONLY_SETTER);

  Nan::Set(target, Nan::New(_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());

  constructor.Reset(lcons);
}

LayerFeatures::LayerFeatures() : DatasetCollection<LayerFeatures, OGRFeature *, OGRLayer *, Feature, Layer>() {
}

LayerFeatures::~LayerFeatures() {
}

/**
 * An encapsulation of a {{#crossLink "gdal.Layer"}}Layer{{/crossLink}}'s
 * features.
 *
 * @class gdal.LayerFeatures
 */

/**
 * Fetch a feature by its identifier.
 *
 * **Important:** The `id` argument is not an index. In most cases it will be
 * zero-based, but in some cases it will not. If iterating, it's best to use the
 * `next()` method.
 *
 * @method get
 * @param {number} id The feature ID of the feature to read.
 * @throws Error
 * @return {gdal.Feature}
 */

/**
 * Fetch a feature by its identifier.
 *
 * **Important:** The `id` argument is not an index. In most cases it will be
 * zero-based, but in some cases it will not. If iterating, it's best to use the
 * `next()` method.
 * {{{async}}}
 *
 * @method getAsync
 * @param {number} id The feature ID of the feature to read.
 * @param {callback<gdal.Feature>} [callback=undefined] {{{cb}}}
 * @throws Error
 * @return {Promise<gdal.Feature>}
 */
GDAL_ASYNCABLE_DEFINE(LayerFeatures::get) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  int feature_id;
  NODE_ARG_INT(0, "feature id", feature_id);
  OGRLayer *gdal_layer = layer->get();
  GDALAsyncableJob<OGRFeature *> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer, feature_id](const GDALExecutionProgress &) {
    CPLErrorReset();
    OGRFeature *feature = gdal_layer->GetFeature(feature_id);
    if (feature == nullptr) throw CPLGetLastErrorMsg();
    return feature;
  };
  job.rval = [](OGRFeature *feature, GetFromPersistentFunc) { return Feature::New(feature); };
  job.run(info, async, 1);
}

/**
 * Resets the feature pointer used by `next()` and
 * returns the first feature in the layer.
 *
 * @method first
 * @return {gdal.Feature}
 */

/**
 * Resets the feature pointer used by `next()` and
 * returns the first feature in the layer.
 * {{{async}}}
 *
 * @method firstAsync
 * @param {callback<gdal.Feature>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.Feature>}
 */
GDAL_ASYNCABLE_DEFINE(LayerFeatures::first) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  OGRLayer *gdal_layer = layer->get();
  GDALAsyncableJob<OGRFeature *> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer](const GDALExecutionProgress &) {
    gdal_layer->ResetReading();
    OGRFeature *feature = gdal_layer->GetNextFeature();
    return feature;
  };
  job.rval = [](OGRFeature *feature, GetFromPersistentFunc) { return Feature::New(feature); };
  job.run(info, async, 0);
}

/**
 * Returns the next feature in the layer. Returns null if no more features.
 *
 * @example
 * ```
 * while (feature = layer.features.next()) { ... }```
 *
 * @method next
 * @return {gdal.Feature}
 */

/**
 * Returns the next feature in the layer. Returns null if no more features.
 * {{{async}}}
 *
 * @example
 * ```
 * while (feature = await layer.features.nextAsync()) { ... }```
 *
 * @method nextAsync
 * @param {callback<gdal.Feature>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.Feature>}
 */
GDAL_ASYNCABLE_DEFINE(LayerFeatures::next) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  OGRLayer *gdal_layer = layer->get();
  GDALAsyncableJob<OGRFeature *> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer](const GDALExecutionProgress &) {
    OGRFeature *feature = gdal_layer->GetNextFeature();
    return feature;
  };
  job.rval = [](OGRFeature *feature, GetFromPersistentFunc) { return Feature::New(feature); };
  job.run(info, async, 0);
}

/**
 * Adds a feature to the layer. The feature should be created using the current
 * layer as the definition.
 *
 * @example
 * ```
 * var feature = new gdal.Feature(layer);
 * feature.setGeometry(new gdal.Point(0, 1));
 * feature.fields.set('name', 'somestring');
 * layer.features.add(feature);```
 *
 * @method add
 * @throws Error
 * @param {gdal.Feature} feature
 */

/**
 * Adds a feature to the layer. The feature should be created using the current
 * layer as the definition.
 * {{{async}}}
 *
 * @example
 * ```
 * var feature = new gdal.Feature(layer);
 * feature.setGeometry(new gdal.Point(0, 1));
 * feature.fields.set('name', 'somestring');
 * await layer.features.addAsync(feature);```
 *
 * @method addAsync
 * @throws Error
 * @param {gdal.Feature} feature
 * @param {callback<void>} [callback=undefined] {{{cb}}}
 * @return {Promise<void>}
 */

GDAL_ASYNCABLE_DEFINE(LayerFeatures::add) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  Feature *f;
  NODE_ARG_WRAPPED(0, "feature", Feature, f)

  OGRLayer *gdal_layer = layer->get();
  OGRFeature *gdal_f = f->get();
  GDALAsyncableJob<int> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer, gdal_f](const GDALExecutionProgress &) {
    int err = gdal_layer->CreateFeature(gdal_f);
    if (err != CE_None) throw getOGRErrMsg(err);
    return err;
  };
  job.rval = [](int, GetFromPersistentFunc) { return Nan::Undefined(); };
  job.run(info, async, 1);
}

/**
 * Returns the number of features in the layer.
 *
 * @method count
 * @param {boolean} [force=true]
 * @return {number} number of features in the layer.
 */

/**
 * Returns the number of features in the layer.
 * {{{async}}}
 *
 * @method countAsync
 * @param {boolean} [force=true]
 * @param {callback<number>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>} number of features in the layer.
 */

GDAL_ASYNCABLE_DEFINE(LayerFeatures::count) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  Local<Object> ds;
  if (object_store.has(layer->getParent())) {
    ds = object_store.get(layer->getParent());
  } else {
    Nan::ThrowError("Dataset object already destroyed");
    return;
  }

  int force = 1;
  NODE_ARG_BOOL_OPT(0, "force", force);

  OGRLayer *gdal_layer = layer->get();
  GDALAsyncableJob<GIntBig> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer, force](const GDALExecutionProgress &) {
    GIntBig count = gdal_layer->GetFeatureCount(force);
    return count;
  };
  job.rval = [](GIntBig count, GetFromPersistentFunc) { return Nan::New<Number>(count); };
  job.run(info, async, 1);
}

/**
 * Sets a feature in the layer.
 *
 * @method set
 * @throws Error
 * @param {gdal.Feature} feature
 */

/**
 * Sets a feature in the layer.
 *
 * @method set
 * @throws Error
 * @param {number} id
 * @param {gdal.Feature} feature
 */

/**
 * Sets a feature in the layer.
 * {{{async}}}
 *
 * @method setAsync
 * @throws Error
 * @param {number} id
 * @param {gdal.Feature} feature
 * @param {callback<gdal.Feature>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.Feature>}
 */
GDAL_ASYNCABLE_DEFINE(LayerFeatures::set) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  Local<Object> ds;
  if (object_store.has(layer->getParent())) { ds = object_store.get(layer->getParent()); }
  if (!layer->isAlive()) {
    Nan::ThrowError("Dataset object already destroyed");
    return;
  }

  int err;
  Feature *f;

  Local<Object> feature;
  if (info[0]->IsObject()) {
    NODE_ARG_WRAPPED(0, "feature", Feature, f);
    feature = info[0].As<Object>();
  } else if (info[0]->IsNumber()) {
    int i = 0;
    NODE_ARG_INT(0, "feature id", i);
    NODE_ARG_WRAPPED(1, "feature", Feature, f);
    feature = info[1].As<Object>();
    err = f->get()->SetFID(i);
    if (err) {
      Nan::ThrowError("Error setting feature id");
      return;
    }
  } else {
    Nan::ThrowError("Invalid arguments");
    return;
  }

  if (!f->isAlive()) {
    Nan::ThrowError("Feature already destroyed");
    return;
  }

  OGRLayer *gdal_layer = layer->get();
  OGRFeature *gdal_feature = f->get();
  GDALAsyncableJob<OGRErr> job(layer->parent_uid);
  job.persist(layer->handle(), f->handle());
  job.main = [gdal_layer, gdal_feature](const GDALExecutionProgress &) {
    OGRErr err = gdal_layer->SetFeature(gdal_feature);
    if (err != CE_None) throw getOGRErrMsg(err);
    return err;
  };

  job.rval = [](int, GetFromPersistentFunc) { return Nan::Undefined(); };
  job.run(info, async, 2);
}

/**
 * Removes a feature from the layer.
 *
 * @method remove
 * @throws Error
 * @param {number} id
 */

/**
 * Removes a feature from the layer.
 *
 * @method removeAsync
 * @throws Error
 * @param {number} id
 * @param {callback<void>} [callback=undefined] {{{cb}}}
 * @return {Promise<void>}
 */

GDAL_ASYNCABLE_DEFINE(LayerFeatures::remove) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Layer *layer = Nan::ObjectWrap::Unwrap<Layer>(parent);
  if (!layer->isAlive()) {
    Nan::ThrowError("Layer object already destroyed");
    return;
  }

  int i;
  NODE_ARG_INT(0, "feature id", i);

  OGRLayer *gdal_layer = layer->get();
  GDALAsyncableJob<int> job(layer->parent_uid);
  job.persist(layer->handle());
  job.main = [gdal_layer, i](const GDALExecutionProgress &) {
    int err = gdal_layer->DeleteFeature(i);
    if (err) { throw getOGRErrMsg(err); }
    return err;
  };
  job.rval = [](int, GetFromPersistentFunc) { return Nan::Undefined(); };
  job.run(info, async, 1);

  return;
}

/**
 * Parent layer
 *
 * @attribute layer
 * @type {gdal.Layer}
 */

} // namespace node_gdal
