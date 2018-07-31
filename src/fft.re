module BA = MyBigarray.Array1;

let pi = 4.0 *. atan(1.0);
let two_pi = pi *. 2.;

let rec fftInplace = complex => {
  let m = BA.dim(complex) / 2;
  let evenElements = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);
  let oddElements = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);

  for (k in 0 to m - 1) {
    BA.unsafe_set(evenElements, k, BA.unsafe_get(complex, 2 * k));
    BA.unsafe_set(oddElements, k, BA.unsafe_get(complex, 2 * k + 1));
  };

  if (m > 1) {
    fftInplace(evenElements);
    fftInplace(oddElements);
  };

  for (k in 0 to m - 1) {
    let a = -. pi *. float_of_int(k) /. float_of_int(m);
    let x = cos(a);
    let y = sin(a);
    let com = BA.unsafe_get(oddElements, k);
    let new_com =
      Complex.{
        re: x *. com.re -. y *. com.im,
        im: x *. com.im +. y *. com.re,
      };
    let c = BA.unsafe_get(evenElements, k);
    BA.unsafe_set(complex, k, Complex.add(c, new_com));
    BA.unsafe_set(complex, k + m, Complex.sub(c, new_com));
  };
};

let fft = (~data, ~maxAmplitude, ()) => {
  let m = BA.dim(data);

  let data_cp = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);
  BA.blit(data, data_cp);

  fftInplace(data_cp);

  let maxAmplitudeFrequency = ref(0.);

  let spectrum = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);

  /* Compute the amplitude of the output of the FFT */
  for (i in 0 to m - 1) {
    BA.unsafe_set(spectrum, i, Complex.norm(BA.unsafe_get(data_cp, i)));
    if (BA.unsafe_get(spectrum, i) > maxAmplitudeFrequency^) {
      maxAmplitudeFrequency := BA.unsafe_get(spectrum, i);
    };
  };

  /* Scale the fft amplitude graph by the max amplitude found before */
  for (i in 0 to m - 1) {
    BA.unsafe_set(
      spectrum,
      i,
      BA.unsafe_get(spectrum, i) /. maxAmplitudeFrequency^ *. maxAmplitude,
    );
  };

  spectrum;
};

let hannWindow = (~data) => {
  let bucketSize = BA.dim(data);
  let total = float_of_int(bucketSize - 1);
  for (i in 0 to bucketSize - 1) {
    let multiplier = 0.5 -. 0.5 *. cos(two_pi *. float_of_int(i) /. total);
    let com = BA.unsafe_get(data, i);
    BA.unsafe_set(
      data,
      i,
      {Complex.re: com.Complex.re *. multiplier, im: com.Complex.im},
    );
  };
};

let hammingWindow = (~data) => {
  let bucketSize = BA.dim(data);
  let total = float_of_int(bucketSize - 1);
  for (i in 0 to bucketSize - 1) {
    let multiplier = 0.54 -. 0.46 *. cos(two_pi *. float_of_int(i) /. total);
    let com = BA.unsafe_get(data, i);
    BA.unsafe_set(
      data,
      i,
      {Complex.re: com.Complex.re *. multiplier, im: com.Complex.im},
    );
  };
};

let getMaxAmplitude = data => {
  let maxAmplitude = ref(0.);
  let size = BA.dim(data);
  for (i in 0 to size - 1) {
    let x = MyBigarray.Array1.get(data, i);
    if (x.Complex.re > maxAmplitude^) {
      maxAmplitude := x.Complex.re;
    };
  };
  maxAmplitude^;
};

let generateSineInplace =
    (~data, ~frequency, ~samplingRate, ~offset=0., ~numberOfSines=1, ()) => {
  let size = BA.dim(data);
  let samplingRatef = float_of_int(samplingRate);
  for (i in 0 to size - 1) {
    let x = ref(0.);
    for (k in 0 to numberOfSines - 1) {
      let coef = float_of_int(k * 2 + 1);
      x :=
        x^
        +. sin(
             offset
             *. coef
             +. float_of_int(i)
             *. frequency
             *. coef
             /. samplingRatef
             *. pi
             *. 2.,
           )
        /. coef;
    };
    BA.unsafe_set(data, i, Complex.{re: x^, im: 0.});
  };
};

let generateSine =
    (
      ~frequency,
      ~samplingRate=256,
      ~size=256,
      ~offset=0.,
      ~numberOfSines=1,
      (),
    ) => {
  let data = BA.create(MyBigarray.complex64, MyBigarray.c_layout, size);
  generateSineInplace(
    ~data,
    ~frequency,
    ~samplingRate,
    ~offset,
    ~numberOfSines,
    (),
  );
  data;
};
