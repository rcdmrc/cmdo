# cmdo

A C++ library for accessing command line options.

by Ricardo Mauricio

https://github.com/rcdmrc/cmdo

## Build and Install

```bash
mkdir cmdo-build
cd cmdo-build
cmake <cmdo-src> -DCMAKE_INSTALL_PREFIX=<cmdo-install-dir>

make
make install
```

Tests aren't build by default, run cmake with -DBUILD_TESTS=ON to build unit 
tests. Unit tests use googletest (https://github.com/google/googletest)

## Examples

### Short version

The code:

```c++
#include <cmdo/CmdLineOptions.h>
#include <string>
#include <iostrean>

int main(int argc, char ** argv) {
    cmdo::CmdLineOptions cmdo("Program that shows you how this thing works.");
    cmdo.add_required("-in", "an input file");
    cmdo.add_required("-out", "an output file");
    cmdo.add_switch("-super", "do the work in super mode.", false);
    cmdo.add_optional("-opt1", "this is optional", "5");
    cmdo.parse(&argc, argv);
    
    std::string const& inputFile = cmdo.get_option("-in");    
    std::string const& outputFile = cmdo.get_option("-out");
    if(cmdo.get_switch("-super") {
        // do something in super mode.
    }
}
```

Use:

```bash
example -in in_file.txt -out out_file.txt -super
```

### Adding validators

The code:

```c++
#include <cmdo/CmdLineOptions.h>
#include <unistd.h>
#include <string>
#include <iostrean>

int main(int argc, char ** argv) {
    cmdo::CmdLineOptions cmdo("Program that shows you how this thing works.");
    cmdo.add_required("-in", "an input file");
    cmdo.add_required("-out", "an output file");
    cmdo.add_switch("-super", "do the work in super mode.", false);
    cmdo.add_optional("-opt1", "this is optional", "5");
    cmdo.attach_validator("-in", [](std::string const &name,
                                    std::string const &value) {
        return ::access(value.c_str(), R_OK) == 0;
    });
    
    cmdo.parse(&argc, argv);
    
    std::string const& inputFile = cmdo.get_option("-in");    
    std::string const& outputFile = cmdo.get_option("-out");
    if(cmdo.get_switch("-super") {
        // do something in super mode.
    }
}
```

Use:

```bash
example -in in_file.txt -out out_file.txt
```

In this case cmdo will check that the value of "-in" can be open for reading.