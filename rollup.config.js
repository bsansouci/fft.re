import resolve from 'rollup-plugin-node-resolve';

export default {
  input: 'lib/es6/src/fft.js',
  output: {
    file: 'bundle.js',
    format: 'iife'
  },
  name: 'MyModule',
  plugins: [resolve()]
};
