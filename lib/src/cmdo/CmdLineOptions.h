#ifndef CMDO_CMDLINEOPTIONS_H
#define CMDO_CMDLINEOPTIONS_H

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include "cmdo/StringUtil.h"

namespace cmdo {

struct BadFunction : public std::exception {

};

struct OptionNotSet : public std::exception {

};

struct UndefinedOption : public std::exception {

};

struct OptionDefined : public std::exception {

};

struct BadOption : public std::exception {

};

/**
 * @brief Allows parsing of command line options, and access to any argument
 * values.
 */
class CmdLineOptions {
public:
  /**
   * @brief Function signature for user-defined validator of argument values.
   * Returns recevies the argument value as std::string, returns true if the
   * value passes the validation test, false if it doesn't.
   */
  typedef std::function<bool(std::string const &, std::string const &)>
      ValidatorFunction;
  typedef std::vector<std::string> StringList;
  /**
   * @brief Function signature for user-defined handlers when errors are
   * found when parsing the command line arguments.
   * (list of unknown options), (list of required options not in argv),
   * (list of argument options with no value), (list of invalid options)
   *
   * An invalid option is one that didn't pass one or more ValidatorFunction
   * test.
   */
  typedef std::function<void(StringList const &, StringList const &,
                             StringList const &, StringList const &)>
      ParserResultHandler;

  /**
   * @brief Initialize with program description. This description is shown
   * in print_usage().
   * @see print_usage
   */
  CmdLineOptions(std::string const &program_description);

  CmdLineOptions(std::string const &program_description,
                 std::string const &additional_args);

  /**
   * @brief Returns the name of the program. The name of the program is
   * collected after calling parse(), until then this returns an empty string.
   * @see parse
   */
  std::string program_name() const;

  /**
   * @brief Returns the description of the program specified in the constructor.
   */
  std::string program_description() const;

  /**
   * @brief Parses the command line for options defined using add_required, add_optional and add_switch. 
   * @param[in] argc Number of command line argments
   * @param[in] argv Command line arguments
   * @param[out] left_overs Things in the command line that were not defined
   * as options in CmdLineOptions. Strings will be added here in the same order
   * they appear in the command line.
   */
  void parse(int argc, char **argv, StringList& left_overs);

  /**
   * @brief Defines a required argument. If the argument is not present in
   * the command line, the name will be added to the list of missing required
   * arguments which is passed to the ParserResultHandler. The default
   * ParserResultHandler prints an error and causes the process to exit.
   * @param[in] name Name of the argument. You should add any option prefixes
   * here. For instance: -my_option, -my-option, etc. No spaces.
   * @param[in] description Description of the option.
   * @throws OptionDefined
   *   If the option is already been defined.
   */
  void add_required(std::string const &name, std::string const &description);

  /**
   * @brief Defines an optional argument. If the argument is not present in
   * the command line, then default_value will be used as its value.
   * @param[in] name Name of the argument. You should add any "option prefixes"
   * here. For instance: -my_option, -my-option, etc. No spaces.
   * @param[in] description Description of the option.
   * @param[in] default_value Default value for the argument in case that its
   * not present in the command line.
   * @throws OptionDefined
   *   If the option is already been defined.
   */
  void add_optional(std::string const &name, std::string const &description,
                    std::string const &default_value);

  /**
   * @brief Defines a switch (can only be true/false). If the switch is found in
   * the command line, its value will be set to !default_setting.
   * @param[in] name Name of the argument. You should add any "option prefixes"
   * here. For instance: -my_option, -my-option, etc. No spaces.
   * @param[in] description Description of the option.
   * @param[in] default_setting Default state of the switch.
   * @throws OptionDefined
   *   If the option is already been defined.
   */
  void add_switch(std::string const &name, std::string const &description,
                  bool default_setting);

  /**
   * @brief Adds a validator for an argument. Validators are called after
   * parsing the command line arguments.
   * @param[in] opt_name Name of the argument.
   * @param[in] validator The validator. Must be a valid function (validator
   * .bool() == true).
   * @see ValidatorFunction
   */
  void attach_validator(std::string const &opt_name,
                        ValidatorFunction validator);

  /**
   * @brief Set a custom handler for results of parsing the command line
   * arguments.
   * @param[in] handler The handler. Must be a valid function
   * (handler.bool() == true).
   * @see ParserResultHandler
   */
  void set_parser_result_handler(ParserResultHandler handler);

  /**
   * @brief Get the value of an argument option.
   * @throws UndefinedOption
   *   If argument was not defined with any of the add_*_arg functions.
   * @throws OptionNotSet
   *   If argument was not set, and has no default value (required arguments).
   */
  std::string get_option(std::string const &name) const;

