// Generated by BUCKLESCRIPT VERSION 3.2.0, PLEASE EDIT WITH CARE

import * as Fft from "../src/fft.js";
import * as Block from "bs-platform/lib/es6/block.js";
import * as Curry from "bs-platform/lib/es6/curry.js";
import * as Printf from "bs-platform/lib/es6/printf.js";
import * as Caml_int32 from "bs-platform/lib/es6/caml_int32.js";
import * as Reprocessing from "/Users/sansouci/Desktop/fft/node_modules/reprocessing/src/Reprocessing.js";
import * as Reprocessing_Env from "/Users/sansouci/Desktop/fft/node_modules/reprocessing/src/Reprocessing_Env.js";
import * as Reprocessing_Draw from "/Users/sansouci/Desktop/fft/node_modules/reprocessing/src/Reprocessing_Draw.js";
import * as Reprocessing_Utils from "/Users/sansouci/Desktop/fft/node_modules/reprocessing/src/Reprocessing_Utils.js";

var pi = 4.0 * Math.atan(1.0);

function setup(env) {
  Reprocessing_Env.size(1000, 600, env);
  Reprocessing_Draw.strokeCap(/* Square */1, env);
  return /* record */[
          /* time */0,
          /* checked */Reprocessing_Draw.loadImage("assets/checked.png", /* None */0, env),
          /* unchecked */Reprocessing_Draw.loadImage("assets/unchecked.png", /* None */0, env),
          /* hamming */false,
          /* hann */false
        ];
}

function mouseDown(state, env) {
  var match = Reprocessing_Env.mouse(env);
  var my = match[1];
  var mx = match[0];
  if (mx > 700 && mx < 728 && my > 100 && my < 128) {
    return /* record */[
            /* time */state[/* time */0],
            /* checked */state[/* checked */1],
            /* unchecked */state[/* unchecked */2],
            /* hamming */!state[/* hamming */3],
            /* hann */state[/* hann */4]
          ];
  } else if (mx > 700 && mx < 728 && my > 160 && my < 188) {
    return /* record */[
            /* time */state[/* time */0],
            /* checked */state[/* checked */1],
            /* unchecked */state[/* unchecked */2],
            /* hamming */state[/* hamming */3],
            /* hann */!state[/* hann */4]
          ];
  } else {
    return state;
  }
}

var prevDeltaTime = [0];

function draw(state, env) {
  Reprocessing_Draw.background(Reprocessing_Utils.color(199, 217, 229, 255), env);
  var dt = Reprocessing_Env.deltaTime(env);
  var offset = state[/* time */0];
  var data = Fft.generateSine(15.1, /* Some */[8192], /* Some */[8192], /* Some */[offset], /* Some */[20], /* () */0);
  var maxAmplitude = Fft.getMaxAmplitude(data);
  if (state[/* hamming */3]) {
    Fft.hammingWindow(data);
  }
  if (state[/* hann */4]) {
    Fft.hannWindow(data);
  }
  var spectrum = Fft.fft(data, maxAmplitude, /* () */0);
  var match = Reprocessing_Env.mouse(env);
  if (state[/* hamming */3]) {
    Reprocessing_Draw.image(state[/* checked */1], /* tuple */[
          700,
          100
        ], /* None */0, /* None */0, env);
  } else {
    Reprocessing_Draw.image(state[/* unchecked */2], /* tuple */[
          700,
          100
        ], /* None */0, /* None */0, env);
  }
  Reprocessing_Draw.text(/* None */0, "Hamming window", /* tuple */[
        738,
        100
      ], env);
  if (state[/* hann */4]) {
    Reprocessing_Draw.image(state[/* checked */1], /* tuple */[
          700,
          160
        ], /* None */0, /* None */0, env);
  } else {
    Reprocessing_Draw.image(state[/* unchecked */2], /* tuple */[
          700,
          160
        ], /* None */0, /* None */0, env);
  }
  Reprocessing_Draw.text(/* None */0, "Hann window", /* tuple */[
        738,
        160
      ], env);
  var j = (match[0] - 10) / 5 | 0;
  if (j >= 0 && j < 8192) {
    Reprocessing_Draw.text(/* None */0, Curry._2(Printf.sprintf(/* Format */[
                  /* Int */Block.__(4, [
                      /* Int_d */0,
                      /* No_padding */0,
                      /* No_precision */0,
                      /* String_literal */Block.__(11, [
                          "Hz - ",
                          /* Float */Block.__(8, [
                              /* Float_f */0,
                              /* No_padding */0,
                              /* No_precision */0,
                              /* End_of_format */0
                            ])
                        ])
                    ]),
                  "%dHz - %f"
                ]), j, spectrum[j]), /* tuple */[
          10,
          10
        ], env);
  }
  var timeWindowSeconds = 8192 / 256;
  Reprocessing_Draw.strokeWeight(1, env);
  for(var i = 1 ,i_finish = Caml_int32.div(8192, timeWindowSeconds | 0) - 1 | 0; i <= i_finish; ++i){
    Reprocessing_Draw.stroke(Reprocessing_Utils.color(0, 0, 0, 255), env);
    var f = i;
    var prev_000 = 10 + (i - 1 | 0) * 2;
    var prev_001 = 150 + data[(f - 1) * timeWindowSeconds | 0][/* re */0] * 100;
    var prev = /* tuple */[
      prev_000,
      prev_001
    ];
    var cur_000 = 10 + i * 2;
    var cur_001 = 150 + data[f * timeWindowSeconds | 0][/* re */0] * 100;
    var cur = /* tuple */[
      cur_000,
      cur_001
    ];
    Reprocessing_Draw.linef(prev, cur, env);
  }
  Reprocessing_Draw.noStroke(env);
  for(var i$1 = 0; i$1 <= 4095; ++i$1){
    var height = spectrum[i$1] * 400;
    if (i$1 === j) {
      Reprocessing_Draw.fill(Reprocessing_Utils.color(255, 255, 255, 255), env);
    } else {
      Reprocessing_Draw.fill(Reprocessing_Utils.color(255 - (spectrum[i$1] * 255 | 0) | 0, spectrum[i$1] * 255 | 0, 0, 255), env);
    }
    Reprocessing_Draw.rectf(/* tuple */[
          10 + i$1 * 5,
          600 - height
        ], 5 - 1, height, env);
  }
  return /* record */[
          /* time */state[/* time */0] + dt,
          /* checked */state[/* checked */1],
          /* unchecked */state[/* unchecked */2],
          /* hamming */state[/* hamming */3],
          /* hann */state[/* hann */4]
        ];
}

Reprocessing.run(setup, /* None */0, /* Some */[draw], /* None */0, /* None */0, /* Some */[mouseDown], /* None */0, /* None */0, /* None */0, /* None */0, /* () */0);

var fftBinSize = 8192;

var frequency = 15.1;

var samplingRate = 8192;

var hammingX = 700;

var hammingY = 100;

var hannX = 700;

var hannY = 160;

export {
  pi ,
  fftBinSize ,
  frequency ,
  samplingRate ,
  setup ,
  hammingX ,
  hammingY ,
  hannX ,
  hannY ,
  mouseDown ,
  prevDeltaTime ,
  draw ,
  
}
/* pi Not a pure module */
