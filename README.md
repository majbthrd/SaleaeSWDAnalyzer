## SW-DP (Serial Wire Debug Port) Analyzer plugin for the Saleae Logic

The SW-DP protocol is described by the following publicly available documents:

[DDI 0316 CoreSight™ DAP-Lite Technical Reference Manual](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0316d/DDI0316D_dap_lite_trm.pdf)

[Programming Internal Flash Over the Serial Wire Debug Interface](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN0062.pdf)

[CY8C41xx, CY8C42xx Programming Specifications](http://www.cypress.com/?docID=48133)

## Building

The Saleae SDK requires Python to script the build of an analyzer; if your computer doesn't have this installed, you are out of luck.

To compile, first download and extract the Saleae SDK:

http://support.saleae.com/hc/en-us/articles/201104644-Analyzer-SDK

This will result in a directory of the form "SaleaeAnalyzerSdk-1.1.x".  Place the "SW-DP" directory from this project into this Saleae directory.

To compile, follow the PDF in "SaleaeAnalyzerSdk-1.1.x/documentation" and the text file "lib/readme.txt".  Consult the Saleae support web site for any issues with the compilation procedure.

The end result of compilation is a library in the "SW-DP/release/" subdirectory.  This file must be copied to the "Analyzers" subdirectory of the Saleae Logic software.

## License

The contents of this repository are released under [LGPL 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).

