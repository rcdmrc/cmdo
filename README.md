# cmdo

[![Build Status](https://travis-ci.org/rcdmrc/cmdo.svg?branch=master)](https://travis-ci.org/rcdmrc/cmdo)

A C++ library for accessing command line options.

https://github.com/rcdmrc/cmdo

## Install

Quick install:

```bash
git clone https://github.com/rcdmrc/cmdo
mkdir cmdo-build
cd cmdo-build
cmake ../cmdo -DCMAKE_INSTALL_PREFIX=<cmdo-install-dir>
make && make install
```

To build unit tests:

```bash
git clone https://github.com/rcdmrc/cmdo

# Initializes tests/thirdparty/googletest
cd cmdo
git submodule init && git submodule update
cd ..

mkdir cmdo-build
cd cmdo-build
cmake ../cmdo -DCMAKE_INSTALL_PREFIX=<cmdo-install-dir> -DBUILD_TESTS=ON
make && make install
```

## Examples

### Basic command line options

```c++
#include <cmdo/CmdLineOptions.h>
#include <string>
#include <iostrean>

int main(int argc, char ** argv) {
    cmdo::CmdLineOptions cmdo("Program that shows you how this thing works.");
    cmdo.add_required("-in", "an input file");
    cmdo.add_switch("-super", "do the work in super mode.", false);
    cmdo.add_optional("-opt1", "this is optional", "5");
    cmdo.parse(&argc, argv);
    
    std::string const& inputFile = cmdo.get_option("-in");    
    std::string const& outputFile = cmdo.get_option("-out");
    if(cmdo.get_switch("-super")) {
        // do something in super mode.
    }
    
    // do some more work ...
}
```
Using this example:

```bash
example -in in_file.txt -opt1 10
```

### Using input validators

```c++
#include <cmdo/CmdLineOptions.h>
#include <unistd.h>
#include <string>
#include <iostrean>

int main(int argc, char ** argv) {
    cmdo::CmdLineOptions cmdo("Program that shows you how this thing works.");
    cmdo.add_required("-in", "an input file");
    cmdo.attach_validator("-in", [](std::string const &name,
                                    std::string const &value) {
        return ::access(value.c_str(), R_OK) == 0;
    });
    
    cmdo.parse(&argc, argv);
    
    std::string const& inputFile = cmdo.get_option("-in");   
    
    // do some more work ...
}
```
Using this example:

```bash
example -in in_file.txt
```

### Error handling

The default behavior is to print any errors to std::cerr, and exit the process (except for unknown options). You can have cmd do something else by setting your own ParserResultHandler.

This is the default implementation of the error handler:
```c++
void handler(StringList const &unknownInput, StringList const &missingOptions,
             StringList const &emptyOptions, StringList const &invalidOptions)
{

  for (std::string const &name : unknownInput) {
    // do something about unknown options
  }

  for (std::string const &name : emptyOptions) {
    // do something about options which require an argument.
  }
  for (std::string const &name : invalidOptions) {
    // do something about options which were set to an invalid value (failed one or more validators).
  }
  for (std::string const &name : missingOptions) {
    // do something about options which are required, but were not provided.
  }
  if (!missingOptions.empty() || !unknownInput.empty()
      || !invalidOptions.empty() || !emptyOptions.empty()) {
    exit(EXIT_FAILURE);
  }
}
```
