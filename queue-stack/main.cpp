#include <iostream>

/**
 * @class Stack
 * @brief A simple integer stack implementation using a dynamic array.
 */
class Stack
{
public:
    int size;  // Maximum capacity of the stack
    int top;   // Index of the top element
    int *data; // Pointer to the array storing stack elements

    // Construct a new Stack object and initializes top to -1.
    Stack()
    {
        this->top = -1;
    }
    // Checks if the stack is empty
    bool isEmpty();
    // Checks if the stack is full
    bool isFull();
    // Pushes an integer onto the stack
    void push(int value);
    // Removes and returns the top element
    int pop();
    // Returns the top element without removing it
    int hTop();
};

/**
 * @class Queue
 * @brief A simple integer queue implementation using a dynamic array.
 */
class Queue
{
public:
    int size;  // Maximum capacity of the queue
    int front; // Index of the front element
    int rear;  // Index of the rear element
    int *data; // Pointer to the array storing queue elements

    // Construct a new Queue object and initializes front and rear to -1.
    Queue()
    {
        // Store -1 as defualt while creating instance
        this->front = this->rear = -1;
    }

    // Checks if the queue is empty
    bool isEmpty();
    // Checks if the queue is full
    bool isFull();
    // Adds an integer to the back of the queue
    void enqueue(int value);
    // Removes and returns the front element
    int dequeue();
    // Returns the front element without removing it
    int qFront();
};

namespace QueueUsingStack
{
    /**
     * @brief Enqueues a value into the queue using two stacks.
     *
     * To simulate queue (FIFO) behavior using stacks (LIFO), we ensure
     * the oldest element is always at the top of sr after every enqueue.
     *
     * Variables:
     *  sr -> main stack acting as the queue
     *  s2 -> helper stack used temporarily during enqueue
     *
     * Algorithm:
     *  1. Pop all elements from sr and push to s2 (empties sr)
     *  2. Push new value into empty sr (it becomes the only element)
     *  3. Pop all elements back from s2 to sr (new value sinks to bottom)
     *
     * Visual Overview:
     *
     * Initial state (sr has existing elements):
     *  | 1  |     |    |
     *  | 2  |     |    |
     *  |_3__|     |____|
     *    sr          s2
     *
     * Step 1 — Pop all from sr, push to s2:
     *  |    |     | 3  |
     *  |    |     | 2  |
     *  |____|     |_1__|
     *    sr          s2
     *
     * Step 2 — Push new value (4) into empty sr:
     *  |    |     | 3  |
     *  |    |     | 2  |
     *  |_4__|     |_1__|
     *    sr          s2
     *
     * Step 3 — Pop all from s2, push back to sr:
     *  | 1  |     |    |
     *  | 2  |     |    |
     *  | 3  |     |    |
     *  |_4__|     |____|
     *    sr          s2
     *
     * Result: oldest element (1) is always at top of sr
     *  sr.hTop() -> 1  (queue front)
     *  sr.pop()  -> 1  (queue dequeue)
     */
    class SQueue
    {
    protected:
        Stack sr, s2;

    public:
        SQueue(int size)
        {
            /**
             * Initialize both stacks with the same maximum capacity to ensure
             * that s2 can always hold all elements from sr during the enqueue process.
             */
            this->sr.size = this->s2.size = size;

            // Create heap memory. Otherwise it will throw Segment Fault error
            this->sr.data = new int[size];
            this->s2.data = new int[size];
        }
        void enqueue(int value);
        int dequeue();
        int front();
        void freeMemory();
    };

    void SQueue::freeMemory()
    {
        delete[] this->sr.data;
        delete[] this->s2.data;
    }

    /**
     * @brief Enqueues an element into the queue.
     *
     * Algorithm:
     * 1. Move all elements from the main stack (sr) to the helper stack (s2).
     * 2. Push the new value into the now empty main stack (sr).
     * 3. Move all elements back from the helper stack (s2) to the main stack (sr).
     */
    void SQueue::enqueue(int value)
    {
        while (!this->sr.isEmpty())
        {

            this->s2.push(this->sr.hTop());
            this->sr.pop();
        }

        this->sr.push(value);

        while (!this->s2.isEmpty())
        {
            this->sr.push(this->s2.hTop());
            this->s2.pop();
        }
    }

    /**
     * @brief Removes and returns the front element of the queue.
     *
     * Algorithm:
     * 1. Check if the main stack (sr) is empty. If so, return -1.
     * 2. Retrieve the top element from the main stack (sr), pop it, and return it.
     */
    int SQueue::dequeue()
    {
        if (this->sr.isEmpty())
        {
            std::cout << "Empty Queue" << std::endl;
            return -1;
        }

        int element = this->sr.hTop();
        this->sr.pop();
        return element;
    }

    /**
     * @brief Returns the front element of the queue without removing it.
     *
     * Algorithm:
     * 1. Check if the main stack (sr) is empty. If so, return -1.
     * 2. Return the top element from the main stack (sr) without popping.
     */
    int SQueue::front()
    {
        if (this->sr.isEmpty())
        {
            std::cout << "Empty Queue" << std::endl;
            return -1;
        }

        return this->sr.hTop();
    }

} // namespace QueueUsingStack

