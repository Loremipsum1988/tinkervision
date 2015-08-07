/*
Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
Copyright (C) 2014-2015 philipp.kroos@fh-bielefeld.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef MODULE_H
#define MODULE_H

#include <iostream>  // for development

#include "tinkervision_defines.h"
#include "image.hh"
#include "bitflag.hh"
#include "logger.hh"

namespace tfv {

class Module {
public:
    enum class Tag : unsigned {
        // static tags
        None = 0x00,
        Executable = 0x01,
        Fx = 0x02,
        Analysis = 0x04,
        Output = 0x08,

        // runtime tags
        ExecAndRemove = 0x10,
        ExecAndDisable = 0x20,
        Removable = 0x30,
        Sequential = 0x40
    };

private:
    std::string type_;
    bool active_;
    Module::Tag tags_;

protected:
    TFV_Int module_id_;

    Module(TFV_Int module_id, std::string type, Tag tags)
        : type_{type}, active_{true}, tags_{tags}, module_id_{module_id} {
        Log("MODULE", "Constructing module ", module_id);
    }

    Module(TFV_Int module_id, std::string type)
        : Module(module_id, type, Tag::None) {}

    virtual void execute(tfv::Image const& image) = 0;

    bool in_range(TFV_Word value, TFV_Word low, TFV_Word high) const {
        return value >= low and value <= high;
    }

public:
    virtual ~Module(void) = default;

    // No copy allowed
    Module(Module const& other) = delete;
    Module(Module&& other) = delete;
    Module& operator=(Module const& rhs) = delete;
    Module& operator=(Module&& rhs) = delete;

    TFV_Int id(void) const { return module_id_; }

    virtual bool running(void) const noexcept { return enabled(); }

    bool enabled(void) const noexcept { return active_; }

    // return false if previous state was the same
    bool enable(void) noexcept { return switch_active(true); }

    // return false if previous state was the same
    bool disable(void) noexcept { return switch_active(false); }

    // return false if previous state was the same
    bool switch_active(bool to_state) {
        Log("MODULE", to_state ? "Enabling " : "Disabling ", "module ",
            module_id_, " (was ", active_ ? "active)" : "inactive)");
        auto was_active = active_;
        active_ = to_state;
        return not(was_active == active_);
    }

    void exec(tfv::Image const& image) {
        // Log("MODULE::Execute", this);
        execute(image);
    }
    virtual ColorSpace expected_format(void) const = 0;

    virtual bool has_parameter(std::string const& parameter) const {
        return false;
    }

    virtual bool set(std::string const& parameter, TFV_Word value) {
        return false;
    }
    virtual TFV_Word get(std::string const& parameter) { return 0; }

    bool set_parameter(std::string const& parameter, TFV_Word value) {
        return set(parameter, value);
    }

    void get_parameter(std::string const& parameter, TFV_Word& value) {
        value = get(parameter);
    }

    Tag const& tags(void) const { return tags_; }
    void tag(Tag tags) { tags_ |= tags; }
};

// helper for compile time check
template <typename T>
struct false_for_type : std::false_type {};

// Interface methods to be implemented by modules

template <typename T, typename... Args>
bool valid(Args&... args) {

    // compiler message if this method is undefined for a module
    static_assert(false_for_type<T>::value, "Undefined interface valid");

    // will never occur
    return false;
}

template <typename T, typename... Args>
void set(T* module, Args... args) {

    // compiler message if this method is undefined for a module
    static_assert(false_for_type<T>::value, "Undefined interface set");
}

template <typename T, typename... Args>
void get(T const& module, Args&... args) {

    // compiler message if this method is undefined for a module
    static_assert(false_for_type<T>::value, "Undefined interface get");
}

#define DECLARE_EMPTY_INTERFACE(Classname) \
    template <>                            \
    bool valid<Classname>(void);           \
    template <>                            \
    void get(Classname const& sh);         \
    template <>                            \
    void set(Classname* sh);

#define IMPLEMENT_EMPTY_INTERFACE(Classname)   \
    template <>                                \
    bool tfv::valid<tfv::Classname>(void) {    \
        return true;                           \
    }                                          \
    template <>                                \
    void tfv::get(tfv::Classname const& sh) {} \
    template <>                                \
    void tfv::set<tfv::Classname>(tfv::Classname * sh) {}
}
#endif
