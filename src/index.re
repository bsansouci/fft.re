let sum = ref(0.);
let iterations = 100;
let hack = ref(0.);
for (_ in 0 to iterations - 1) {
  let data = Fft.generateSine2(
    ~frequency=5000.,
    ~samplingRate=131072,
    ~size=131072,
    ~numberOfSines=10,
    (),
  );
  let maxAmplitude = Fft.getMaxAmplitude2(data);
  let prevTime = Unix.gettimeofday();
  let spectrum = Fft.fft2(~data, ~maxAmplitude, ());
  hack := MyBigarray.Array1.get(spectrum, 0);
  sum := sum^ +. (Unix.gettimeofday() -. prevTime);
};
sum := sum^ /. float_of_int(iterations);
Printf.printf("%fs\n", sum^);

/*
  For 2 arrays of floats
  Sampling rate: 256Hz
  FFT bucket size: 256 points
  Iterations: 10,000
  Time taken: 0.000244 sec/iteration (0.244 ms)

  Using complex module:
  Sampling rate: 256Hz
  FFT bucket size: 256 points
  Iterations: 10,000
  Time taken: 0.000150 sec/iteration (0.150 ms)

  Using complex module:
  Sampling rate: 2048Hz
  FFT bucket size: 2048 points
  Iterations: 500
  Time take: 0.001471 sec/iteration (1.471 ms)

  Using complex module:
  Sampling rate: 8192Hz
  FFT bucket size: 8192 points
  Iterations: 500
  Time take: 0.006063 sec/iteration (6.063 ms)

  Using complex module (with global and mutable bigarray bucket):
  Sampling rate: 8192Hz
  FFT bucket size: 8192 points
  Iterations: 500
  Time take: 0.005910 sec/iteration (5.91 ms)

  Using complex module:
  Sampling rate: 131072Hz
  FFT bucket size: 131072 points
  Iterations: 100
  Time take: 0.111438 sec/iteration (111.438 ms)
 */

/*let pi = 4.0 *. atan(1.0);

  let fftBinSize = 1024;

  let frequency = 1.;
  let samplingRate = 1024;

  open Reprocessing;

  type state = {
    time: float,
    checked: imageT,
    unchecked: imageT,
    hamming: bool,
    hann: bool,
  };

  let setup = env => {
    Env.size(~width=1000, ~height=600, env);
    Draw.strokeCap(Square, env);
    {
      time: 0.,
      checked: Draw.loadImage(~filename="assets/checked.png", env),
      unchecked: Draw.loadImage(~filename="assets/unchecked.png", env),
      hamming: false,
      hann: false,
    };
  };

  let (hammingX, hammingY) = (700, 100);
  let (hannX, hannY) = (700, 160);

  let mouseDown = (state, env) => {
    let (mx, my) = Env.mouse(env);
    if (mx > hammingX
        && mx < hammingX
        + 28
        && my > hammingY
        && my < hammingY
        + 28) {
      {...state, hamming: ! state.hamming};
    } else if (mx > hannX && mx < hannX + 28 && my > hannY && my < hannY + 28) {
      {...state, hann: ! state.hann};
    } else {
      state;
    };
  };

  let prevDeltaTime = ref(0.);
  let draw = (state, env) => {
    Draw.background(Utils.color(~r=199, ~g=217, ~b=229, ~a=255), env);
    let dt = Env.deltaTime(env);

    let offset = state.time;

    let (reals, imaginaries) =
      Fft.generateSine(
        ~frequency,
        ~samplingRate,
        ~offset,
        ~size=fftBinSize,
        ~numberOfSines=20,
        (),
      );

    let maxAmplitude = Fft.getMaxAmplitude(reals);

    if (state.hamming) {
      Fft.hammingWindow(~reals);
    };

    if (state.hann) {
      Fft.hannWindow(~reals);
    };

    let spectrum = Fft.fft(~reals, ~imaginaries, ~maxAmplitude, ());

    let (mx, _) = Env.mouse(env);

    {
      if (state.hamming) {
        Draw.image(state.checked, ~pos=(hammingX, hammingY), env);
      } else {
        /*   let width = 1.;
             Draw.strokeCap(Round, env);
             Draw.strokeWeight(2, env);
             Draw.stroke(Utils.color(0, 0, 0, 255), env);
             Draw.fill(Utils.color(0, 0, 0, 255), env);
             Draw.rect(~pos=(hammingX, hammingY), ~width=18, ~height=18, env);
             Draw.strokeWeight(2, env);
             Draw.stroke(Utils.color(189, 189, 189, 255), env);
             Draw.fill(Utils.color(189, 189, 189, 255), env);
             Draw.rectf(
               ~pos=(
                 float_of_int(hammingX) +. width /. 2.,
                 float_of_int(hammingY) +. width /. 2.,
               ),
               ~width=18. -. width,
               ~height=18. -. width,
               env,
             );*/
        Draw.image(
          state.unchecked,
          ~pos=(hammingX, hammingY),
          env,
        );
      };
      Draw.text(~body="Hamming window", ~pos=(738, 100), env);
    };

    /* Hann option */
    {
      if (state.hann) {
        Draw.image(state.checked, ~pos=(hannX, hannY), env);
      } else {
        /*   let width = 1.;
             Draw.strokeCap(Round, env);
             Draw.strokeWeight(2, env);
             Draw.stroke(Utils.color(0, 0, 0, 255), env);
             Draw.fill(Utils.color(0, 0, 0, 255), env);
             Draw.rect(~pos=(hannX, hannY), ~width=18, ~height=18, env);
             Draw.strokeWeight(2, env);
             Draw.stroke(Utils.color(189, 189, 189, 255), env);
             Draw.fill(Utils.color(189, 189, 189, 255), env);
             Draw.rectf(
               ~pos=(
                 float_of_int(hannX) +. width /. 2.,
                 float_of_int(hannY) +. width /. 2.,
               ),
               ~width=18. -. width,
               ~height=18. -. width,
               env,
             );*/
        Draw.image(
          state.unchecked,
          ~pos=(hannX, hannY),
          env,
        );
      };

      Draw.text(~body="Hann window", ~pos=(738, 160), env);
    };

    let padding = 10.;
    let bucketSize = 5.;
    let j = int_of_float((float_of_int(mx) -. padding) /. bucketSize);
    if (j >= 0 && j < fftBinSize) {
      Draw.text(
        ~body=
          Printf.sprintf("%dHz - %f", j, MyBigarray.Array1.get(spectrum, j)),
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
        +. MyBigarray.Array1.get(
             reals,
             int_of_float((f -. 1.) *. timeWindowSeconds),
           )
        *. 100.,
      );
      let cur = (
        padding +. float_of_int(i) *. 2.,
        150.
        +. MyBigarray.Array1.get(reals, int_of_float(f *. timeWindowSeconds))
        *. 100.,
      );
      Draw.linef(~p1=prev, ~p2=cur, env);
    };
    Draw.noStroke(env);
    let bottom = 600.;
    for (i in 0 to fftBinSize / 2 - 1) {
      let height = MyBigarray.Array1.get(spectrum, i) *. 400.;
      if (i == j) {
        Draw.fill(Utils.color(~r=255, ~g=255, ~b=255, ~a=255), env);
      } else {
        Draw.fill(
          Utils.color(
            ~r=255 - int_of_float(MyBigarray.Array1.get(spectrum, i) *. 255.),
            ~g=int_of_float(MyBigarray.Array1.get(spectrum, i) *. 255.),
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
    {...state, time: state.time +. dt};
  };

  run(~setup, ~draw, ~mouseDown, ());
  */
