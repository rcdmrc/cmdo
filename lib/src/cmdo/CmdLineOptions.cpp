#include "cmdo/CmdLineOptions.h"
#include <iostream>
#include <iomanip>

namespace cmdo {

std::string const CmdLineOptions::HELP_SWITCH_NAME{"-h"};

CmdLineOptions::CmdLineOptions(std::string const &program_description)
    : CmdLineOptions(program_description, "") {
}

CmdLineOptions::CmdLineOptions(std::string const &program_description,
                   std::string const &additional_args)
    : programDescription_(program_description), errorStream_(std::cerr),
      stdStream_(std::cout), parserResultHandler_() {
  add_switch(HELP_SWITCH_NAME, "Show program help.", false);

  // Default fail function.
  parserResultHandler_ = [this](StringList const &unknownInput,
                                StringList const &missingOptions,
                                StringList const &emptyOptions,
                                StringList const &invalidOptions) {
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
  };
}

void CmdLineOptions::parse(int *argc, char **argv) {
  std::unique_lock<std::mutex> l(mutex_);
  auto get_nice_program_name = [argv]() -> std::string {
    std::string result = std::string(argv[0]);
    // Remove everything but the command's name.
    size_t const pos = result.find_last_of("/");
    if (pos != result.npos) {
      if (pos + 1 < result.size()) {
        result = result.substr(pos);
      }
    }
    return result;
  };
  programName_ = get_nice_program_name();

  StringList listOfUnknownOptions;
  StringList listOfOptionsWithNoValue;
  int const n(*argc);
  for (int i(1); i < n; ++i) {
    int const next = (i + 1) < n ? i + 1 : -1;

    std::string arg(argv[i]);
    {
      SwitchOptList::iterator it = find_switch(arg);
      if (it != switchOptionList_.end()) {
        it->set(!it->get_default());
        continue;
      }
    }
    {
      ArgOptList::iterator it = find_arg(arg);
      if (it != argOptionList_.end()) {
        if (next != -1) {
          it->set(argv[next]);
          ++i;
          continue;
        } else {
          listOfOptionsWithNoValue.push_back(arg);
          continue;
        }
      }
    }

    // not an option :/
    listOfUnknownOptions.push_back(arg);
  }

  // check for the help switch.
  if (get_switch(HELP_SWITCH_NAME)) {
    print_usage(stdStream_);
    exit(EXIT_SUCCESS);
  }

  StringList listOfMissingRequiredOptions;
  StringList listOfInvalidOptions;
  for (StringOption const &option : argOptionList_) {
    if (!option.is_set() && option.is_required()) {
      listOfMissingRequiredOptions.push_back(option.name());
    } else {
      // Validate the argument
      ValidatorFunctionMap::const_iterator it = validatorFunctionMap_.find(
          option.name());
      if (it != validatorFunctionMap_.end()) {
        ValidatorFunctionList const &list(it->second);
        for (ValidatorFunction const &validator : list) {
          if (!validator(option.name(), option.get())) {
            listOfInvalidOptions.push_back(option.name());
          }
        }
      }
    }
  }

  parserResultHandler_(listOfUnknownOptions, listOfMissingRequiredOptions,
                       listOfOptionsWithNoValue, listOfInvalidOptions);
}

void CmdLineOptions::add_required(std::string const &name,
                                  std::string const &description) {
  std::unique_lock<std::mutex> l(mutex_);

  if(is_arg(name) || is_switch(name)){
    throw OptionDefined();
  }
  std::string niceName(name);
  trim(niceName);

  StringOption option(niceName, description, "");
  option.set_required(true);
  argOptionList_.push_back(option);
}

void CmdLineOptions::add_optional(std::string const &name,
                                  std::string const &description,
                                  std::string const &default_value) {
  std::unique_lock<std::mutex> l(mutex_);
  if(is_arg(name) || is_switch(name)){
    throw OptionDefined();
  }
  std::string niceName(name);
  trim(niceName);
  argOptionList_.push_back(StringOption(niceName, description, default_value));
}

void CmdLineOptions::add_switch(std::string const &name,
                          std::string const &description,
                          bool default_setting) {
  std::unique_lock<std::mutex> l(mutex_);
  if(is_arg(name) || is_switch(name)){
    throw OptionDefined();
  }
  std::string niceName(name);
  trim(niceName);

  switchOptionList_.push_back(BoolOption(niceName, description, default_setting));
}

void CmdLineOptions::attach_validator(std::string const &arg_name,
                                CmdLineOptions::ValidatorFunction validator) {
  std::unique_lock<std::mutex> l(mutex_);
  if (!is_arg(arg_name)) {
    throw UndefinedOption();
  }
  if(!validator) {
    throw BadFunction();
  }

  ValidatorFunctionMap::iterator it = validatorFunctionMap_.find(arg_name);
  if (it != validatorFunctionMap_.end()) {
    it->second.push_back(validator);
  } else {
    ValidatorFunctionList list;
    list.push_back(validator);
    validatorFunctionMap_.insert(std::make_pair(arg_name, list));
  }
}

std::string CmdLineOptions::get_option(std::string const &name) const {
  ArgOptList::const_iterator it = find_arg(name);
  if (it != argOptionList_.end()) {
    return it->get();
  }
  throw UndefinedOption();
}

bool CmdLineOptions::get_switch(std::string const &switch_name) const {
  SwitchOptList::const_iterator found = find_switch(switch_name);
  if (found != switchOptionList_.end()) {
    return found->get();
  }
  return false;
}

template<typename T>
void printOptionList(std::ostream &out, std::vector<T> const &option_list,
                     bool const &is_switch = false) {
  for (T const &option: option_list) {
    std::stringstream desc;
    desc << option.description();
    if (!option.is_required()) {
      desc << " (def = " << to_string(option.get_default());
      if (option.is_set()) {
        desc << ", curr = " << option.get();
      }
      desc << ")";
    } else {
      desc << "(required";

      if (option.is_set()) {
        desc << ", curr = " << option.get();
      }
      desc << ")";
    }

    std::string const name = is_switch ? option.name() : option.name() + " [...]";
    out << " "
    << std::setfill(' ')
    << std::setw(2)
    << std::setfill(' ')
    << std::setw(20) << std::left
    << name
    << std::setfill(' ')
    << std::setw(40) << std::left
    << desc.str();
    out << "\n";
  }
}

void CmdLineOptions::print_usage(std::ostream &out) const {
  out << "Usage: " << programName_ << " [options]" << "\n";
  if (!programDescription_.empty()) {
    out << "Description: \n"
    << " " << programDescription_ <<
    "\n\n";
  }

  out << "Available options:\n";
  printOptionList(std::cout, switchOptionList_, true);

  printOptionList(std::cout, argOptionList_);

  out << "\n";
}

bool CmdLineOptions::is_switch(std::string const &name) const {
  SwitchOptList::const_iterator it = find_switch(name);
  return it != switchOptionList_.end();
}

bool CmdLineOptions::is_arg(std::string const &name) const {
  ArgOptList::const_iterator it = find_arg(name);
  return it != argOptionList_.end();
}

CmdLineOptions::ArgOptList::const_iterator CmdLineOptions::find_arg(
    std::string const &name) const {
  return std::find_if(argOptionList_.begin(), argOptionList_.end(),
                      [name](StringOption const &option) {
                        return name == option.name();
                      });
}

CmdLineOptions::ArgOptList::iterator CmdLineOptions::find_arg(std::string const &name) {
  return std::find_if(argOptionList_.begin(),
                      argOptionList_.end(),
                      [name](StringOption const &option) {
                        return name == option.name();
                      });
}

CmdLineOptions::SwitchOptList::const_iterator CmdLineOptions::find_switch(
    std::string const &name) const {
  return std::find_if(switchOptionList_.begin(), switchOptionList_.end(),
                      [name](BoolOption const &option) {
                        return name == option.name();
                      });
}

CmdLineOptions::SwitchOptList::iterator CmdLineOptions::find_switch(
    std::string const &name) {
  return std::find_if(switchOptionList_.begin(), switchOptionList_.end(),
                      [name](BoolOption const &option) {
                        return name == option.name();
                      });
}

CmdLineOptions::ErrorPrinter::ErrorPrinter(std::ostream &out)
    : out_(out) {
  out_ << "* ";
}

CmdLineOptions::ErrorPrinter::~ErrorPrinter() {
  out_ << "\n";
}

void CmdLineOptions::set_parser_result_handler(
    CmdLineOptions::ParserResultHandler handler) {
  if (handler) {
    parserResultHandler_ = handler;
    return;
  }
  throw BadFunction();
}

std::string CmdLineOptions::program_name() const {
  return programName_;
}

std::string CmdLineOptions::program_description() const {
  return programDescription_;
}

}