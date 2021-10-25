#include "linestring_points.hpp"
#include "../gdal_common.hpp"
#include "../geometry/gdal_geometry.hpp"
#include "../geometry/gdal_linestring.hpp"
#include "../geometry/gdal_point.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> LineStringPoints::constructor;

void LineStringPoints::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> lcons = Nan::New<FunctionTemplate>(LineStringPoints::New);
  lcons->InstanceTemplate()->SetInternalFieldCount(1);
  lcons->SetClassName(Nan::New("LineStringPoints").ToLocalChecked());

  Nan::SetPrototypeMethod(lcons, "toString", toString);
  Nan::SetPrototypeMethod(lcons, "count", count);
  Nan::SetPrototypeMethod(lcons, "get", get);
  Nan::SetPrototypeMethod(lcons, "set", set);
  Nan::SetPrototypeMethod(lcons, "add", add);
  Nan::SetPrototypeMethod(lcons, "reverse", reverse);
  Nan::SetPrototypeMethod(lcons, "resize", resize);

  Nan::Set(target, Nan::New("LineStringPoints").ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());

  constructor.Reset(lcons);
}

LineStringPoints::LineStringPoints()
  : StandaloneCollection<LineStringPoints, OGRPoint *, OGRLineString *, Geometry, LineString>() {
}

LineStringPoints::~LineStringPoints() {
}

/**
 * An encapsulation of a {{#crossLink
 * "gdal.LineString"}}LineString{{/crossLink}}'s points.
 *
 * @class gdal.LineStringPoints
 */

/**
 * Returns the number of points that are part of the line string.
 *
 * @method count
 * @return {number}
 */

/**
 * Returns the number of points that are part of the line string.
 * {{{async}}}
 *
 * @method countAsync
 * @param {callback<gdal.Layer>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */

int LineStringPoints::__count(OGRLineString *parent) {
  return parent->getNumPoints();
}

/**
 * Reverses the order of all the points.
 *
 * @method reverse
 */
NAN_METHOD(LineStringPoints::reverse) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  LineString *geom = Nan::ObjectWrap::Unwrap<LineString>(parent);

  geom->get()->reversePoints();

  return;
}

/**
 * Adjusts the number of points that make up the line string.
 *
 * @method resize
 * @param {number} count
 */
NAN_METHOD(LineStringPoints::resize) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  LineString *geom = Nan::ObjectWrap::Unwrap<LineString>(parent);

  int count;
  NODE_ARG_INT(0, "point count", count)
  geom->get()->setNumPoints(count);

  return;
}

/**
 * Returns the point at the specified index.
 *
 * @method get
 * @param {number} index 0-based index
 * @throws Error
 * @return {gdal.Point}
 */
NAN_METHOD(LineStringPoints::get) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  LineString *geom = Nan::ObjectWrap::Unwrap<LineString>(parent);

  OGRPoint pt;
  int i;

  NODE_ARG_INT(0, "index", i);
  if (i < 0 || i >= geom->get()->getNumPoints()) {
    Nan::ThrowError("Invalid point requested");
    return;
  }

  geom->get()->getPoint(i, &pt);

  // New will copy the point with GDAL clone()
  info.GetReturnValue().Set(Point::New(&pt, false));
}

/**
 * Sets the point at the specified index.
 *
 * @example
 * ```
 * lineString.points.set(0, new gdal.Point(1, 2));```
 *
 * @method set
 * @throws Error
 * @param {number} index 0-based index
 * @param {gdal.Point|xyz} point
 */

/**
 * @method set
 * @throws Error
 * @param {number} index 0-based index
 * @param {number} x
 * @param {number} y
 * @param {number} [z]
 */
NAN_METHOD(LineStringPoints::set) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  LineString *geom = Nan::ObjectWrap::Unwrap<LineString>(parent);

  int i;
  NODE_ARG_INT(0, "index", i);
  if (i < 0 || i >= geom->get()->getNumPoints()) {
    Nan::ThrowError("Point index out of range");
    return;
  }

  int n = info.Length() - 1;

  if (n == 0) {
    Nan::ThrowError("Point must be given");
    return;
  } else if (n == 1) {
    if (!info[1]->IsObject()) {
      Nan::ThrowError("Point or object expected for second argument");
      return;
    }
    if (IS_WRAPPED(info[1], Point)) {
      // set from Point object
      Point *pt = Nan::ObjectWrap::Unwrap<Point>(info[1].As<Object>());
      geom->get()->setPoint(i, pt->get());
    } else {
      Local<Object> obj = info[1].As<Object>();
      // set from object {x: 0, y: 5}
      double x, y;
      NODE_DOUBLE_FROM_OBJ(obj, "x", x);
      NODE_DOUBLE_FROM_OBJ(obj, "y", y);

      Local<String> z_prop_name = Nan::New("z").ToLocalChecked();
      if (Nan::HasOwnProperty(obj, z_prop_name).FromMaybe(false)) {
        Local<Value> z_val = Nan::Get(obj, z_prop_name).ToLocalChecked();
        if (!z_val->IsNumber()) {
          Nan::ThrowError("z property must be number");
          return;
        }
        geom->get()->setPoint(i, x, y, Nan::To<double>(z_val).ToChecked());
      } else {
        geom->get()->setPoint(i, x, y);
      }
    }
  } else {
    // set x, y, z from numeric arguments
    if (!info[1]->IsNumber()) {
      Nan::ThrowError("Number expected for second argument");
      return;
    }
    if (!info[2]->IsNumber()) {
      Nan::ThrowError("Number expected for third argument");
      return;
    }
    if (n == 2) {
      geom->get()->setPoint(i, Nan::To<double>(info[1]).ToChecked(), Nan::To<double>(info[2]).ToChecked());
    } else {
      if (!info[3]->IsNumber()) {
        Nan::ThrowError("Number expected for fourth argument");
        return;
      }

      geom->get()->setPoint(
        i,
        Nan::To<double>(info[1]).ToChecked(),
        Nan::To<double>(info[2]).ToChecked(),
        Nan::To<double>(info[3]).ToChecked());
    }
  }

  return;
}

