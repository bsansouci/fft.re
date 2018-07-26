let pi = 4.0 *. atan(1.0);

let fftBinSize = 256;
/* Optimization: we pre-compute cos and sin! */
/*let coefsReal =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize / 2);
let coefsIm =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize / 2);

for (i in 0 to fftBinSize / 2 - 1) {
  let a = -. pi *. float_of_int(i) /. float_of_int(fftBinSize / 2);
  coefsReal.{i} = cos(a);
  coefsIm.{i} = sin(a);
}*/

let rec fft = (x_re, x_im) => {
  let m = Bigarray.Array1.dim(x_re) / 2;
  let x_re2 = Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, m);
  let x_im2 = Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, m);
  let y_re2 = Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, m);
  let y_im2 = Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, m);

  for (k in 0 to m - 1) {
    x_re2.{k} = x_re.{(2 * k)};
    x_im2.{k} = x_im.{(2 * k)};
    y_re2.{k} = x_re.{(2 * k + 1)};
    y_im2.{k} = x_im.{(2 * k + 1)};
  };

  if (m > 1) {
    fft(x_re2, x_im2);
    fft(y_re2, y_im2);
  };

  for (k in 0 to m - 1) {
    let a = -. pi *. float_of_int(k) /. float_of_int(m);
    let tw_re = cos(a);
    let tw_im = sin(a);
    let a = tw_re *. y_re2.{k} -. tw_im *. y_im2.{k};
    let b = tw_re *. y_im2.{k} +. tw_im *. y_re2.{k};
    x_re.{k} = x_re2.{k} +. a;
    x_im.{k} = x_im2.{k} +. b;
    x_re.{(k + m)} = x_re2.{k} -. a;
    x_im.{(k + m)} = x_im2.{k} -. b;
  };
};

let two_pi = pi *. 2.;

let hannWindow = (data, bucketSize) =>
  for (i in 0 to bucketSize - 1) {
    let multiplier =
      0.5
      -. 0.5
      *. cos(two_pi *. float_of_int(i) /. float_of_int(bucketSize - 1));
    data.{i} = data.{i} *. multiplier;
  };
let hammingWindow = (data, bucketSize) =>
  for (i in 0 to bucketSize - 1) {
    let multiplier =
      0.54
      -. 0.46
      *. cos(two_pi *. float_of_int(i) /. float_of_int(bucketSize - 1));

    data.{i} = data.{i} *. multiplier;
  };
let frequency = 17.5;
let samplingRate = 256;
/*let dataLength = samplingRate * timeIntervalSeconds;*/
/*let data = open_out("data");
  output_string(data, string_of_int(dataLength) ++ "\n");
  for (i in 0 to dataLength - 1) {
    output_string(
      data,
      string_of_float(
        sin(
          float_of_int(i)
          /. float_of_int(samplingRate * timeIntervalSeconds)
          *. pi
          *. 2.,
        ),
      )
      ++ "\n",
    );
  };
  close_out(data);

  let data = open_in("data");
  let len = int_of_string(input_line(data));
  let r = Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, len);
  let maxAmplitude = ref(0.);
  for (i in 0 to len - 1) {
    r.{i} = float_of_string(input_line(data));
    if (r.{i} > maxAmplitude^) {
      maxAmplitude := r.{i};
    };
  };
  close_in(data);*/

