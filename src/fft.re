module BA = MyBigarray.Array1;

let pi = 4.0 *. atan(1.0);
let two_pi = pi *. 2.;

let rec fftInplace = (reals, imaginaries) => {
  let m = BA.dim(reals) / 2;
  let x_re2 = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  let x_im2 = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  let y_re2 = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  let y_im2 = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);

  for (k in 0 to m - 1) {
    BA.set(x_re2, k, BA.get(reals, 2 * k));
    BA.set(x_im2, k, BA.get(imaginaries, 2 * k));
    BA.set(y_re2, k, BA.get(reals, 2 * k + 1));
    BA.set(y_im2, k, BA.get(imaginaries, 2 * k + 1));
  };

  if (m > 1) {
    fftInplace(x_re2, x_im2);
    fftInplace(y_re2, y_im2);
  };

  for (k in 0 to m - 1) {
    let a = -. pi *. float_of_int(k) /. float_of_int(m);
    let tw_re = cos(a);
    let tw_im = sin(a);
    let a = tw_re *. BA.get(y_re2, k) -. tw_im *. BA.get(y_im2, k);
    let b = tw_re *. BA.get(y_im2, k) +. tw_im *. BA.get(y_re2, k);
    BA.set(reals, k, BA.get(x_re2, k) +. a);
    BA.set(imaginaries, k, BA.get(x_im2, k) +. b);
    BA.set(reals, k + m, BA.get(x_re2, k) -. a);
    BA.set(imaginaries, k + m, BA.get(x_im2, k) -. b);
  };
};

let rec fftInplace2 = complex => {
  let m = BA.dim(complex) / 2;
  let x_re2 = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);
  let y_re2 = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);

  for (k in 0 to m - 1) {
    BA.set(x_re2, k, BA.get(complex, 2 * k));
    BA.set(y_re2, k, BA.get(complex, 2 * k + 1));
  };

  if (m > 1) {
    fftInplace2(x_re2);
    fftInplace2(y_re2);
  };

  for (k in 0 to m - 1) {
    let a = -. pi *. float_of_int(k) /. float_of_int(m);
    let tw_re = cos(a);
    let tw_im = sin(a);
    let com = BA.get(complex, k);
    let new_com =
      Complex.{
        re: tw_re *. com.Complex.re -. tw_im *. com.Complex.im,
        im: tw_re *. com.Complex.im +. tw_im *. com.Complex.re,
      };

    BA.set(complex, k, Complex.add(BA.get(x_re2, k), new_com));
    BA.set(complex, k + m, Complex.sub(BA.get(x_re2, k), new_com));
  };
};

let fft2 = (~data, ~maxAmplitude, ()) => {
  let m = BA.dim(data);

  let data_cp = BA.create(MyBigarray.complex64, MyBigarray.c_layout, m);
  BA.blit(data, data_cp);

  /*let imaginaries_cp = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);*/
  /*BA.blit(imaginaries, imaginaries_cp);*/

  fftInplace2(data_cp);

  let maxAmplitudeFrequency = ref(0.);

  let spectrum = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  /* Compute the amplitude of the output of the FFT */
  for (i in 0 to m - 1) {
    BA.set(spectrum, i, Complex.norm(BA.get(data_cp, i)));
    if (BA.get(spectrum, i) > maxAmplitudeFrequency^) {
      maxAmplitudeFrequency := BA.get(spectrum, i);
    };
  };
  /* Scale the fft amplitude graph by the max amplitude found before */
  for (i in 0 to m - 1) {
    BA.set(
      spectrum,
      i,
      BA.get(spectrum, i) /. maxAmplitudeFrequency^ *. maxAmplitude,
    );
  };

  spectrum;
};

