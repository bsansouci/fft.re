let pi = 4.0 *. atan(1.0);

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
      (),
    );

  let maxAmplitude = ref(0.);
  for (i in 0 to fftBinSize - 1) {
    if (MyBigarray.Array1.get(reals, i) > maxAmplitude^) {
      maxAmplitude := MyBigarray.Array1.get(reals, i);
    };
  };

  if (state.hamming) {
    Fft.hammingWindow(~reals);
  };

  if (state.hann) {
    Fft.hannWindow(~reals);
  };

  let spectrum =
    Fft.fft(~reals, ~imaginaries, ~maxAmplitude=maxAmplitude^, ());

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
