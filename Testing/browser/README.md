# README

Test of CMSIS-CV algorithms using CMSIS-DSP and CMSIS-Stream running in the browser.

The same CMSIS-Stream nodes can used for both a board (HiMax Seeed grove for instance) and the web.

The web is using the scalar version of the algorithm. The board the Helium one.

## How to run 

Open `web/index.html`.

It may take a few seconds to launch the video. You may have to click once in the window if it does not start.

It will use the first camera it finds ...

## How to build

Clone this repository.

Clone CMSIS-DSP repository

Install `emsdk`: https://emscripten.org/docs/getting_started/downloads.html

After install, each time you want to build, you'll need to activate emsdk (it sets some environment variables).

Go to folder build

`create.bat` to regenerate the `Makefile` from `cmake`

Paths must be changed in `create.bat` and use absolute paths.

```
OpenCV_DIR must point to the cmake folder inside the opencv that is inside this project.
```

`make`

You should have new `stream.js` generated in `build` and copied to `web`.

## Creating new examples:

Check latest version (at least `v.1.14.0`) of CMSIS-Stream is installed:

```shell
pip install cmsis-stream
```

Type

```shell
python create_web.py
```

to regenerate the examples. (You need to rebuild after this)

To see how to add a new example, look at the `README` in `stream/tests/template`
