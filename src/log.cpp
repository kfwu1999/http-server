/**
 * \file src/log.cpp
 */


#include <vector>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "log.h"


namespace http {


std::shared_ptr<spdlog::logger> Log::s_logger;


void Log::init() {
    // 
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks[0]->set_pattern("%^[%H:%M:%S][%t][%n]: %v%$");

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