let r =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
let drawingReal =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
let imag =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
/*let maxAmplitude = ref(0.);
  for (i in 0 to fftBinSize - 1) {
    r.{i} =
      sin(
        float_of_int(i * frequency) /. float_of_int(samplingRate) *. pi *. 2.,
      )
      +. sin(
           float_of_int(i * frequency * 3)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 3.
      +. sin(
           float_of_int(i * frequency * 5)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 5.
      +. sin(
           float_of_int(i * frequency * 7)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 7.
      +. sin(
           float_of_int(i * frequency * 9)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 9.
      +. sin(
           float_of_int(i * frequency * 11)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 11.
      +. sin(
           float_of_int(i * frequency * 13)
           /. float_of_int(samplingRate)
           *. pi
           *. 2.,
         )
      /. 13.;

    drawingReal.{i} = r.{i};
    imag.{i} = 0.;
    if (r.{i} > maxAmplitude^) {
      maxAmplitude := r.{i};
    };
  };

  fft(r, imag);

  let spectrum =
    Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
  let maxAmplitudeFrequency = ref(0.);

  /* Compute the amplitude of the output of the FFT */
  for (i in 0 to fftBinSize - 1) {
    spectrum.{i} = sqrt(r.{i} *. r.{i} +. imag.{i} *. imag.{i});
    if (spectrum.{i} > maxAmplitudeFrequency^) {
      maxAmplitudeFrequency := spectrum.{i};
    };
  };

  /* Scale the fft amplitude graph by the max amplitude found before */
  for (i in 0 to fftBinSize - 1) {
    spectrum.{i} = spectrum.{i} /. maxAmplitudeFrequency^ *. maxAmplitude^;
  };*/

open Reprocessing;

let setup = env => {
  Env.size(~width=1000, ~height=600, env);
  Draw.strokeCap(Square, env);
  0.;
};

let spectrum =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);

let freqReal =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize + 1);
let freqIm =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize + 1);
let runningReal =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
let runningIm =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
let runningSepctrum =
  Bigarray.Array1.create(Bigarray.float64, Bigarray.c_layout, fftBinSize);
for (i in 0 to fftBinSize - 1) {
  runningReal.{0} = 0.;
  runningIm.{0} = 0.;
  freqReal.{0} =0.;
  freqIm.{0} =0.;
  runningSepctrum.{0} = 0.;
};
freqReal.{fftBinSize} = 0.;
freqIm.{fftBinSize} = 0.;

/*
 (a+ib) * (a2+ib2)
 a * a2 + i*a*b2 + i*b*a2 - b*b2
 (a*a2 - b*b2) + i(a*b2 + b*a2)
 */

let updateRunningFft = (newDataPointReal, newDataPointIm, idx) => {
  let oldDataPointReal = runningReal.{idx};
  let oldDataPointIm = runningIm.{idx};
  runningReal.{idx} = newDataPointReal;
  runningIm.{idx} = newDataPointIm;
  let deltaReal = newDataPointReal -. oldDataPointReal;
  let deltaIm = newDataPointIm -. oldDataPointIm;
  let ci = ref(0);

  for (i in 0 to fftBinSize - 1) {
    let a = -. pi *. float_of_int(ci^) /. float_of_int(fftBinSize / 2);
    let tw_re = cos(a);
    let tw_im = sin(a);
    freqReal.{i} = freqReal.{i} +. (deltaReal *. tw_re -. deltaIm *. tw_im);
    freqIm.{i} = freqIm.{i} +. deltaReal *. tw_im +. deltaIm *. tw_re;
    runningSepctrum.{i} = sqrt(freqReal.{i} *. freqReal.{i} +. freqIm.{i} *. freqIm.{i});
    ci := ci^ + idx;
    if (ci^ >= fftBinSize) {
      ci := ci^ - fftBinSize;
    };
  };
};

let prevDeltaTime = ref(0.);

