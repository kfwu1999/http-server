/**
 * \file include/log.h
 */

#pragma once

#ifndef LOG_H_
#define LOG_H_

#include <memory>
#include <spdlog/spdlog.h>

#define HTTP_LOG_LEVEL trace


namespace http {


/**
 * \class Log
 *
 * \ref The Cherno: Logging | Game Engine Series
 *             url: https://youtu.be/dZr-53LAlOw
 */
class Log {
/* Constructor, Destructor and Operators */
public:
    /**
     * \brief Default constructor
     */
    Log() = default;

    /**
     * \brief Default destructor
     */
    ~Log() = default;

/**/
public:
    /**
     * \brief Initialize the logger.
     *
     * Setup the sinks, the patttern and the log level of the logger.
     * This function should use at the start of the application.
     */
    static void init();

    /**
     * \brief Shutdown the logger.
     *
     * This method should use at the end of the application.
     */
    static void shutdown();

    /**
     * \brief Retrieve the logger instance.
     *
     * \return A shared pointer to the logger instance.
     */
    static std::shared_ptr<spdlog::logger>& getLogger() { return s_logger; }

/**/
private:
    static std::shared_ptr<spdlog::logger> s_logger;
};


#ifndef NDEBUG
#define HTTP_TRACE(...)  ::http::Log::getLogger()->trace(__VA_ARGS__)
#define HTTP_DEBUG(...)  ::http::Log::getLogger()->debug(__VA_ARGS__)
#define HTTP_INFO(...)   ::http::Log::getLogger()->info(__VA_ARGS__)
#define HTTP_WARN(...)   ::http::Log::getLogger()->warn(__VA_ARGS__)
#define HTTP_ERROR(...)  ::http::Log::getLogger()->error(__VA_ARGS__)
#define HTTP_FATAL(...)  ::http::Log::getLogger()->fatal(__VA_ARGS__)

#else
#define HTTP_TRACE(...)  (void)0
#define HTTP_DEBUG(...)  (void)0
#define HTTP_INFO(...)   (void)0
#define HTTP_WARN(...)   (void)0
#define HTTP_ERROR(...)  (void)0
#define HTTP_FATAL(...)  (void)0

#endif // NDEBUG


} // namespace http::

#endif // LOG_H_
