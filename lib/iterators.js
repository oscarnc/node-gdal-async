module.exports = function (gdal) {

  /**
 * forEach() callback type
 * @element forEachCb
 * @typedef forEachCb<T> (obj: T, idx: number) => boolean|void
 */

  /**
 * Iterates through all bands using a callback function.
 * Note: GDAL band indexes start at 1, not 0.
 *
 * @example
 * ```
 * dataset.bands.forEach(function(band, i) { ... });```
 *
 * @for gdal.DatasetBands
 * @method forEach
 * @param {forEachCb<gdal.RasterBand>} callback The callback to be called with each {{#crossLink "RasterBand"}}RasterBand{{/crossLink}}
 */
  gdal.DatasetBands.prototype.forEach = function (callback) {
    const n = this.count()
    for (let i = 1; i <= n; i++) {
      if (callback(this.get(i), i) === false) return
    }
  }

  /**
 * Iterates through all bands using an iterator
 *
 * @example
 * ```
 * for (const band of dataset.bands) {
 * }```
 *
 * @for gdal.DatasetBands
 * @type {gdal.RasterBand}
 * @method Symbol.iterator
 */
  gdal.DatasetBands.prototype[Symbol.iterator] = function () {
    let i = 1

    return {
      next: () => {
        const done = !(i <= this.count())
        return {
          done,
          value: done ? null : this.get(i++)
        }
      }
    }
  }

  /**
 * Iterates through all bands using an async iterator
 *
 * @example
 * ```
 * for await (const band of dataset.bands) {
 * }```
 *
 * @for gdal.DatasetBands
 * @type {gdal.RasterBand}
 * @method Symbol.asyncIterator
 */
  if (Symbol.asyncIterator) {
    gdal.DatasetBands.prototype[Symbol.asyncIterator] = function () {
      let i = 1

      return {
        next: () => this.countAsync()
          .then((count) => {
            if (i <= count) {
              return this.getAsync(i++).then((value) => ({ done: false, value }))
            }
            return { done: true, value: null }
          })
      }
    }
  }

  /**
 * Iterates through all features using a callback function.
 *
 * @example
 * ```
 * layer.features.forEach(function(feature, i) { ... });```
 *
 * @for gdal.LayerFeatures
 * @method forEach
 * @param {forEachCb<gdal.Feature>} callback The callback to be called with each {{#crossLink "Feature"}}Feature{{/crossLink}}
 */
  gdal.LayerFeatures.prototype.forEach = function (callback) {
    let i = 0
    let feature = this.first()
    while (feature) {
      if (callback(feature, i++) === false) return
      feature = this.next()
    }
  }

  /**
 * Iterates through all features using an iterator
 *
 * @example
 * ```
 * for (const feature of layer.features) {
 * }```
 *
 * @for gdal.LayerFeatures
 * @type {gdal.Feature}
 * @method Symbol.iterator
 */
  gdal.LayerFeatures.prototype[Symbol.iterator] = function () {
    let feature

    return {
      next: () => {
        feature = feature ? this.next() : this.first()
        return {
          done: !feature,
          value: feature
        }
      }
    }
  }

  /**
 * Iterates through all features using an async iterator
 *
 * @example
 * ```
 * for await (const feature of layer.features) {
 * }```
 *
 * @for gdal.LayerFeatures
 * @type {gdal.Feature}
 * @method Symbol.asyncIterator
 */
  if (Symbol.asyncIterator) {
    gdal.LayerFeatures.prototype[Symbol.asyncIterator] = function () {
      let feature

      return {
        next: () => {
          feature = feature ? this.nextAsync() : this.firstAsync()
          return feature.then((value) => ({
            done: !value,
            value
          }))
        }
      }
    }
  }

  /**
 * Iterates through all fields using a callback function.
 *
 * @example
 * ```
 * layer.features.get(0).fields.forEach(function(value, key) { ... });```
 *
 * @for gdal.FeatureFields
 * @method forEach
 * @param {forEachCb<any>} callback The callback to be called with each feature `value` and `key`.
 */
  gdal.FeatureFields.prototype.forEach = function (callback) {
    const obj = this.toObject()
    Object.entries(obj).every(([ k, v ]) =>
      callback(v, k) !== false
    )
  }

  /**
 * Outputs the fields as a serialized JSON string.
 *
 * @for gdal.FeatureFields
 * @method toJSON
 * @return {string} Serialized JSON
 */
  gdal.FeatureFields.prototype.toJSON = function () {
    return JSON.stringify(this.toObject())
  }

  /**
 * Converts the geometry to a GeoJSON object representation.
 *
 * @for gdal.Geometry
 * @method toObject
 * @return {object} GeoJSON
 */
  gdal.Geometry.prototype.toObject = function () {
    return JSON.parse(this.toJSON())
  }

  require('./default_iterators.js')(gdal)
}
