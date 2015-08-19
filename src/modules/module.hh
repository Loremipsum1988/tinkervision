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

#include <typeinfo>
#include <type_traits>
#include <cassert>

#include "tinkervision_defines.h"
#include "image.hh"
#include "bitflag.hh"
#include "logger.hh"

namespace tfv {
struct Result {
    virtual ~Result(void) = default;
};

struct StringResult : public Result {
    std::string result = "";
    StringResult(void) = default;
    StringResult(std::string const& s) : result(s) {}
};

struct ScalarResult : public Result {
    TFV_Size scalar = 0;
    ScalarResult(void) = default;
    ScalarResult(TFV_Size i) : scalar(i) {}
};

struct PointResult : public Result {
    TFV_Size x = 0;
    TFV_Size y = 0;
    PointResult(void) = default;
    PointResult(TFV_Size x, TFV_Size y) : x(x), y(y) {}
};

struct RectangleResult : public Result {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    RectangleResult(void) = default;
    RectangleResult(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}
};

bool is_compatible_callback(Result const* result, TFV_CallbackPoint const&);
bool is_compatible_callback(Result const* result, TFV_CallbackValue const&);
bool is_compatible_callback(Result const* result, TFV_CallbackRectangle const&);
bool is_compatible_callback(Result const* result, TFV_CallbackString const&);

class Executable;
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

    Executable* executable_ = nullptr;

public:
    TFV_Int module_id_;
    // deprecated
    Module(TFV_Int module_id, std::string type, Tag tags)
        : type_{type}, active_{true}, tags_{tags}, module_id_{module_id} {
        Log("MODULE", "Constructing module ", module_id);
    }

    // deprecated
    Module(TFV_Int module_id, std::string type)
        : Module(module_id, type, Tag::None) {}

    Module(Executable* executable) : executable_(executable) {
        assert(executable_ != nullptr);
    }

    virtual void execute(tfv::Image const& image);
    virtual void execute_modifying(tfv::Image& image);
    virtual bool modifies_image(void) const;

public:
    virtual ~Module(void) { Log("MODULE::Destructor", type_); }

    // No copy allowed
    Module(Module const& other) = delete;
    Module(Module&& other) = delete;
    Module& operator=(Module const& rhs) = delete;
    Module& operator=(Module&& rhs) = delete;

    TFV_Int id(void) const { return module_id_; }
    std::string const& name(void) const { return type_; }

    bool running(void) const noexcept;

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

    void exec(tfv::Image& image) {
        if (modifies_image()) {
            execute_modifying(image);
        } else {
            execute(image);
        }
    }
    virtual ColorSpace expected_format(void) const;

    virtual bool has_parameter(std::string const& parameter) const;

    virtual bool set(std::string const& parameter, TFV_Word value);

    virtual TFV_Word get(std::string const& parameter);

    bool set_parameter(std::string const& parameter, TFV_Word value) {
        return set(parameter, value);
    }

    void get_parameter(std::string const& parameter, TFV_Word& value) {
        value = get(parameter);
    }

    virtual Result const* get_result(void) const;

    Tag const& tags(void) const;
    void tag(Tag tags);

    Executable* executable(void) { return executable_; }
};

class Executable {
private:
    std::string type_;
    bool active_{false};
    Module::Tag tags_;

protected:
    TFV_Int module_id_;

public:
    Executable(TFV_Int module_id, std::string type, tfv::Module::Tag tags)
        : type_{type}, active_{true}, tags_{tags}, module_id_{module_id} {
        Log("EXECUTABLE", "Constructing  ", module_id);
    }

    Executable(TFV_Int module_id, std::string type)
        : Executable(module_id, type, Module::Tag::None) {}

    virtual ~Executable(void) { Log("EXECUTABLE::Destructor", type_); }
    virtual void execute(tfv::Image const& image) {
        LogError("MODULE", "execute called");
    }
    virtual void execute_modifying(tfv::Image& image) {
        LogError("MODULE", "execute_modifying called");
    }

    virtual bool modifies_image(void) const { return false; }
    virtual Result const* get_result(void) const { return nullptr; }
    virtual ColorSpace expected_format(void) const = 0;

    virtual bool has_parameter(std::string const& parameter) const {
        return false;
    }

    virtual bool set(std::string const& parameter, TFV_Word value) {
        return false;
    }
    virtual TFV_Word get(std::string const& parameter) { return 0; }

    virtual bool running(void) const noexcept { return true; }

    Module::Tag const& tags(void) const { return tags_; }
    void tag(Module::Tag tags) { tags_ |= tags; }
};
}
#define DECLARE_API_MODULE(name)                                           \
    extern "C" tfv::Executable* create(TFV_Int id, tfv::Module::Tag tags); \
    extern "C" void destroy(tfv::name* module);

#define DEFINE_API_MODULE(name)                                             \
    extern "C" tfv::Executable* create(TFV_Int id, tfv::Module::Tag tags) { \
        return new tfv::name(id, tags);                                     \
    }                                                                       \
    extern "C" void destroy(tfv::name* module) { delete module; }

#endif
