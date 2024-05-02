# CMSIS-CV Documentation

CMSIS-CV Documentation in HTML format is published online at [https://arm-software.github.io/CMSIS-CV](https://arm-software.github.io/CMSIS-CV).

The version drop-down menu there allows to switch between the documentation provided with official releases and the latest draft documentation for the main branch.

The documentation source is maintained in `Documentation/Doxygen/` folder as a mixture of markdown and doxygen formats. Component source files, templates and examples may also contribute information that gets integrated into the final documentation.

Generating the HTML-formatted documentation from the source is done with `gen_doc.sh` script:

```sh
CMSIS-CV $ ./Documentation/Doxygen/gen_doc.sh
```

The script expects specific version of [doxygen](https://www.doxygen.nl/) to be installed locally. After successful execution the resulting documentation package is then available in `./Documentation/html/`.
