# fft.re

This is a simple Cooley-Tukey FFT implementation ported from https://gist.github.com/mbitsnbites/a065127577ff89ff885dd0a932ec2477.

This works in native and in web using a shim of `Bigarray` that uses `Float64Array` under the hood.

The API has mutable and immutable APIs, for performance or simplicity respectively.

### Example
```reason
let shouldApplyHammingWindow = false;
let samplingRate = 8192;

let data = Fft.generateSine(
  ~frequency=200.,
  ~samplingRate,
  ~size=8192,
  (),
);

/* Make sure to call getMaxAmplitude before applying any window to the data, 
   as the windowing function mutate the array, which would alter the min/max 
   values. */
let maxAmplitude = Fft.getMaxAmplitude(data);

if (shouldApplyHammingWindow) {
  Fft.hammingWindow(~data);
};

let spectrum = Fft.fft(~data, ~maxAmplitude, ~samplingRate);

assert(abs_float(Fft.BA.get(spectrum, 200) -. 1.0) < 0.000001);
```

### API docs

#### fft
Performs an FFT on the given data and returns an array of floats representing the spectrum of the signal. Each index is a frequency in Hz, the value at each index is the amplitude of that frequency. The amplitudes in the spectrum are scaled such that the peak in the time domain has the same amplitude as the peak in the frequency domain.
```reason
let fft = (
  ~data : MyBigarray.t(Complex.t), 
  ~maxAmplitude: float,
  ~samplingRate: float,
) => MyBigarray.t(float);
```

#### generateSine
Helper to generate a sine or sum of sine waves (for a square wave).
```reason
let generateSine = (
  ~frequency : int,
  ~samplingRate=256 : int,
  ~size=256 : int,
  ~offset=0. : float,
  ~numberOfSines=1 : int,
  () : unit
) => MyBigarray.t(Complex.t)
```

#### getMaxAmplitude
Returns the max real component in the given array of data.
This is used to scale the FFT so that the peak in the frequency domain matches the peak in the time domain.
```reason
let getMaxAmplitude = (~data : MyBigarray.t(Complex.t)) => Complex.t
```

#### hammingWindow
Mutates the data passed, applying a Hamming window to it.
`amount` is used to tweak how much to apply the windowing function onto the signal, and is a number between 0 and 1.
```reason
let hammingWindow = (
  ~data : MyBigarray.t(Complex.t), 
  ~amount=1. : float, 
  () : unit
) => unit
```

#### hannWindow
Mutates the data passed, applying a Hann window to it. 
`amount` is used to tweak how much to apply the windowing function onto the signal, and is a number between 0 and 1.
```reason
let hannWindow = (
  ~data : MyBigarray.t(Complex.t)
  ~amount=1. : float,
  () : unit
) => unit
```

#### fftInplace
In place version of the main FFT algorithm. Does not do any scaling of the amplitudes (see `fft` for explanation of the scaling done).
```reason
let fftInplace = (data : MyBigarray.t(Complex.t)) => unit
```


### Benchmarks
The following benchmarks were done in native.

For 2 arrays of floats
- Sampling rate: 256Hz
- FFT bucket size: 256 points
- Iterations: 10,000
- Time taken: 0.000244 sec/iteration (0.244 ms)

Using complex module:
- Sampling rate: 256Hz
- FFT bucket size: 256 points
- Iterations: 10,000
- Time taken: 0.000150 sec/iteration (0.150 ms)

Using complex module:
- Sampling rate: 2048Hz
- FFT bucket size: 2048 points
- Iterations: 500
- Time take: 0.001471 sec/iteration (1.471 ms)

Using complex module:
- Sampling rate: 8192Hz
- FFT bucket size: 8192 points
- Iterations: 500
- Time take: 0.006063 sec/iteration (6.063 ms)

Using complex module (with global and mutable bigarray bucket):
- Sampling rate: 8192Hz
- FFT bucket size: 8192 points
- Iterations: 500
- Time take: 0.005910 sec/iteration (5.91 ms)

Using complex module:
- Sampling rate: 131072Hz
- FFT bucket size: 131072 points
- Iterations: 100
- Time take: 0.108501 sec/iteration (108.501 ms)
