# Source code rearrangement

The source tree layout still reflects the old idea that the Multovl
input data might come not only from files but also from databases,
hence the `file` subdirectory. This is not appropriate any more.
The Multovl application top-level sources are also scattered around
in various subdirectories, and there are 3 support libraries.

## Old source tree

```
src
├── config
├── exercise
├── file
│   ├── prob
│   │   └── test
│   └── test
├── scripts
└── test
    └── data
```

## New source tree

```
src                 <-- applications (*.cc top-level)
├── config
├── exercise
├── incl            <-- header files (*.hh)
│   └── multovl     <-- namespace multovl
│       ├── io      <-- namespace multovl::io; file I/O related routines
│       └── prob    <-- namespace multovl::prob
├── lib             <-- implementation files (*.cc), namespace multovl
│   ├── io          <-- namespace multovl::io implementation files
│   └── prob        <-- namespace multovl::prob implementation files
├── scripts
└── tests           <-- all unit tests together, directory name changed
    └── data
```

The former `movl_fileio` library components will be included
in the `multovl` library. The `multovl::prob` namespace components
will be kept in a separate `multovl_prob` library as not all apps need it.

## Header inclusion changes

### Bulk fixes of the source files

Change the `#include` directives first. Use 

`sed -E -e 's/#include "([a-z]+)\.hh"/#include "multovl\/\1.hh"/'`

to prefix the header name with `multovl/`. For headers from the `multovl::io`
and `multovl::prob` namespaces, change to `multovl/io` or `multovl/prob` accordingly.

In the `lib` and `tests` directory, use:

```
for SRC in *.cc; do sed -E -i.backup -e 's/#include "([a-z]+)\.hh"/#include "multovl\/\1.hh"/' $SRC; done
```

In the `lib/io` directory, use:

```
for SRC in *.cc; do sed -E -i.backup -e 's/#include "([a-z]+)\.hh"/#include "multovl\/io\/\1.hh"/' $SRC; done
```

In the `lib/prob` directory, use:

```
for SRC in *.cc; do sed -E -i.backup -e 's/#include "([a-z]+)\.hh"/#include "multovl\/prob\/\1.hh"/' $SRC; done
```

Of course some of these bulk changes will be incorrect; the idea is to try to build
the library and let the compiler find them. The incorrect `#include`-s will be fixed manually.

In the unit test sources, we just add the `multovl/` prefix.

### Bulk fixes of the header files

Similar procedure as above. Generically,

`for HDR in *.hh; do sed -E -i.backup -e 's/#include "([a-z]+)\.hh"/#include "multovl\/\1.hh"/' $HDR; done`

