# ProteoWizard

This is copy of the ProteoWizard code used to learn and explore the
project. See the official ProteoWizard
[page](http://proteowizard.sourceforge.net/) for details. The orignal
code is available at http://proteowizard.sourceforge.net.

## License

Apache License Version 2.0 - see [LICENSE](./LICENSE) for details.

## Installation and compilation

```
git clone git@github.com:lgatto/proteowizard.git
cd proteowizard
./quickbuild.sh
```

```
$cat BUILDING 
BUILDING PROTEOWIZARD

Run "quickbuild.sh" to build ProteoWizard. (Use "quickbuild.bat" for Windows.) 

Use the "--help" switch for some useful hints on building ProteoWizard.

Open doc/index.html in a browser for more detailed info on
ProteoWizard, known issues, etc.
```

## Technical documentation

See [./pwiz/doc/technical/index.html]

### Directory Layout

```
[root]
    build   [everything is built here]
    doc     [documentation]
    example_data [some example data files]
    libraries    [3rd party library archives]
    pwiz       [main source tree -- all Apache licensed]
    pwiz_aux   [non-Apache licensed contributed code]
    pwiz_tools [source code for tools]
```


### Projects

Here is an outline of the various ProteoWizard projects, organized by
dependency level.  There may be dependencies within a given level, but
there should never be any up-level dependencies.  Unless otherwise
noted, all projects are cross-platform.  Each project's source files
are contained in the subdirectory of pwiz of the same name.

#### level 0 (pwiz/utility)
- math:  Mathematics classes (linear algebra, statistics, special mathematical functions)
- misc:  Miscellaneous standalone utility classes (Base64, SHA-1, 2D drawing, unit testing)
- minimxml:  XML parsing and writing
- proteome:  Chemical formula, peptide, and isotope calculations.
- vendor_api:  Vendor-specific API wrappers (Windows only)

#### level 1 (pwiz/data):
- msdata:  Mass spec file format abstraction layer.
- ident: identification file format abstraction layer.
- misc: Library containing classes for handling FT transient data,
  complex frequency data, MS1 peak data.
- vendor_readers:  Vendor-specific Reader implementations

#### level 2 (pwiz/analysis):
 - chromatogram_processing:  Chromatogram analysis
 - frequency:  Library of routines for frequency-domain peak detection.
 - passive:  Event-driven analysis modules
 - peakdetect:  General interface for peak detection
 - peptideid:  Modules handling peptide id info abstraction and parsing
 - spectrum_processing:  Spectrum analysis

#### level 3 (pwiz_tools):
 - commandline:  Command-line tools
 - SeeMS:  Graphic data visualization program (Windows only)

### Code Conventions

A code module consists of an interface (`Foo.hpp`), implementation
(`Foo.cpp`), and a unit test (`FooTest.cpp`).  The interface should be
self-documenting, with optional inclusion of comment markup for
automated documentation tools (e.g. Doxygen).  The unit test serves
two purposes:

- To exercise the module's interface and validate its behavior
  independent of other modules.
- To document the intended usage of the code module.

Clients of a code module should never need to look at the
implementation for questions about usage.

### ProteoWizard Data Access Layer Design

The ProteoWizard data access layer library is `pwiz/msdata`, and the
interface and data structure definitions are in `MSData.hpp`.

The data model is a one-to-one translation from mzML data elements to
C++ structs. The root mzML element correspondes to an `MSData` struct,
and the sub-elements correspond to structs with similar
names. SpectrumList has a virtual interface, which allows for lazy
evaluation backed by a data file.

The mzML controlled vocabulary (CV) is parsed at compile time,
generating `cv.hpp` and `cv.cpp`. This allows CV terms to be used in a
typesafe manner, and also makes the various CV relations and synonyms
available to C++ client code.

Mapping from the various structs to mzML is done in the IO module, and
diff calculations in Diff. `Serializer_mzML` and `Serializer_mzXML`
allow serializations to/from iostreams in mzML and mzXML formats,
respectively.

`MSDataFile` is a subclass of `MSData` that adds file I/O
handling. `MSDataFile::Reader` provides a generic interface for file
readers. By default, Readers for mzML, mzXML, and Thermo RAW files are
provided. On instantiation with a filename, MSDataFile finds a Reader
that will accept the file, and uses that Reader to fill in the
internal data structures.

## Sandbox

`./pwiz/sandox` with code initially from `./pwiz/doc/technical/hello_pwiz`.

## Notes

### `identdata`

- `IdentData`: `mzIdentML` structure.
- `IdentDataFile`: `IdentData` object plus file I/O.
- `Reader`: interface for file readers.
- `DefaultReaderList`: reader classes for mzid, pepXML, protXML.
- `MascotReader`: Mascot file reader.
-  `Serializer_*`: write `mzid`, `pepXML`, `protXML` and `Text` (tab
   delimited) files.
- `examples`: generate example files.
- `IO`: XML read/write.
- `References`: functions for resolving references from objects into
  the internal MSData lists
- `Pep2MzIdent`: Translates data from a `MinimumPepXML` object into a
  `IdentData` object tree when a translation is known.

- Other: `DelimReader`, `DelimWriter`, `Diff`, `KwCVMap`,
  `MzidPredicates`, `TextWriter`, `Version`.

