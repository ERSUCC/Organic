# Organic

Organic is a programming language for algorithmic audio synthesis, with the goal of providing electronic musicians with
a flexible yet easy-to-learn way to create unique music.

## Generative AI Statement

Organic is a labor of love and passion, conceived of and developed from scratch without the use of generative AI. All
language and code decisions are meaningful and made with care, reflecting the true intent of the project.

## Table of Contents

1. [Using Organic](#using-organic)
2. [Building Organic](#building-organic)
3. [Credits](#credits)

## Using Organic

For usage instructions and language details, see the documentation at
[https://organic-lang.readthedocs.io](https://organic-lang.readthedocs.io).

## Building Organic

This section details the process of building Organic yourself, rather than using one of the released binary files.

Currently the only provided method for building Organic is CMake, which can be found here:
[https://cmake.org/download/](https://cmake.org/download/). Make sure CMake is installed and working properly before
continuing.

Before building Organic, ensure that you have installed Organic's dependencies, `rtaudio`, `libsndfile`, and
`libsamplerate`. You can install the latest versions of `libsndfile` and `libsamplerate` with your preferred package
manager, but you must build `rtaudio` from the latest GitHub source to ensure compatibility with Organic.

Once you have installed the dependencies, download Organic's source code. To build with CMake, run the following
commands:

```
cmake -B build
cmake --build build --config Release
```

This will create the `organic` binary in the `build` directory (Mac/Linux) or the `build/Release` directory (Windows).

## Credits

Organic uses RtAudio for cross-platform real-time audio output. RtAudio can be found here:
[https://github.com/thestk/rtaudio](https://github.com/thestk/rtaudio).

Organic uses libsndfile for audio file input and output. libsndfile can be found here:
[https://github.com/libsndfile/libsndfile](https://github.com/libsndfile/libsndfile).

Organic uses libsamplerate for resampling audio files. libsamplerate can be found here:
[https://github.com/libsndfile/libsamplerate](https://github.com/libsndfile/libsamplerate).

Organic uses CMake to build the source code and link dependencies. CMake can be found here:
[https://cmake.org](https://cmake.org).

Organic uses Sphinx to generate HTML documentation pages. Sphinx can be found here:
[https://www.sphinx-doc.org/en/master/index.html](https://www.sphinx-doc.org/en/master/index.html).