namespace StackUsingQueue
{

    /**
     * @brief Pushes a value onto the stack using two queues.
     *
     * To simulate stack (LIFO) behavior using queues (FIFO), we ensure
     * the newest element is always at the front of qr after every push.
     *
     * Variables:
     *  qr -> main queue acting as the stack
     *  q2 -> helper queue used temporarily during push
     *
     * Algorithm:
     *  1. Move all elements from qr to q2 (empties qr)
     *  2. Enqueue new value into empty qr (it becomes the only element)
     *  3. Move all elements back from q2 to qr (new value stays at front)
     *
     * Visual Overview:
     *
     * Initial state (qr has existing elements):
     *  qr -> [7 ][8 ][10][  ][  ]    f=-1, r=2
     *  q2 -> [  ][  ][  ][  ][  ]    f=r=-1 (empty)
     *
     * Step 1 — Move all from qr to q2:
     *  qr -> [  ][  ][  ][  ][  ]    f=r=-1 (empty)
     *  q2 -> [7 ][8 ][10][  ][  ]    f=-1, r=2
     *
     * Step 2 — Enqueue new value (12) into empty qr:
     *  qr -> [12][  ][  ][  ][  ]    f=-1, r=0
     *  q2 -> [7 ][8 ][10][  ][  ]    f=-1, r=2
     *
     * Step 3 — Move all from q2 back to qr:
     *  qr -> [12][7 ][8 ][10][  ]    f=-1, r=3
     *  q2 -> [  ][  ][  ][  ][  ]    f=r=-1 (empty)
     *
     * Result: newest element (12) is always at front of qr
     *  qr.qFront() -> 12  (stack top)
     *  qr.dequeue() -> 12 (stack pop)
     */
    class QStack
    {
    protected:
        Queue qr, q2;

    public:
        QStack(int size)
        {
            /**
             * Initialize both Queues with the same maximum capacity to ensure
             * that q2 can always hold all elements from qr during the push process.
             */
            this->qr.size = this->q2.size = size;
            // Create heap memory. Otherwise it will throw Segment Fault error
            this->qr.data = new int[size];
            this->q2.data = new int[size];
        }

        void push(int value);
        int pop();
        int top();
        void freeMemory();
    };

    void QStack::freeMemory()
    {
        delete[] this->qr.data;
        delete[] this->q2.data;
    }

    /**
     * @brief Pushes an element onto the stack.
     *
     * Algorithm:
     * 1. Move all elements from the main queue (qr) to the helper queue (q2).
     * 2. Enqueue the new value into the now empty main queue (qr).
     * 3. Move all elements back from the helper queue (q2) to the main queue (qr).
     *
     * @param value The integer value to push.
     */
    void QStack::push(int value)
    {

        while (!this->qr.isEmpty())
        {
            this->q2.enqueue(this->qr.qFront());
            this->qr.dequeue();
        }

        this->qr.enqueue(value);

        while (!this->q2.isEmpty())
        {
            this->qr.enqueue(q2.qFront());
            this->q2.dequeue();
        }
    }

    /**
     * @brief Removes and returns the top element of the stack.
     *
     * Algorithm:
     * 1. Check if the main queue (qr) is empty. If so, return -1.
     * 2. Dequeue and return the front element from the main queue (qr).
     */
    int QStack::pop()
    {
        if (this->qr.isEmpty())
        {
            return -1;
        }

        return this->qr.dequeue();
    }

    /**
     * @brief Returns the top element of the stack without removing it.
     *
     * Algorithm:
     * 1. Check if the main queue (qr) is empty. If so, return -1.
     * 2. Return the front element from the main queue (qr) without dequeuing.
     */
    int QStack::top()
    {
        if (this->qr.isEmpty())
        {
            return -1;
        }

        return this->qr.qFront();
    }
} // namespace StackUsingQueue

// ┌──────────────────────────────────────────────────────────┐
// │                 Main Program Execution                   │
// └──────────────────────────────────────────────────────────┘

int main()
{

    QueueUsingStack::SQueue sQueue = QueueUsingStack::SQueue(10);
    StackUsingQueue::QStack qStack = StackUsingQueue::QStack(100);

    // Queue Using Stack
    sQueue.enqueue(23);
    sQueue.enqueue(24);
    sQueue.enqueue(25);
    sQueue.enqueue(26);

    // Stack Using Queue
    qStack.push(23);
    qStack.push(24);
    qStack.push(25);
    qStack.push(26);
    qStack.push(27);
    qStack.push(28);

    // Queue
    std::cout << "Showing Queue Using Stack" << std::endl;
    std::cout << "Dequeued: " << sQueue.dequeue() << std::endl;
    std::cout << "Dequeued: " << sQueue.dequeue() << std::endl;
    std::cout << "Front is: " << sQueue.front() << std::endl
              << std::endl;

    // Stack
    std::cout << "Showing Queue Using Stack" << std::endl;
    std::cout << "Popped: " << qStack.pop() << std::endl;
    std::cout << "Popped: " << qStack.pop() << std::endl;
    std::cout << "Top is: " << qStack.top() << std::endl
              << std::endl;

    sQueue.freeMemory();
    qStack.freeMemory();
    return 0;
}

