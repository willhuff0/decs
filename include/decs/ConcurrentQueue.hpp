#pragma once

template<typename T>
class ConcurrentQueue {
public:
    enum Status {
        Success = 0,
        Full,
        Empty,
        Closed,
    };

    virtual ~ConcurrentQueue() = default;

    /*
     * Enqueues value. Blocks if the queue is full. (Some implementations cannot be full.)
     */
    virtual Status Enqueue(const T& value) = 0;
    /*
     * Enqueues value. Returns immediately. Fails if the queue is full. (Some implementations cannot be full.)
     */
    virtual Status TryEnqueue(const T& value) = 0;

    /*
     * Enqueues value. Blocks if the queue is full. (Some implementations cannot be full.)
     */
    virtual Status Enqueue(T&& value) = 0;
    /*
     * Enqueues value. Returns immediately. Fails if the queue is full. (Some implementations cannot be full.)
     */
    virtual Status TryEnqueue(T&& value) = 0;

    /*
     * Dequeues into out. Blocks if the queue is empty.
     */
    virtual Status Dequeue(T& out) = 0;
    /*
     * Dequeues into out. Returns immediately. Fails and does not modify out if the queue is empty.
     */
    virtual Status TryDequeue(T& out) = 0;

    /*
     * Returns true iff the queue is full. (Some implementations cannot be full and will always return false.)
     *
     * Note: This is only reliable if the queue is not being modified by another thread. Otherwise, the fullness of the queue may change immediately following this function call.
     */
    virtual bool IsFull() const noexcept = 0;
    /*
     * Returns true iff the queue is empty.
     *
     * Note: This is only reliable if the queue is not being modified by another thread. Otherwise, the emptiness of the queue may change immediately following this function call.
     */
    virtual bool IsEmpty() const noexcept = 0;

    /*
     * Closes the queue. Subsequent enqueue operations will fail with Status::Closed. Outstanding enqueue operations will be canceled and return Status::Closed. Dequeue operations are not affected.
     */
    virtual void Close() noexcept = 0;
    /*
     * Reopens a closed queue. Does nothing if the queue is already open.
     */
    virtual void Open() = 0;
    /*
     * Returns true iff the queue is closed. Enqueue operations will fail with Status::Closed on closed queues. Closedness does not affect dequeue operations.
     *
     * Note: This is only reliable if the queue is not being modified by another thread. Otherwise, the closedness of the queue may change immediately following this function call.
     */
    virtual bool IsClosed() const noexcept = 0;
};