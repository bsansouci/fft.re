import resolve from 'rollup-plugin-node-resolve';

export default {
  input: 'example/index.js',
  output: {
    file: 'bundle.js',
    format: 'iife',
    name: 'MyBundle'
  },
  plugins: [resolve()]
};