// ┌──────────────────────────────────────────────────────────┐
// │                 Stack implementation                     │
// └──────────────────────────────────────────────────────────┘
/**
 * @brief Checks if the stack is empty.
 *
 * Algorithm:
 * 1. Check if the top index is equal to -1.
 * 2. Return true if it is, indicating the stack has no elements.
 *
 * @return true if the stack contains no elements, false otherwise.
 */
bool Stack::isEmpty()
{
    if (this->top == -1)
    {
        return true;
    }

    return false;
};

/**
 * @brief Checks if the stack has reached its maximum capacity.
 *
 * Algorithm:
 * 1. Compare the current top index with the maximum capacity (size - 1).
 * 2. Return true if they are equal, indicating the stack is full.
 *
 * @return true if the stack is full, false otherwise.
 */
bool Stack::isFull()
{
    if (this->top == this->size - 1)
    {
        return true;
    }

    return false;
}

/**
 * @brief Pushes an integer onto the stack.
 *
 * Algorithm:
 * 1. Check if the stack is full to prevent overflow.
 * 2. Increment the top index.
 * 3. Assign the value to the data array at the new top index.
 */
void Stack::push(int value)
{
    if (this->isFull())
    {
        std::cout << "Stack overflow" << std::endl;
        return;
    }
    this->top++;
    this->data[this->top] = value;
}

/**
 * @brief Removes and returns the top element of the stack.
 *
 * Algorithm:
 * 1. Check if the stack is empty to prevent underflow.
 * 2. Retrieve the value from the data array at the current top index.
 * 3. Decrement the top index and return the value.
 */
int Stack::pop()
{
    if (this->isEmpty())
    {
        std::cout << "Stack Underflow. Cannot pop element!" << std::endl;
        return -1;
    }

    int element;
    element = this->data[this->top];
    this->top--;
    return element;
}

/**
 * @brief Returns the top element of the stack without removing it.
 *
 * Algorithm:
 * 1. Check if the stack is empty to prevent underflow.
 * 2. Return the value from the data array at the current top index.
 */
int Stack::hTop()
{
    if (this->isEmpty())
    {
        std::cout << "Stack Underflow. Cannot pop element!" << std::endl;
        return -1;
    }

    return this->data[this->top];
}

// ┌──────────────────────────────────────────────────────────┐
// │                 Queue implementation                     │
// └──────────────────────────────────────────────────────────┘
/**
 * @brief Checks if the queue is empty.
 *
 * Algorithm:
 * 1. Compare the front index with the rear index.
 * 2. If they are equal, the queue is empty.
 */
bool Queue::isEmpty()
{
    if (this->front == this->rear)
    {
        return true;
    }
    return false;
}

/**
 * @brief Checks if the queue has reached its maximum capacity.
 *
 * Algorithm:
 * 1. Compare the rear index with the maximum capacity (size - 1).
 * 2. Return true if they are equal, indicating the queue is full.
 */
bool Queue::isFull()
{
    if (this->rear == this->size - 1)
    {
        return true;
    }

    return false;
}

/**
 * @brief Adds an integer to the back of the queue.
 *
 * Algorithm:
 * 1. If the queue is full, shift all existing elements to the start of the array to reclaim space.
 * 2. Increment the rear index.
 * 3. Assign the new value to the data array at the updated rear index.
 */
void Queue::enqueue(int value)
{
    if (this->isFull())
    {
        // shift all elements to front of array and reset indices
        int count = this->rear - this->front;
        for (int i = 0; i < count; i++)
        {
            this->data[i] = this->data[this->front + 1 + i];
        }
        this->front = -1;
        this->rear = count - 1;
    }

    this->rear++;
    this->data[this->rear] = value;
}

/**
 * @brief Removes and returns the front element of the queue.
 *
 * Algorithm:
 * 1. Check if the queue is empty to prevent underflow.
 * 2. Increment the front index and retrieve the value.
 * 3. If the queue becomes empty after removal, reset indices to -1.
 */
int Queue::dequeue()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is empty!" << std::endl;
        return -1;
    }
    this->front++;
    int element = this->data[this->front];

    // reset when fully empty
    if (this->front == this->rear)
    {
        this->front = this->rear = -1;
    }

    return element;
}

/**
 * @brief Returns the front element of the queue without removing it.
 *
 * Algorithm:
 * 1. Check if the queue is empty to prevent underflow.
 * 2. Return the value from the data array at the index (front + 1).
 */
int Queue::qFront()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is emtpy!" << std::endl;
        return -1;
    }

    return this->data[this->front + 1];
}