/**
 *    Copyright (C) 2018-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include "mongo/s/sharding_task_executor_pool_controller.h"
#include "mongo/stdx/unordered_map.h"
#include "mongo/util/net/hostandport.h"

namespace mongo {
namespace executor {

/**
 * Holds connection information for a specific pool or remote host. These objects are maintained by
 * a parent ConnectionPoolStats object and should not need to be created directly.
 */
struct ConnectionStatsPer {
    ConnectionStatsPer(size_t nInUse,
                       size_t nAvailable,
                       size_t nCreated,
                       size_t nRefreshing,
                       size_t nRefreshed,
                       size_t nWasNeverUsed);

    ConnectionStatsPer();

    ConnectionStatsPer& operator+=(const ConnectionStatsPer& other);

    size_t inUse = 0u;
    size_t available = 0u;
    size_t created = 0u;
    size_t refreshing = 0u;
    size_t refreshed = 0u;
    size_t wasNeverUsed = 0u;
};

/**
 * Aggregates connection information for the connPoolStats command. Connection pools should
 * use the updateStatsForHost() method to append their host-specific information to this object.
 * Total connection counts will then be updated accordingly.
 */
struct ConnectionPoolStats {
    void updateStatsForHost(std::string pool, HostAndPort host, ConnectionStatsPer newStats);

    // FTDC : Full Time Diagnostic Data Collection
    void appendToBSON(mongo::BSONObjBuilder& result, bool forFTDC = false);

    size_t totalInUse = 0u;
    size_t totalAvailable = 0u;
    size_t totalCreated = 0u;
    size_t totalRefreshing = 0u;
    size_t totalRefreshed = 0u;
    size_t totalWasNeverUsed = 0u;
    boost::optional<ShardingTaskExecutorPoolController::MatchingStrategy> strategy;

    using StatsByHost = std::map<HostAndPort, ConnectionStatsPer>;

    struct PoolStats final : public ConnectionStatsPer {
        StatsByHost statsByHost;
    };
    using StatsByPool = std::map<std::string, PoolStats>;

    StatsByHost statsByHost;
    StatsByPool statsByPool;
};

}  // namespace executor
}  // namespace mongo
