#pragma once

#include <chrono>
#include <iostream>
#include <string_view>

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)

/**
 * ������ �������� �����, ��������� � ������� ������ ������
 * �� ����� �������� �����, � ������� � ����� std::cerr.
 *
 * ������ �������������:
 *
 *  void Task1() {
 *      LOG_DURATION("Task 1"s); // ������� � cerr ����� ������ ������� Task1
 *      ...
 *  }
 *
 *  void Task2() {
 *      LOG_DURATION("Task 2"s); // ������� � cerr ����� ������ ������� Task2
 *      ...
 *  }
 *
 *  int main() {
 *      LOG_DURATION("main"s);  // ������� � cerr ����� ������ ������� main
 *      Task1();
 *      Task2();
 *  }
 */
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)

 /**
  * ��������� ���������� ������� LOG_DURATION, ��� ���� ����� ������� �����,
  * � ������� ������ ���� �������� ���������� �����.
  *
  * ������ �������������:
  *
  *  int main() {
  *      // ������� ����� ������ main � ����� std::cout
  *      LOG_DURATION("main"s, std::cout);
  *      ...
  *  }
  */
#define LOG_DURATION_STREAM(x, y) LogDuration UNIQUE_VAR_NAME_PROFILE(x, y)

class LogDuration {
public:
    // ������� ��� ���� std::chrono::steady_clock
    // � ������� using ��� ��������
    using Clock = std::chrono::steady_clock;

    LogDuration(std::string_view id, std::ostream& dst_stream = std::cerr)
        : id_(id)
        , dst_stream_(dst_stream) {
    }

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        dst_stream_ << id_ << ": "sv << duration_cast<milliseconds>(dur).count() << " ms"sv << std::endl;
    }

private:
    const std::string id_;
    const Clock::time_point start_time_ = Clock::now();
    std::ostream& dst_stream_;
};