## SW-DP (Serial Wire Debug Port) Analyzer plugin for the Saleae Logic

The SW-DP protocol is described by the following publicly available documents:

[DDI 0316 CoreSight™ DAP-Lite Technical Reference Manual](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0316d/DDI0316D_dap_lite_trm.pdf)

[Programming Internal Flash Over the Serial Wire Debug Interface](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN0062.pdf)

[CY8C41xx, CY8C42xx Programming Specifications](http://www.cypress.com/?docID=48133)

## Background

This was written in 2013, long before Saleae started bundling their own "SWD" analyzer.  It doesn't have the fancy "Decoded Protocols" window that Saleae added in subsequent years (you must use the "Export as text/csv" option), but I continue to find that this analyzer decodes transactions that the Saleae offering seems unable to.

## Building

The Saleae SDK requires Python to script the build of an analyzer; if your computer doesn't have this installed, you are out of luck.

After downloading this project's code, issue the following command to download the SDK files from Saleae's github page:

```
git submodule update AnalyzerSDK
```

Read the file ./AnalyzerSDK/lib/readme.txt and follow its instructions.  It will direct you to rename libAnalyzer64.so to libAnalyzer.so if you are running 64-bit Linux.

To compile, run the following from the base directory of this project:

```
python build_analyzer.py
```

Consult the Saleae support web site and their [SampleAnalyzer](https://github.com/saleae/SampleAnalyzer) for any issues with the compilation procedure.

The end result of compilation is a library in the "SW-DP/release/" subdirectory.  This file must be copied to the "Analyzers" subdirectory of the Saleae Logic software.

## License

The contents of this repository are released under [LGPL 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).

