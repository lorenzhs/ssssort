/*******************************************************************************
 * benchmark.h
 *
 * Benchmark utilities
 *
 *******************************************************************************
 * Copyright (C) 2016 Lorenz Hübschle-Schneider <lorenz@4z2.de>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#pragma once

const bool debug = false;

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>

#include "ssssort.h"
#include "timer.h"
#include "progress_bar.h"

struct statistics {
    // Single-pass standard deviation calculation as described in Donald Knuth:
    // The Art of Computer Programming, Volume 2, Chapter 4.2.2, Equations 15&16
    double mean;
    double nvar; // approx n * variance; stddev = sqrt(nvar / (count-1))
    size_t count;

    statistics() : mean(0.0), nvar(0.0), count(0) {}

    void push(double t) {
        ++count;
        if (count == 1) {
            mean = t;
        } else {
            double oldmean = mean;
            mean += (t - oldmean) / count;
            nvar += (t - oldmean) * (t - mean);
        }
    }

    double avg() {
        return mean;
    }
    double stddev() {
        assert(count > 1);
        return sqrt(nvar / (count - 1));
    }
};

template <typename T, typename Sorter>
statistics run(T* data, const T* const copy, T* out, size_t size, Sorter sorter,
               size_t iterations, const std::string &algoname,
               bool reset_out = true) {
    progress_bar bar(iterations + 1, algoname);
    // warmup
    sorter(data, out, size);
    ++bar;

    statistics stats;
    Timer timer;
    for (size_t it = 0; it < iterations; ++it) {
        // reset data and timer
        std::copy(copy, copy+size, data);
        if (reset_out)
            memset(out, 0, size * sizeof(T));
        timer.reset();

        sorter(data, out, size);

        stats.push(timer.get());
        ++bar;
    }
    bar.undraw();
    return stats;
}

template <typename T, typename Generator>
size_t benchmark(size_t size, size_t iterations, Generator generator,
                 const std::string &name, std::ofstream *stat_stream) {
    T *data = new T[size],
        *out = new T[size],
        *copy = new T[size];

    Timer timer;
    // Generate random numbers as input
    size = generator(data, size);

    // create a copy to be able to sort it multiple times
    std::copy(data, data+size, copy);
    double t_generate = timer.get_and_reset();

    // Number of iterations
    if (iterations == static_cast<size_t>(-1)) {
        if (size < (1<<16)) iterations = 1000;
        else if (size < (1<<18)) iterations = 500;
        else if (size < (1<<20)) iterations = 250;
        else if (size < (1<<24)) iterations = 100;
        else iterations = 50;
    }

    // 1. Super Scalar Sample Sort
    auto t_ssssort = run(data, copy, out, size,
                         [](T* data, T* out, size_t size)
                         { ssssort::ssssort(data, data + size, out); },
                         iterations, "ssssort: ");

    // 2. std::sort
    auto t_stdsort = run(data, copy, out, size,
                         [](T* data, T* /*ignored*/, size_t size)
                         { std::sort(data, data + size); },
                         iterations, "std::sort: ", false);


    // verify
    timer.reset();
    bool incorrect = !std::is_sorted(out, out + size);
    if (incorrect) {
        std::cerr << "Output data isn't sorted" << std::endl;
    }
    for (size_t i = 0; i < size; ++i) {
        incorrect |= (out[i] != data[i]);
        if (debug && out[i] != data[i]) {
            std::cerr << "Err at pos " << i << " expected " << data[i]
                      << " got " << out[i] << std::endl;
        }
    }
    double t_verify = timer.get_and_reset();

    delete[] out;
    delete[] data;
    delete[] copy;

    std::stringstream output;
    output << "RESULT algo=ssssort"
           << " name=" << name
           << " size=" << size
           << " iters=" << iterations
           << " time=" << t_ssssort.avg()
           << " stddev=" << t_ssssort.stddev()
           << " t_gen=" << t_generate
           << " t_check=" << t_verify
           << " ok=" << !incorrect
           << std::endl
           << "RESULT algo=stdsort"
           << " name=" << name
           << " size=" << size
           << " iters=" << iterations
           << " time=" << t_stdsort.avg()
           << " stddev=" << t_stdsort.stddev()
           << " t_gen=" << t_generate
           << " t_check=0"
           << " ok=1"
           << std::endl;
    auto result_str = output.str();
    std::cout << result_str;
    if (stat_stream != nullptr)
        *stat_stream << result_str << std::flush;

    return size;
}

template <typename T, typename Generator>
void benchmark_generator(Generator generator, const std::string &name,
                         size_t iterations, std::ofstream *stat_stream,
                         size_t max_log_size = 27) {
    auto wrapped_generator = [generator](auto data, size_t size) {
        generator(data, size);
        return size;
    };

    // warmup
    benchmark<T>(1<<10, 10, wrapped_generator, "warmup", nullptr);

    for (size_t log_size = 10; log_size < max_log_size; ++log_size) {
        size_t size = 1 << log_size;
        benchmark<T>(size, iterations, wrapped_generator, name, stat_stream);
    }
}


template <typename T, typename Generator>
void sized_benchmark_generator(Generator generator, const std::string &name,
                               size_t iterations, std::ofstream *stat_stream,
                               size_t max_log_size = 27) {
    // warmup
    benchmark<T>(1<<10, 10, generator, "warmup", nullptr);

    for (size_t log_size = 10; log_size < max_log_size; ++log_size) {
        size_t size = 1 << log_size;
        auto last_size = benchmark<T>(size, iterations, generator, name, stat_stream);
        if (last_size < size) break;
    }
}
