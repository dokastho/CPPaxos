#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <iostream>
#include <chrono>

#include "paxos.h"

#define sp ' '

// Class for logging atomically in correct format
class Logger
{
private:
    std::string log_file_name;
    std::ofstream log_file_fp;
    std::mutex m;

public:
    // opens log file for writing
    // closes when object is scoped out
    // ^ may have to change that if output is slow
    Logger(std::string log_file_name) : log_file_name(log_file_name)
    {
        // open file stream
        log_file_fp.open(log_file_name, std::ios::binary);
    }

    template <typename T>
    void write_line(T datum)
    {
        m.lock();
        std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        log_file_fp << std::chrono:: << sp << "n_a:" << sp << datum.n_a << sp << "n_p:" << sp << datum.n_p << sp << "status:" << sp << datum.status << sp << "v_a:" << datum.v_a.data << "\n";
        m.unlock();
        flush_log();
    }

    void flush_log()
    {
        m.lock();
        log_file_fp.flush();
        m.unlock();
    };

    ~Logger()
    {
        flush_log();
        log_file_fp.close();
    }
};
#endif