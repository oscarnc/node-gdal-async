#include "compound_curves.hpp"
#include "../gdal_common.hpp"
#include "../geometry/gdal_geometry.hpp"
#include "../geometry/gdal_linearring.hpp"
#include "../geometry/gdal_simplecurve.hpp"
#include "../geometry/gdal_compoundcurve.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> CompoundCurveCurves::constructor;

void CompoundCurveCurves::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  StandaloneCollection<CompoundCurveCurves, OGRCurve *, OGRCompoundCurve *, Geometry, CompoundCurve>::Initialize(
    target);
  Local<FunctionTemplate> lcons = Nan::New(CompoundCurveCurves::constructor);

  Nan::SetPrototypeMethod(lcons, "add", add);

  Nan::Set(target, Nan::New(_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());
}

CompoundCurveCurves::CompoundCurveCurves()
  : StandaloneCollection<CompoundCurveCurves, OGRCurve *, OGRCompoundCurve *, Geometry, CompoundCurve>() {
}

CompoundCurveCurves::~CompoundCurveCurves() {
}

/**
 * A collection of connected curves, used by {{#crossLink
 * "gdal.CompoundCurve"}}gdal.CompoundCurve{{/crossLink}}.
 *
 * @class gdal.CompoundCurveCurves
 */

/**
 * Returns the number of curves that exist in the collection.
 *
 * @method count
 * @return {number}
 */

/**
 * Returns the number of curves that exist in the collection.
 * {{{async}}}
 *
 * @method countAsync
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */
int CompoundCurveCurves::__count(OGRCompoundCurve *parent) {
  return parent->getNumCurves();
}

/**
 * Returns the curve at the specified index.
 *
 * @example
 * ```
 * var curve0 = compound.curves.get(0);
 * var curve1 = compound.curves.get(1);```
 *
 * @method get
 * @param {number} index
 * @throws Error
 * @return {gdal.SimpleCurve}
 */

/**
 * Returns the curve at the specified index.
 * {{{async}}}
 *
 * @example
 * ```
 * var curve0 = compound.curves.get(0);
 * var curve1 = compound.curves.get(1);```
 *
 * @method get
 * @param {number} index
 * @throws Error
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.SimpleCurve>}
 */
OGRCurve *CompoundCurveCurves::__get(OGRCompoundCurve *parent, size_t idx) {
  if (idx >= static_cast<size_t>(__count(parent))) return nullptr;
  return parent->getCurve(idx);
}

OGRCurve *CompoundCurveCurves::__get(OGRCompoundCurve *parent, std::string name) {
  throw "index must be a number";
}

/**
 * Adds a curve to the collection.
 *
 * @example
 * ```
 * var ring1 = new gdal.CircularString();
 * ring1.points.add(0,0);
 * ring1.points.add(1,0);
 * ring1.points.add(1,1);
 * ring1.points.add(0,1);
 * ring1.points.add(0,0);
 *
 * // one at a time:
 * compound.curves.add(ring1);
 *
 * // many at once:
 * compound.curves.add([ring1, ...]);```
 *
 * @method add
 * @param {gdal.SimpleCurve|gdal.SimpleCurve[]} curves
 */
NAN_METHOD(CompoundCurveCurves::add) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  CompoundCurve *geom = Nan::ObjectWrap::Unwrap<CompoundCurve>(parent);

  SimpleCurve *ring;

  if (info.Length() < 1) {
    Nan::ThrowError("curve(s) must be given");
    return;
  }
  if (info[0]->IsArray()) {
    // set from array of geometry objects
    Local<Array> array = info[0].As<Array>();
    int length = array->Length();
    for (int i = 0; i < length; i++) {
      Local<Value> element = Nan::Get(array, i).ToLocalChecked();
      if (IS_WRAPPED(element, SimpleCurve)) {
        ring = Nan::ObjectWrap::Unwrap<SimpleCurve>(element.As<Object>());
        OGRErr err = geom->get()->addCurve(ring->get());
        if (err) {
          NODE_THROW_OGRERR(err);
          return;
        }
      } else {
        Nan::ThrowError("All array elements must be SimpleCurves");
        return;
      }
    }
  } else if (IS_WRAPPED(info[0], SimpleCurve)) {
    ring = Nan::ObjectWrap::Unwrap<SimpleCurve>(info[0].As<Object>());
    OGRErr err = geom->get()->addCurve(ring->get());
    if (err) {
      NODE_THROW_OGRERR(err);
      return;
    }
  } else {
    Nan::ThrowError("curve(s) must be a SimpleCurve or array of SimpleCurves");
    return;
  }

  return;
}

} // namespace node_gdal