/**
 * Adds point(s) to the line string. Also accepts any object with an x and y
 * property.
 *
 * @example
 * ```
 * lineString.points.add(new gdal.Point(1, 2));
 * lineString.points.add([
 *     new gdal.Point(1, 2)
 *     new gdal.Point(3, 4)
 * ]);```
 *
 * @method add
 * @throws Error
 * @param {gdal.Point|xyz|(gdal.Point|xyz)[]} points
 */

/**
 *
 * @method add
 * @throws Error
 * @param {number} x
 * @param {number} y
 * @param {number} [z]
 */
NAN_METHOD(LineStringPoints::add) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  LineString *geom = Nan::ObjectWrap::Unwrap<LineString>(parent);

  int n = info.Length();

  if (n == 0) {
    Nan::ThrowError("Point must be given");
    return;
  } else if (n == 1) {
    if (!info[0]->IsObject()) {
      Nan::ThrowError("Point, object, or array of points expected");
      return;
    }
    if (IS_WRAPPED(info[0], Point)) {
      // set from Point object
      Point *pt = Nan::ObjectWrap::Unwrap<Point>(info[0].As<Object>());
      geom->get()->addPoint(pt->get());
    } else if (info[0]->IsArray()) {
      // set from array of points
      Local<Array> array = info[0].As<Array>();
      int length = array->Length();
      for (int i = 0; i < length; i++) {
        Local<Value> element = Nan::Get(array, i).ToLocalChecked();
        if (!element->IsObject()) {
          Nan::ThrowError("All points must be Point objects or objects");
          return;
        }
        Local<Object> element_obj = element.As<Object>();
        if (IS_WRAPPED(element_obj, Point)) {
          // set from Point object
          Point *pt = Nan::ObjectWrap::Unwrap<Point>(element_obj);
          geom->get()->addPoint(pt->get());
        } else {
          // set from object {x: 0, y: 5}
          double x, y;
          NODE_DOUBLE_FROM_OBJ(element_obj, "x", x);
          NODE_DOUBLE_FROM_OBJ(element_obj, "y", y);

          Local<String> z_prop_name = Nan::New("z").ToLocalChecked();
          if (Nan::HasOwnProperty(element_obj, z_prop_name).FromMaybe(false)) {
            Local<Value> z_val = Nan::Get(element_obj, z_prop_name).ToLocalChecked();
            if (!z_val->IsNumber()) {
              Nan::ThrowError("z property must be number");
              return;
            }
            geom->get()->addPoint(x, y, Nan::To<double>(z_val).ToChecked());
          } else {
            geom->get()->addPoint(x, y);
          }
        }
      }
    } else {
      // set from object {x: 0, y: 5}
      Local<Object> obj = info[0].As<Object>();
      double x, y;
      NODE_DOUBLE_FROM_OBJ(obj, "x", x);
      NODE_DOUBLE_FROM_OBJ(obj, "y", y);

      Local<String> z_prop_name = Nan::New("z").ToLocalChecked();
      if (Nan::HasOwnProperty(obj, z_prop_name).FromMaybe(false)) {
        Local<Value> z_val = Nan::Get(obj, z_prop_name).ToLocalChecked();
        if (!z_val->IsNumber()) {
          Nan::ThrowError("z property must be number");
          return;
        }
        geom->get()->addPoint(x, y, Nan::To<double>(z_val).ToChecked());
      } else {
        geom->get()->addPoint(x, y);
      }
    }
  } else {
    // set x, y, z from numeric arguments
    if (!info[0]->IsNumber()) {
      Nan::ThrowError("Number expected for first argument");
      return;
    }
    if (!info[1]->IsNumber()) {
      Nan::ThrowError("Number expected for second argument");
      return;
    }
    if (n == 2) {
      geom->get()->addPoint(Nan::To<double>(info[0]).ToChecked(), Nan::To<double>(info[1]).ToChecked());
    } else {
      if (!info[2]->IsNumber()) {
        Nan::ThrowError("Number expected for third argument");
        return;
      }

      geom->get()->addPoint(
        Nan::To<double>(info[0]).ToChecked(),
        Nan::To<double>(info[1]).ToChecked(),
        Nan::To<double>(info[2]).ToChecked());
    }
  }

  return;
}

} // namespace node_gdal
