# CMSIS-CV

![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/ARM-software/CMSIS-CV?include_prereleases) ![GitHub](https://img.shields.io/github/license/ARM-software/CMSIS-CV)

[![C Tests](https://github.com/ARM-software/CMSIS-CV/actions/workflows/runtest.yaml/badge.svg)](https://github.com/ARM-software/CMSIS-CV/actions/workflows/runtest.yaml)


## About

CMSIS-CV is an optimized computer vision library for embedded systems.

It provides optimized compute kernels for Cortex-M.

Different variants are available according to the core and most of the functions are using a vectorized version when the Helium or Neon extension is available.

CMSIS-CV is licensed under [Apache License 2.0](LICENSE).

**It is a work in progress and has just started. API is not yet stable and will evolve for a few versions.**

# Testing

Two kind of tests are provided:

* web : You can test the scalar versions of the kernels in the browser and compare with OpenCV.
* board: Test framework to run on a FVP or MPS3 board

For the web version, clone the repository and just open `Testing/browser/web/index.html` with your browser.



## Support / Contact

For any questions or to reach the CMSIS-CV  team, please create a new issue in https://github.com/ARM-software/CMSIS-CV/issues