  /**
   * @brief Like get_option() but casts the argument value to whatever you want.
   * @throws BadCast
   *   If the value cannot be casted to what you want.
   */
  template<typename T>
  T get_option_as(std::string const &opt_name) const;

  /**
   * @brief Get the state of a switch.
   * @throws UndefinedOption
   *   If argument was not defined with add_switch.
   */
  bool get_switch(std::string const &switch_name) const;

  /**
   * @brief Prints simple help on using this program. This contains the
   * description of the program, and the list of all options and their
   * descriptions.
   * @param[out] out the stream on which to print the help.
   */
  void print_usage(std::ostream &out) const;

private:

  template<typename T>
  class Option {
  public:
    Option(std::string const &name, std::string const &description,
           T const &default_value);

    std::string name() const;

    std::string description() const;

    /**
     * @brief Get the value of this option.
     * @throws OptionNotSet
     *   If a value was not set with set().
     */
    T const &get() const;

    void set(T value);

    /**
     * @brief Returns the default value specified in the constructor.
     */
    T const &get_default() const;

    /**
     * @brief Returns true if the value for this option is set with set().
     */
    bool is_set() const;

    /**
     * @brief Returns true if this option is required.
     */
    bool is_required() const;

    void set_required(bool const &required);

  private:
    std::string name_;
    std::string description_;
    bool isSet_;
    bool isRequired_;
    T value_;
    T defaultValue_;
  };

  typedef Option<std::string> StringOption;
  typedef std::vector<StringOption> ArgOptList;
  typedef Option<bool> BoolOption;
  typedef std::vector<BoolOption> SwitchOptList;
  typedef std::vector<ValidatorFunction> ValidatorFunctionList;
  typedef std::map<std::string, ValidatorFunctionList> ValidatorFunctionMap;

  bool is_switch(std::string const &name) const;

  bool is_arg(std::string const &name) const;

  ArgOptList::const_iterator find_arg(std::string const &name) const;

  ArgOptList::iterator find_arg(std::string const &name);

  SwitchOptList::const_iterator find_switch(std::string const &name) const;

  SwitchOptList::iterator find_switch(std::string const &name);

  class ErrorPrinter {
  public:
    ErrorPrinter(std::ostream &out);

    ~ErrorPrinter();

    template<typename T>
    std::ostream &operator<<(T const &data);

  private:
    std::ostream &out_;
  };

  // The -h switch is added by default.
  static std::string const HELP_SWITCH_NAME;

  std::mutex mutex_;
  ArgOptList argOptionList_;
  SwitchOptList switchOptionList_;
  std::string programName_;
  std::string programDescription_;
  ValidatorFunctionMap validatorFunctionMap_;
  std::ostream &errorStream_;
  std::ostream &stdStream_;
  ParserResultHandler parserResultHandler_;
};

template<typename T>
T CmdLineOptions::get_option_as(std::string const &opt_name) const {
  std::string const &v = get_option(opt_name);
  return from_string<T>(v);
}

template<typename T>
CmdLineOptions::Option<T>::Option(std::string const &name,
                                  std::string const &description,
                                  T const &default_value)
    : name_(name), description_(description),
      isSet_(false), isRequired_(false), value_(default_value),
      defaultValue_(default_value) {
  if (name_.empty()) {
    throw BadOption();
  }
}

template<typename T>
std::string CmdLineOptions::Option<T>::name() const {
  return name_;
}

template<typename T>
std::string CmdLineOptions::Option<T>::description() const {
  return description_;
}

template<typename T>
bool CmdLineOptions::Option<T>::is_set() const {
  return isSet_;
}

template<typename T>
bool CmdLineOptions::Option<T>::is_required() const {
  return isRequired_;
}

template<typename T>
T const &CmdLineOptions::Option<T>::get() const {
  if (is_required() && !is_set()) {
    throw OptionNotSet();
  }

  return value_;
}

template<typename T>
void CmdLineOptions::Option<T>::set(T value) {
  value_ = value;
  isSet_ = true;
}

template<typename T>
void CmdLineOptions::Option<T>::set_required(bool const &required) {
  isRequired_ = required;
}

template<typename T>
T const &CmdLineOptions::Option<T>::get_default() const {
  return defaultValue_;
}

template<typename T>
std::ostream &CmdLineOptions::ErrorPrinter::operator<<(const T &data) {
  return out_ << data;
}

}

#endif //CMDO_CMDLINEOPTIONS_H
