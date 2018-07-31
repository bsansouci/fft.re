let pi = 4.0 *. atan(1.0);

let fftBinSize = 8192;
let frequency = 15.1;
let samplingRate = 8192;

open Reprocessing;

type state = {
  time: float,
  hamming: UI.checkboxT,
  hann: UI.checkboxT,
};

let setup = env => {
  Env.size(~width=1000, ~height=600, env);
  Draw.strokeCap(Square, env);
  {time: 0., hamming: UI.makeCheckbox(env), hann: UI.makeCheckbox(env)};
};

let (hammingX, hammingY) = (700, 100);
let (hannX, hannY) = (700, 160);

let prevDeltaTime = ref(0.);
let draw = (state, env) => {
  Draw.background(Utils.color(~r=199, ~g=217, ~b=229, ~a=255), env);
  let dt = Env.deltaTime(env);

  let offset = state.time /. 10.;

  let data =
    Fft.generateSine(
      ~frequency,
      ~samplingRate,
      ~offset,
      ~size=fftBinSize,
      ~numberOfSines=20,
      (),
    );

  let maxAmplitude = Fft.getMaxAmplitude(data);

  switch (state.hamming.animationState) {
  | CheckedToUnchecked =>
    Fft.hammingWindow(
      ~data,
      ~amount=
        Utils.remapf(
          ~value=state.hamming.time,
          ~low1=0.,
          ~high1=state.hamming.animationTime,
          ~low2=1.,
          ~high2=0.,
        ),
      (),
    )
  | UncheckedToChecked =>
    Fft.hammingWindow(
      ~data,
      ~amount=
        Utils.remapf(
          ~value=state.hamming.time,
          ~low1=0.,
          ~high1=state.hamming.animationTime,
          ~low2=0.,
          ~high2=1.,
        ),
      (),
    )
  | Nothing when state.hamming.checked => Fft.hammingWindow(~data, ())
  | _ => ()
  };

  switch (state.hann.animationState) {
  | CheckedToUnchecked =>
    Fft.hannWindow(
      ~data,
      ~amount=
        Utils.remapf(
          ~value=state.hann.time,
          ~low1=0.,
          ~high1=state.hann.animationTime,
          ~low2=1.,
          ~high2=0.,
        ),
      (),
    )
  | UncheckedToChecked =>
    Fft.hannWindow(
      ~data,
      ~amount=
        Utils.remapf(
          ~value=state.hann.time,
          ~low1=0.,
          ~high1=state.hann.animationTime,
          ~low2=0.,
          ~high2=1.,
        ),
      (),
    )
  | Nothing when state.hann.checked => Fft.hannWindow(~data, ())
  | _ => ()
  };

  let spectrum = Fft.fft(~data, ~maxAmplitude, ());

  /* Hamming option */
  let hamming = {
    Draw.text(~body="Hamming window", ~pos=(738, 100), env);
    UI.drawCheckbox(~checkbox=state.hamming, ~pos=(hammingX, hammingY), env);
  };

  /* Hann option */
  let hann = {
    Draw.text(~body="Hann window", ~pos=(738, 160), env);
    UI.drawCheckbox(~checkbox=state.hann, ~pos=(hannX, hannY), env);
  };
  let (mx, _) = Env.mouse(env);

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
           data,
           int_of_float((f -. 1.) *. timeWindowSeconds),
         ).Complex.re
      *. 100.,
    );
    let cur = (
      padding +. float_of_int(i) *. 2.,
      150.
      +. MyBigarray.Array1.get(data, int_of_float(f *. timeWindowSeconds)).Complex.re
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
  {...state, time: state.time +. dt, hann, hamming};
};

run(~setup, ~draw, ());
