# cmdo

[![Build Status](https://travis-ci.org/rcdmrc/cmdo.svg?branch=master)](https://travis-ci.org/rcdmrc/cmdo)

A C++ library for accessing command line options.

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
In this case cmdo will check that the value of "-in" can be open for reading.

Use:

```bash
example -in in_file.txt -out out_file.txt
```

### Error handling

The default behavior is to print any errors to std::cerr, and exit the process (except for unknown options). You can have cmd do something else by setting your own ParserResultHandler.

This is the default implementation of the error handler:
```c++
void handler(StringList const &unknownInput, StringList const &missingOptions,
             StringList const &emptyOptions, StringList const &invalidOptions)
{

  for (std::string const &name : unknownInput) {
    ErrorPrinter(errorStream_) << "unknown option: " + name;
  }

  for (std::string const &name : emptyOptions) {
    ErrorPrinter(errorStream_) << "option requires an argument: " << name;
  }
  for (std::string const &name : invalidOptions) {
    ErrorPrinter(errorStream_) << "invalid argument: "
                               << name << " = " << this->get_option(name);
  }
  for (std::string const &name : missingOptions) {
    ErrorPrinter(errorStream_) << "option is required: " << name;
  }
  if (!missingOptions.empty() || !unknownInput.empty()
      || !invalidOptions.empty() || !emptyOptions.empty()) {
    exit(EXIT_FAILURE);
  }
}
```