let draw = (tick, env) => {
  Draw.background(Utils.color(~r=199, ~g=217, ~b=229, ~a=255), env);
  let dt = Env.deltaTime(env);

  let numberOfSamplesToGenerate = dt /. (1. /. float_of_int(samplingRate));
  let j = tick *. 10.;
  /*Draw.text(
    ~body=Printf.sprintf("** %d ** ", int_of_float(numberOfSamplesToGenerate) - 1),
    ~pos=(400, 10),
    env,
  );*/
  /*for (i in 0 to int_of_float(numberOfSamplesToGenerate) - 1) {
    updateRunningFft(
      sin(
        j +.
         float_of_int(i)
        *.
        frequency
        /. float_of_int(samplingRate)
        *. pi
        *. 2.,
      ),
      0.,
      int_of_float(j) mod fftBinSize,
    );
  };*/

  let maxAmplitude = ref(0.);
  for (i in 0 to fftBinSize - 1) {
    r.{i} = 0.;
    imag.{i} = 0.;
    for (k in 0 to 10) {
      let coef = float_of_int(k * 2 + 1);
      r.{i} =
        r.{i}
        +. sin(
             j
             *. coef
             +. float_of_int(i)
             *. frequency
             *. coef
             /. float_of_int(samplingRate)
             *. pi
             *. 2.,
           )
        /. coef;
      /*let coef2 = float_of_int(k * 2 + 1);
        imag.{i} =
          imag.{i}
          +. sin(
               j
               *. coef2
               +. float_of_int(i)
               *. frequency
               *. coef2
               /. float_of_int(samplingRate)
               *. pi
               *. 2.,
             )
          *. 4.
          /. (coef2 *. pi);*/
    };

    drawingReal.{i} = r.{i};
    if (r.{i} > maxAmplitude^) {
      maxAmplitude := r.{i};
    };
  };
  /*hammingWindow(r, fftBinSize);
    hammingWindow(drawingReal, fftBinSize);*/

  /*hannWindow(r, fftBinSize);
    hannWindow(drawingReal, fftBinSize);*/

  let prevTime = Unix.gettimeofday();
  fft(r, imag);
  prevDeltaTime :=
    prevDeltaTime^ *. 0.8 +. 0.2 *. (Unix.gettimeofday() -. prevTime);
  Draw.text(
    ~body=Printf.sprintf("fft: %fms", prevDeltaTime^ *. 1000.),
    ~pos=(700, 10),
    env,
  );

  let maxAmplitudeFrequency = ref(0.);

  /* Compute the amplitude of the output of the FFT */
  for (i in 0 to fftBinSize - 1) {
    spectrum.{i} = sqrt(r.{i} *. r.{i} +. imag.{i} *. imag.{i});
    if (spectrum.{i} > maxAmplitudeFrequency^) {
      maxAmplitudeFrequency := spectrum.{i};
    };
  };

  /* Scale the fft amplitude graph by the max amplitude found before */
  for (i in 0 to fftBinSize - 1) {
    spectrum.{i} = spectrum.{i} /. maxAmplitudeFrequency^ *. maxAmplitude^;
  };

  let padding = 10.;
  let bucketSize = 5.;
  let (mx, _) = Env.mouse(env);
  let j = int_of_float((float_of_int(mx) -. padding) /. bucketSize);
  if (j >= 0 && j < fftBinSize) {
    Draw.text(
      ~body=Printf.sprintf("%dHz - %f", j, spectrum.{j}),
      ~pos=(10, 10),
      env,
    );
  };
  let timeWindowSeconds = float_of_int(samplingRate) /. 256.;
  Draw.strokeWeight(1, env);
  for (i in 1 to fftBinSize / int_of_float(timeWindowSeconds) - 1) {
    Draw.stroke(Utils.color(~r=0, ~g=0, ~b=0, ~a=255), env);
    let f = float_of_int(i);
    let prev = (
      padding +. float_of_int(i - 1) *. 2.,
      150.
      +. drawingReal.{(int_of_float((f -. 1.) *. timeWindowSeconds))}
      *. 100.,
    );
    let cur = (
      padding +. float_of_int(i) *. 2.,
      150. +. drawingReal.{(int_of_float(f *. timeWindowSeconds))} *. 100.,
    );
    Draw.linef(~p1=prev, ~p2=cur, env);
  };
  Draw.noStroke(env);
  let bottom = 600.;
  for (i in 0 to fftBinSize / 2 - 1) {
    let height = spectrum.{i} /. maxAmplitude^ *. 400.;
    if (i == j) {
      Draw.fill(Utils.color(~r=255, ~g=255, ~b=255, ~a=255), env);
    } else {
      Draw.fill(
        Utils.color(
          ~r=255 - int_of_float(spectrum.{i} /. maxAmplitude^ *. 255.),
          ~g=int_of_float(spectrum.{i} /. maxAmplitude^ *. 255.),
          ~b=0,
          ~a=255,
        ),
        env,
      );
    };
    Draw.rectf(
      ~pos=(padding +. float_of_int(i) *. bucketSize, bottom -. height),
      ~width=bucketSize -. 1.,
      ~height,
      env,
    );
  };

  tick +. dt;
};

run(~setup, ~draw, ());
