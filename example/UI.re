type checkboxTransitionT =
  | CheckedToUnchecked
  | UncheckedToChecked
  | Nothing;
type checkboxT = {
  checkedImage: Reprocessing.imageT,
  uncheckedImage: Reprocessing.imageT,
  animationState: checkboxTransitionT,
  checked: bool,
  time: float,
  animationTime: float,
  prevPressDown: bool,
};

let opacity = (alpha, env) =>
  Reprocessing.(
    Draw.tint(
      Utils.color(
        ~r=255,
        ~g=255,
        ~b=255,
        ~a=
          int_of_float(
            Utils.remapf(
              ~value=alpha,
              ~low1=0.,
              ~high1=1.,
              ~low2=0.,
              ~high2=255.,
            ),
          ),
      ),
      env,
    )
  );
let drawCheckbox = (~checkbox, ~pos as (x, y) as pos, env) => {
  open Reprocessing;
  let size = 28;

  let (mx, my) = Env.mouse(env);
  let pressDown = Env.mousePressed(env);

  let withinBounds = mx > x && mx < x + size && my > y && my < y + size;
  let clicked = checkbox.prevPressDown && ! pressDown && withinBounds;
  switch (clicked, checkbox.animationState) {
  | (true, CheckedToUnchecked)
  | (false, CheckedToUnchecked) =>
    Draw.image(checkbox.uncheckedImage, ~pos, ~width=size, ~height=size, env);

    Draw.pushMatrix(env);
    opacity(
      Utils.remapf(
        ~value=checkbox.time,
        ~low1=0.,
        ~high1=checkbox.animationTime,
        ~low2=1.,
        ~high2=0.,
      ),
      env,
    );
    let scale =
      Utils.remapf(
        ~value=checkbox.time,
        ~low1=0.,
        ~high1=checkbox.animationTime,
        ~low2=1.,
        ~high2=0.9,
      );
    let scaledSize = float_of_int(size) /. 2. *. scale;
    Draw.translate(
      ~x=float_of_int(x) +. scaledSize,
      ~y=float_of_int(y) +. scaledSize,
      env,
    );
    Draw.scale(~x=scale, ~y=scale, env);
    Draw.translate(~x=-. scaledSize, ~y=-. scaledSize, env);
    Draw.image(
      checkbox.checkedImage,
      ~pos=(0, 0),
      ~width=size,
      ~height=size,
      env,
    );
    Draw.popMatrix(env);
    opacity(1., env);

    if (checkbox.time >= checkbox.animationTime) {
      {
        ...checkbox,
        animationState: Nothing,
        time: 0.,
        prevPressDown: pressDown,
      };
    } else {
      {
        ...checkbox,
        time:
          min(checkbox.time +. Env.deltaTime(env), checkbox.animationTime),
        prevPressDown: pressDown,
      };
    };
  | (true, UncheckedToChecked)
  | (false, UncheckedToChecked) =>
    Draw.image(checkbox.uncheckedImage, ~pos, ~width=size, ~height=size, env);
    opacity(
      Utils.remapf(
        ~value=checkbox.time,
        ~low1=0.,
        ~high1=checkbox.animationTime,
        ~low2=0.,
        ~high2=1.,
      ),
      env,
    );
    Draw.pushMatrix(env);
    let scale =
      Utils.remapf(
        ~value=checkbox.time,
        ~low1=0.,
        ~high1=checkbox.animationTime,
        ~low2=0.8,
        ~high2=1.,
      );
    let scaledSize = float_of_int(size) /. 2. *. scale;
    Draw.translate(
      ~x=float_of_int(x) +. scaledSize,
      ~y=float_of_int(y) +. scaledSize,
      env,
    );
    Draw.scale(~x=scale, ~y=scale, env);
    Draw.translate(~x=-. scaledSize, ~y=-. scaledSize, env);
    Draw.image(
      checkbox.checkedImage,
      ~pos=(0, 0),
      ~width=size,
      ~height=size,
      env,
    );
    Draw.popMatrix(env);
    opacity(1., env);

    if (checkbox.time >= checkbox.animationTime) {
      {
        ...checkbox,
        animationState: Nothing,
        time: 0.,
        prevPressDown: pressDown,
      };
    } else {
      {
        ...checkbox,
        time:
          min(checkbox.time +. Env.deltaTime(env), checkbox.animationTime),
        prevPressDown: pressDown,
      };
    };
  | (true, Nothing) =>
    if (checkbox.checked) {
      Draw.image(checkbox.checkedImage, ~pos, ~width=size, ~height=size, env);

      {
        ...checkbox,
        animationState: CheckedToUnchecked,
        checked: false,
        prevPressDown: pressDown,
      };
    } else {
      Draw.image(
        checkbox.uncheckedImage,
        ~pos,
        ~width=size,
        ~height=size,
        env,
      );
      {
        ...checkbox,
        animationState: UncheckedToChecked,
        checked: true,
        prevPressDown: pressDown,
      };
    }
  | (false, Nothing) =>
    if (checkbox.checked) {
      Draw.image(checkbox.checkedImage, ~pos, ~width=size, ~height=size, env);
    } else {
      Draw.image(
        checkbox.uncheckedImage,
        ~pos,
        ~width=size,
        ~height=size,
        env,
      );
    };
    {...checkbox, prevPressDown: pressDown};
  };
};
let makeCheckbox = (~checked=false, env) => {
  checkedImage:
    Reprocessing.Draw.loadImage(~filename="assets/checked.png", env),
  uncheckedImage:
    Reprocessing.Draw.loadImage(~filename="assets/unchecked.png", env),
  animationState: Nothing,
  checked,
  time: 0.,
  animationTime: 0.2,
  prevPressDown: false,
};
