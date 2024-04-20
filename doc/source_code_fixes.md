# Various source code fixes

## Detabbing

2024-04-20

Some source files contained TAB characters probably due to editor misconfiguration.
Detect them (on Linux) with:

`find src -type f \( -name '*.hh' -o -name '*.cc' \) -exec grep -l '\\t' {} \;`

Produces the following list:

```
src/exercise/inputfiles.cc
src/file/test/linewritertest.cc <- false+ ?
src/file/test/linereadertest.cc <- false+ ?
src/file/linewriter.cc
src/file/prob/stat.hh <- false+ ?
src/file/prob/empirdistr.hh 
src/file/linereader.cc
```

Tried to detab using 

`find src -type f \( -name '*.hh' -o -name '*.cc' \) -exec bash -c 'expand -i -t 4 {} | sponge {}' \;`

but this changed too many things, seems to be unreliable for some reason
(or I didn't understand how this works), at the end I detabbed with BBedit manually,
checking each file in turn.
