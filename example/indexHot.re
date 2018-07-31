/* @HACK to force complex.cma to be linked into the hotreloaded exec, so that the dynamic linker
   doens't complain about the Complex module not being present. */
Printf.printf(
  "%f\n",
  Complex.(add({re: 1.0, im: 1.0}, {re: 0.1, im: 0.1})).re,
);
Reprocessing.hotreload("src/index.re");
