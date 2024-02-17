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

#include <cstddef>

#include <boost/optional/optional.hpp>

#include "mongo/bson/bsonobj.h"
#include "mongo/db/repl/oplog_buffer.h"
#include "mongo/platform/mutex.h"
#include "mongo/stdx/condition_variable.h"
#include "mongo/util/duration.h"
#include "mongo/util/interruptible.h"
#include "mongo/util/queue.h"
#include "mongo/util/time_support.h"

namespace mongo {
namespace repl {

/**
 * Oplog buffer backed by in memory blocking queue of BSONObj.
 */
class OplogBufferBlockingQueue final : public OplogBuffer {
public:
    OplogBufferBlockingQueue();
    explicit OplogBufferBlockingQueue(Counters* counters);

    void startup(OperationContext* opCtx) override;
    void shutdown(OperationContext* opCtx) override;
    void push(OperationContext* opCtx,
              Batch::const_iterator begin,
              Batch::const_iterator end,
              std::size_t size = -1) override;
    void waitForSpace(OperationContext* opCtx, std::size_t size) override;
    bool isEmpty() const override;
    std::size_t getMaxSize() const override;
    std::size_t getSize() const override;
    std::size_t getCount() const override;
    void clear(OperationContext* opCtx) override;
    bool tryPop(OperationContext* opCtx, Value* value) override;
    bool waitForDataFor(Milliseconds waitDuration, Interruptible* interruptible) override;
    bool waitForDataUntil(Date_t deadline, Interruptible* interruptible) override;
    bool peek(OperationContext* opCtx, Value* value) override;
    boost::optional<Value> lastObjectPushed(OperationContext* opCtx) const override;

    // In drain mode, the queue does not block. It is the responsibility of the caller to ensure
    // that no items are added to the queue while in drain mode; this is enforced by invariant().
    void enterDrainMode() final;
    void exitDrainMode() final;

private:
    Mutex _notEmptyMutex = MONGO_MAKE_LATCH("OplogBufferBlockingQueue::mutex");
    stdx::condition_variable _notEmptyCv;
    bool _drainMode = false;
    Counters* const _counters;
    BlockingQueue<BSONObj> _queue;
};

}  // namespace repl
}  // namespace mongo
