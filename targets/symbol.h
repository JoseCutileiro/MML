#ifndef __MML_TARGETS_SYMBOL_H__
#define __MML_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace mml
{

  class symbol
  {
    std::string _name; // identifier
    long _value;       // hack!
    bool _constant;    // Is it a constant?
    int _qualifier;    // public, forward, private, etc
    std::shared_ptr<cdk::basic_type> _type;
    bool _initialized; // initialized?
    int _offset = 0;   // 0 means global variable/function
    bool _function;    // false for variables

  public:
    symbol(bool constant, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized,
           bool function) : _name(name), _value(0), _constant(constant), _qualifier(qualifier), _type(type), _initialized(initialized), _function(function)
    {
    }

    virtual ~symbol()
    {
      // EMPTY
    }

    const std::string &name() const
    {
      return _name;
    }
    int value() const
    {
      return _value;
    }
    int value(int v)
    {
      return _value = v;
    }

    bool constant() const
    {
      return _constant;
    }
    int qualifier() const
    {
      return _qualifier;
    }

    std::shared_ptr<cdk::basic_type> type() const
    {
      return _type;
    }
    void set_type(std::shared_ptr<cdk::basic_type> t)
    {
      _type = t;
    }

    void set_qual(int qual)
    {
      _qualifier = qual;
    }

    bool is_typed(cdk::typename_type name) const
    {
      return _type->name() == name;
    }

    const std::string &identifier() const
    {
      return name();
    }
    bool initialized() const
    {
      return _initialized;
    }
    int offset() const
    {
      return _offset;
    }
    void set_offset(int offset)
    {
      _offset = offset;
    }
    bool isFunction() const
    {
      return _function;
    }

    bool global() const
    {
      return _offset == 0;
    }
    bool isVariable() const
    {
      return !_function;
    }
  };

  inline auto make_symbol(bool constant, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name,
                          bool initialized, bool function)
  {
    return std::make_shared<symbol>(constant, qualifier, type, name, initialized, function);
  }

} // mml

#endif
