/* Specialized implementation of Bigarray for float64. */

type kind('a) =
  | Float64: kind(float)
  | Complex64: kind(Complex.t);
type layout =
  | C_layout;

let float64 = Float64;
let complex64 = Complex64;
let c_layout = C_layout;

module Array1 = {
  type t('a);
  [@bs.get] external dim : t('a) => int = "length";

  [@bs.new]
  external createFloat64Array : (int) => t(float) =
    "Float64Array";

  [@bs.new]
  external createComplexArray : int => t(Complex.t) = "Array";

  let create = (type a, kind: kind(a), _layout, size) : t(a) => {
    switch (kind) {
    | Float64 => createFloat64Array(size)
    | Complex64 => createComplexArray(size)
    }
  };

  [@bs.get_index] external get : (t('a), int) => 'a = "";
  [@bs.get_index] external unsafe_get : (t('a), int) => 'a = "";
  [@bs.set_index] external set : (t('a), int, 'a) => unit = "";
  [@bs.set_index] external unsafe_set : (t('a), int, 'a) => unit = "";

  let blit = (a1, a2) =>
    for (i in 0 to dim(a1)) {
      set(a2, i, get(a1, i));
    };
};
