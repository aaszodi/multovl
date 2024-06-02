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

