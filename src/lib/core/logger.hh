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

/** \file logger.hh
    \brief Debugging tool
*/

#ifndef LOGGER_H
#define LOGGER_H

#if !defined(WITH_LOGGER)
#include <string>
namespace tfv {

template <typename... Args>
void Log(std::string const& prefix, Args const&... args) {}
template <typename... Args>
void LogError(std::string const& prefix, Args const&... args) {}
template <typename... Args>
void LogWarning(std::string const& prefix, Args const&... args) {}

#else

#include <fstream>
#include <iostream>
#include <bitset>
#include <chrono>
#include <iomanip>

#include "image.hh"

namespace tfv {

// forward, for ostream declarations
class Module;
class SceneTree;

class Logger {
private:
    static std::string PREFIX_WARNING;
    static std::string PREFIX_ERROR;

    std::chrono::time_point<std::chrono::steady_clock> zero_{
        std::chrono::steady_clock::now()};
    std::string logfilename_ = "/tmp/tfv.log";
    std::ofstream logfile_;

    Logger(void) {
        logfile_.open(logfilename_, std::ios::out | std::ios::trunc);
    }

    ~Logger(void) { logfile_.close(); }

    template <typename T, typename... Args>
    void _print_out(T const& value, Args const&... args) {
        logfile_ << value;
        _print_out(args...);
    }

    template <typename T>
    void _print_out(T const& value) {
        logfile_ << value << std::endl;
    }

    void _print_out(void) { logfile_ << std::endl; }

public:
    template <typename... Args>
    void log_default(std::string prefix, Args const&... args) {
        if (not logfile_.is_open()) {
            return;
        }

        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - zero_).count();

        logfile_ << std::setw(8) << std::setfill('0') << now << "::" << prefix
                 << ": ";
        _print_out(args...);
    }

    template <typename... Args>
    void log_warning(std::string prefix, Args const&... args) {
        log_default(Logger::PREFIX_WARNING + "::" + prefix, args...);
    }

    template <typename... Args>
    void log_error(std::string prefix, Args const&... args) {
        log_default(Logger::PREFIX_ERROR + "::" + prefix, args...);
    }

    static Logger& instance(void) {
        static Logger logger;
        return logger;
    }
};

template <typename... Args>
void Log(std::string const& prefix, Args const&... args) {
    Logger::instance().log_default(prefix, args...);
}

template <typename... Args>
void LogError(std::string const& prefix, Args const&... args) {
    Logger::instance().log_error(prefix, args...);
}

template <typename... Args>
void LogWarning(std::string const& prefix, Args const&... args) {
    Logger::instance().log_warning(prefix, args...);
}

std::ostream& operator<<(std::ostream& os, Module* module);
std::ostream& operator<<(std::ostream& os, SceneTree const& tree);
std::ostream& operator<<(std::ostream& os, ColorSpace const& format);
std::ostream& operator<<(std::ostream& os, ImageHeader const& header);
std::ostream& operator<<(std::ostream& os, Timestamp ts);
std::ostream& operator<<(std::ostream& os, TFV_Id id);

#endif
}
#endif