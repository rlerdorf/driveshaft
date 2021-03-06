/*
 * Driveshaft: Gearman worker manager
 *
 * Copyright (c) [2015] [Keyur Govande]
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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef incl_DRIVESHAFT_MAIN_LOOP_H_
#define incl_DRIVESHAFT_MAIN_LOOP_H_

#include <string>
#include <map>
#include <utility>
#include <time.h>
#include "dist/json/json.h"
#include "common-defs.h"
#include "thread-registry.h"

/* forward declaration so we can friend it to MainLoopImpl below.
 * The friend'ing ensures only main() can create objects of type
 * MainLoopImpl. MainLoopImpl itself can only access the parent
 * constructor and run method and none of the private methods that
 * actually do the work.
 *
 * This whole charade so no users of this header can incorrectly
 * create a stray MainLoop object and call run on it. MainLoop
 * needs to be a singleton in this program.
 */
int main(int argc, char **argv);

namespace Driveshaft {

/* count-of-workers -> [job names] */
typedef struct {
    uint32_t worker_count;
    std::string job_processing_uri;
    StringSet jobs_list;
} PoolData;

typedef std::map<std::string, PoolData> PoolMap;

struct DriveshaftConfig {
    StringSet m_servers_list;
    PoolMap m_pool_map;
    time_t m_load_time;

    std::pair<StringSet, StringSet> compare(const DriveshaftConfig& against) const noexcept;
};

class MainLoop {

protected:
    MainLoop(const std::string& config_file) noexcept;
    void run();
    ~MainLoop() = default;

private:
    void startStatusThread();
    bool setupSignals() const noexcept;
    void doShutdown(uint32_t wait) noexcept;
    void modifyPool(const std::string& name) noexcept;
    bool loadConfig(DriveshaftConfig& new_config);
    void checkRunningConfigAndRegistry() noexcept;

    MainLoop() = delete;
    MainLoop(const MainLoop&) = delete;
    MainLoop(MainLoop&&) = delete;
    MainLoop& operator=(const MainLoop&) = delete;
    MainLoop& operator=(const MainLoop&&) = delete;

    std::string m_config_filename;
    DriveshaftConfig m_config;
    ThreadRegistryPtr m_thread_registry;
    std::unique_ptr<Json::CharReader> m_json_parser;
};

class MainLoopImpl : private MainLoop {
public:
    friend int ::main(int argc, char **argv);

private:
    using MainLoop::MainLoop;
};

} // namespace Driveshaft

#endif // incl_DRIVESHAFT_MAIN_REGISTRY_H_
