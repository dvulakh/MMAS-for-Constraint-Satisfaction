
#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <shared_mutex>
#include <cstdarg>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

using namespace std;

#define TPLUS(t0) (chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - (t0)) / 1000000000.0)
#define SLEEP(ms) this_thread::sleep_for(chrono::milliseconds(ms))
#define JOINABLE(...) threads.push_back(new thread(__VA_ARGS__))
#define JOIN for(thread* t : threads) { t->join(), delete t; }
#define SPAWN(...) thread(__VA_ARGS__).detach()
#define L(T, F) &T::F, this

#define DOUBLE atomic<double>
#define BOOL atomic<bool>
#define INT atomic<int>
#define A(T) atomic<T>

#endif
