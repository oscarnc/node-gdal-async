#ifndef __NODE_GDAL_GROUP_COLLECTION_H__
#define __NODE_GDAL_GROUP_COLLECTION_H__

// node
#include <node.h>
#include <node_object_wrap.h>

// nan
#include "../nan-wrapper.h"

// gdal
#include <gdal_priv.h>

#include "dataset_collection.hpp"
#include "../async.hpp"
#include "../gdal_dataset.hpp"
#include "../gdal_group.hpp"

#if GDAL_VERSION_MAJOR > 3 || (GDAL_VERSION_MAJOR == 3 && GDAL_VERSION_MINOR >= 1)

using namespace v8;
using namespace node;

namespace node_gdal {

template <typename SELF, typename GDALOBJ, typename GDALPARENT, typename NODEOBJ, typename NODEPARENT>
class GroupCollection
  : public DatasetCollection<SELF, std::shared_ptr<GDALOBJ>, std::shared_ptr<GDALPARENT>, NODEOBJ, NODEPARENT> {
    public:
  static constexpr const char *_className = "GroupCollection<abstract>";
  static void Initialize(Local<Object> target) {
    Nan::HandleScope scope;
    DatasetCollection<SELF, std::shared_ptr<GDALOBJ>, std::shared_ptr<GDALPARENT>, NODEOBJ, NODEPARENT>::Initialize(
      target);
    Local<FunctionTemplate> lcons = Nan::New(SELF::constructor);

    ATTR(lcons, "names", namesGetter, READ_ONLY_SETTER);

    Nan::Set(target, Nan::New(SELF::_className).ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());
  }

  static NAN_GETTER(namesGetter) {
    Nan::HandleScope scope;

    Local<Object> parent_ds =
      Nan::GetPrivate(info.This(), Nan::New("parent_ds_").ToLocalChecked()).ToLocalChecked().As<Object>();
    Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent_ds);

    Local<Object> parent_obj =
      Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
    NODEPARENT *parent = Nan::ObjectWrap::Unwrap<NODEPARENT>(parent_obj);

    if (!ds->isAlive()) {
      Nan::ThrowError("Dataset object has already been destroyed");
      return;
    }

    std::vector<std::string> names = SELF::__getNames(parent->get());

    Local<Array> results = Nan::New<Array>(0);
    int i = 0;
    for (std::string &n : names) { Nan::Set(results, i++, SafeString::New(n.c_str())); }

    info.GetReturnValue().Set(results);
  }

  GroupCollection()
    : DatasetCollection<SELF, std::shared_ptr<GDALOBJ>, std::shared_ptr<GDALPARENT>, NODEOBJ, NODEPARENT>() {
  }

    protected:
  ~GroupCollection() {
  }
};

} // namespace node_gdal
#endif
#endif
