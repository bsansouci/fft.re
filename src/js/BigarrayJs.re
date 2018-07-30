/* Specialized implementation of Bigarray for float64. */
type kind =
  | Float64;
type layout =
  | C_layout;

let float64 = Float64;
let c_layout = C_layout;

module Array1 = {
  type t('a);
  [@bs.get] external dim : t('a) => int = "length";

  [@bs.new]
  external create : ([@bs.ignore] kind, [@bs.ignore] layout, int) => t(float) =
    "Float64Array";

  [@bs.get_index] external get : (t(float), int) => float = "";

  [@bs.set_index] external set : (t(float), int, float) => unit = "";

  let blit = (a1, a2) =>
    for (i in 0 to dim(a1)) {
      set(a2, i, get(a1, i));
    };
};