let fft = (~reals, ~imaginaries, ~maxAmplitude, ()) => {
  let m = BA.dim(reals);

  let reals_cp = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  BA.blit(reals, reals_cp);

  let imaginaries_cp = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  BA.blit(imaginaries, imaginaries_cp);

  fftInplace(reals_cp, imaginaries_cp);

  let maxAmplitudeFrequency = ref(0.);

  let spectrum = BA.create(MyBigarray.float64, MyBigarray.c_layout, m);
  /* Compute the amplitude of the output of the FFT */
  for (i in 0 to m - 1) {
    BA.set(
      spectrum,
      i,
      sqrt(
        BA.get(reals_cp, i)
        *. BA.get(reals_cp, i)
        +. BA.get(imaginaries_cp, i)
        *. BA.get(imaginaries_cp, i),
      ),
    );
    if (BA.get(spectrum, i) > maxAmplitudeFrequency^) {
      maxAmplitudeFrequency := BA.get(spectrum, i);
    };
  };
  /* Scale the fft amplitude graph by the max amplitude found before */
  for (i in 0 to m - 1) {
    BA.set(
      spectrum,
      i,
      BA.get(spectrum, i) /. maxAmplitudeFrequency^ *. maxAmplitude,
    );
  };

  spectrum;
};

let hannWindow = (~reals) => {
  let bucketSize = BA.dim(reals);
  for (i in 0 to bucketSize - 1) {
    let multiplier =
      0.5
      -. 0.5
      *. cos(two_pi *. float_of_int(i) /. float_of_int(bucketSize - 1));
    BA.set(reals, i, BA.get(reals, i) *. multiplier);
  };
};

let hammingWindow = (~reals) => {
  let bucketSize = BA.dim(reals);

  for (i in 0 to bucketSize - 1) {
    let multiplier =
      0.54
      -. 0.46
      *. cos(two_pi *. float_of_int(i) /. float_of_int(bucketSize - 1));

    BA.set(reals, i, BA.get(reals, i) *. multiplier);
  };
};

let getMaxAmplitude = reals => {
  let maxAmplitude = ref(0.);
  let size = BA.dim(reals);
  for (i in 0 to size - 1) {
    if (MyBigarray.Array1.get(reals, i) > maxAmplitude^) {
      maxAmplitude := MyBigarray.Array1.get(reals, i);
    };
  };
  maxAmplitude^;
};

let getMaxAmplitude2 = data => {
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
    (
      ~reals,
      ~imaginaries,
      ~frequency,
      ~samplingRate,
      ~size,
      ~offset=0.,
      ~numberOfSines=1,
      (),
    ) => {
  let samplingRatef = float_of_int(samplingRate);
  for (i in 0 to size - 1) {
    BA.set(reals, i, 0.);
    BA.set(imaginaries, i, 0.);
    for (k in 0 to numberOfSines - 1) {
      let coef = float_of_int(k * 2 + 1);
      BA.set(
        reals,
        i,
        BA.get(reals, i)
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
        /. coef,
      );
    };
  };
};

let generateSineInplace2 =
    (
      ~data,
      ~frequency,
      ~samplingRate,
      ~size,
      ~offset=0.,
      ~numberOfSines=1,
      (),
    ) => {
  let samplingRatef = float_of_int(samplingRate);
  for (i in 0 to size - 1) {
    /*BA.set(reals, i, 0.);*/
    /*BA.set(imaginaries, i, 0.);*/
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
    BA.set(data, i, Complex.{re: x^, im: 0.});
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
  let reals = BA.create(MyBigarray.float64, MyBigarray.c_layout, size);
  let imaginaries = BA.create(MyBigarray.float64, MyBigarray.c_layout, size);
  generateSineInplace(
    ~reals,
    ~imaginaries,
    ~frequency,
    ~samplingRate,
    ~size,
    ~offset,
    ~numberOfSines,
    (),
  );
  (reals, imaginaries);
};

let generateSine2 =
    (
      ~frequency,
      ~samplingRate=256,
      ~size=256,
      ~offset=0.,
      ~numberOfSines=20,
      (),
    ) => {
  let data = BA.create(MyBigarray.complex64, MyBigarray.c_layout, size);
  generateSineInplace2(
    ~data,
    ~frequency,
    ~samplingRate,
    ~size,
    ~offset,
    ~numberOfSines,
    (),
  );
  data;
};
