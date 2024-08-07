/**
 * \file src/log.cpp
 */


#include <vector>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "log.h"


namespace http {


std::shared_ptr<spdlog::logger> Log::s_logger;


void Log::init() {
    // 
    std::vector<spdlog::sink_ptr> logSinks;

    // Console sink
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%n][%H:%M:%S][%t]: %v%$");
    logSinks.emplace_back(consoleSink);

    // File sink
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("server.log", true);
    fileSink->set_pattern("[%n][%H:%M:%S][%t][%l]: %v");
    logSinks.emplace_back(fileSink);

    // 
    s_logger = std::make_shared<spdlog::logger>("SERVER", logSinks.begin(), logSinks.end());
    spdlog::register_logger(s_logger);
    s_logger->set_level(spdlog::level::HTTP_LOG_LEVEL);
    s_logger->flush_on(spdlog::level::HTTP_LOG_LEVEL);
}


void Log::shutdown() {
    spdlog::shutdown();
}


} // namespace http::
