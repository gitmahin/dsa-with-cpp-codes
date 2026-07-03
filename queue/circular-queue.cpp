#include <iostream>

/* -------------------------------------------------------------------------- */
/*                              Abstract Data Type                            */
/* -------------------------------------------------------------------------- */
/**
 * A Template-based Circular Queue implementation using dynamic arrays.
 * Abstract Data Type (ADT).
 *
 * This implementation improves upon the Linear Queue by using a "wraparound"
 * strategy. This eliminates the need for O(n) element shifting during dequeue.
 *
 * NOTE: One slot in the array is intentionally left empty to distinguish
 * between the 'Full' and 'Empty' states.
 *
 * Time Complexities:
 * - enqueue()       : O(1) - Constant time insertion at the rear.
 * - dequeue()       : O(1) - Constant time removal (pointer update only).
 * - frontElement()  : O(1) - Direct access using front pointer.
 * - rearElement()   : O(1) - Direct access using rear pointer.
 * - peek()          : O(1) - Calculated access using modulo.
 * - traverse()      : O(n) - Linear time to visit every valid element.
 * - isEmpty/isFull(): O(1) - Simple conditional pointer checks.
 *
 * Alternative Strategies
 * 1. https://medium.com/@e_moreira/circular-queue-a-generic-data-structure-for-efficient-memory-management-c61d8fa88076 [Thanks to the writer]
 */
template <typename DataType>
class CircularQueue
{
public:
    CircularQueue(int size)
    {
        // set rear == front means queue is empty;
        this->front = this->rear = 0;

        // Assing the size of the queue
        this->size = size + 1;
        /**
         * Allocate memory on the heap. Heap allocation is required for
         * dynamic sizing and to ensure data persists outside the constructor scope.
         * Otherwise any operations on data will throw SEGMENT FAULT Error.
         */
        this->data = new DataType[this->size];
    }

    /**
     * Free the dynamically allocated memory
     * To prevent memory leaks when the object is destroyed.
     *
     * Stack Overflow Reference (For Extra Knowledge): https://stackoverflow.com/questions/34400042/in-c-are-constructors-called-before-or-after-object-creation#comment100926919_34401322
     *
     * Cpp Reference: https://en.cppreference.com/cpp/language/destructor
     */
    ~CircularQueue() // ~ Means Destructor. It runs when the lifetime of an object ends
    {
        delete[] this->data;
    }

    // Follows the ADT principles of encapsulation
private:
    int size;                 // Maximum capacity of the queue
    DataType *data = nullptr; // Pointer to dynamically allocated array
    int rear;                 // Index of the last element in the queue
    int front;

public:
    void enqueue(DataType data);
    DataType dequeue();
    DataType frontElement();
    DataType rearElement();
    DataType peek(int position);
    void traverse();
    bool isEmpty();
    bool isFull();

private:
    int countValidElement();
};

// ┌─────────────────────────────────┐
// │ Theory of valid element count   │
// └─────────────────────────────────┘
/**
 * This calculates how many elements are currently in the queue.
 *
 * Important thing to know first:
 * `front` does NOT point to a real element. It points to the slot of the
 * item that was MOST RECENTLY removed (or, if nothing has been removed
 * yet, it points to a starting "empty" slot before the first element).
 * The actual first valid element is always at (front + 1).
 *
 * Picture the array not as a straight line, but as a circle of slots,
 * like a clock face. `enqueue` always moves `rear` one step CLOCKWISE
 * (0 → 1 → 2 → ... → last index → back to 0 → 1 → ...). It never moves
 * counter-clockwise. So to count elements, we're really measuring the
 * clockwise distance from (front + 1) to rear.
 *
 * --------------------------------------------------------------
 * CASE 1: rear > front  (rear hasn't looped past index 0 yet)
 * --------------------------------------------------------------
 * Example: front = 1, rear = 5
 * Walking clockwise from front, the valid elements sit at indices: 2, 3, 4, 5
 * That's 4 elements, and notice: rear - front = 5 - 1 = 4.
 *
 * So when rear is bigger than front, we can just do:
 *      count = rear - front
 *
 * --------------------------------------------------------------
 * CASE 2: rear <= front  (rear has looped clockwise past index 0
 *                         and is now numerically behind front)
 * --------------------------------------------------------------
 * This happens because the queue is circular. `rear` keeps moving
 * CLOCKWISE, and once it passes the last index, the next clockwise step
 * lands back on index 0. So `rear` can end up with a smaller number
 * than `front`, even though rear is still "ahead" of front if you trace
 * the clockwise path instead of comparing raw numbers.
 *
 * Example: front = 6, rear = 2, size = 11 (indices go from 0 to 10)
 * Walking clockwise from front (6 → 7 → 8 → 9 → 10 → 0 → 1 → 2), the
 * valid elements are at indices: 7, 8, 9, 10, 0, 1, 2
 * That's 7 elements.
 *
 * How do we calculate that with math instead of counting by hand?
 * Think of it in two parts:
 *   - Walking COUNTER-CLOCKWISE from front back to rear covers the
 *     EMPTY part of the queue: front - rear = 6 - 2 = 4
 *   - The queue's total size is 11.
 *   - So the FILLED part (what we actually want) is everything else:
 *     11 - 4 = 7
 *
 * So when rear is smaller than (or equal to) front, we do:
 *      count = size - (front - rear)
 *
 * ---------------------------------------------------------
 * Key takeaway:
 * ---------------------------------------------------------
 * `rear` only ever moves CLOCKWISE, one step at a time, looping from
 * the last index back to 0 when needed. It never moves counter-clockwise.
 * That's why "front chasing rear" is always a clockwise journey — and
 * why a small `rear` number doesn't always mean "behind" front.
 */

// ┌────────────────────────────────────────────────────────────┐
// │ Visualization for circular queue and valid element count   │
// └────────────────────────────────────────────────────────────┘
/**
 * Visual: size = 11 (indices 0-10), front = 6, rear = 2
 *
 *                    0 (valid)
 *            10               1 (valid)
 *         (valid)
 *                                  2 <- rear (valid)
 *      9 (valid)
 *                                  3 (empty)
 *      8 (valid)
 *                                  4 (empty)
 *         7 (valid)
 *            6 <- front         5 (empty)
 *                   (empty)
 *
 *   Clockwise order starting right after front:
 *   front(6) -> 7 -> 8 -> 9 -> 10 -> 0 -> 1 -> 2(rear)
 *               ^-------- valid elements (7 of them) --------^
 *
 *   Then continuing clockwise back to front:
 *   2(rear) -> 3 -> 4 -> 5 -> 6(front)
 *              ^---- empty slots (4 of them) ----^
 *
 *   valid (7) + empty (4) = size (11)
 *
 *
 * #Designed by Claude - Sonnet 5 (Medium)
 */
template <typename T>
int CircularQueue<T>::countValidElement()
{

    if (this->rear > this->front)
    {
        return (this->rear - this->front);
    }
    else
    {
        return (this->size - (this->front - this->rear));
    }
}

// Check if queue is empty or not
template <typename T>
bool CircularQueue<T>::isEmpty()
{
    if (this->rear == this->front)
    {
        return true;
    }
    return false;
}

// Check if queue is full or not
template <typename T>
bool CircularQueue<T>::isFull()
{
    if (((this->rear + 1) % this->size) == this->front)
    {
        return true;
    }
    return false;
}

// Inserts a new element at the end (rear) of the queue.
template <typename T>
void CircularQueue<T>::enqueue(T data)
{
    if (this->isFull())
    {
        std::cout << "Queue is full!" << " " << data << std::endl;
        return;
    }

    this->rear = (this->rear + 1) % this->size;
    this->data[this->rear] = data;
    std::cout << "Enqueued: " << data << std::endl;
}

template <typename T>
T CircularQueue<T>::dequeue()
{
    // Ensure the queue is not empty before removal
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T(); // Return a default-constructed value
    }

    /**
     * Moves the front pointer forward using the modulo operator to achieve a "wraparound" effect.
     * This effectively turns a linear array into a circular buffer.
     *
     * Example:
     * Suppose Queue Size = 5 (Indices: 0, 1, 2, 3, 4)
     * Current front = 4 (The pointer is at the very last element)
     *
     * Without Modulo: front + 1 = 5 (Index 5 is out of bounds!)
     * With Modulo:    (4 + 1) % 5 = 0
     *
     * Result: The pointer "wraps around" from index 4 back to index 0, allowing
     * the queue to reuse the freed space at the start of the array.
     *
     * Note: Same for this->rear also
     */
    this->front = (this->front + 1) % this->size;
    return this->data[this->front];
}

// Get front element from the queue
template <typename T>
T CircularQueue<T>::frontElement()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }
    return this->data[(this->front + 1) % this->size];
}

// Get last element from the queue
template <typename T>
T CircularQueue<T>::rearElement()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }

    return this->data[this->rear % this->size];
}

/**
 * Returns the element at a specific logical position (1st, 2nd, etc.)
 *
 * Logic:
 * 1. (this->front + 1) -> Locates the first actual element (since front is a sentinel).
 * 2. (position - 1)    -> Converts 1-based human position to 0-based offset.
 * 3. % this->size      -> Wraps the index around to the start of the array if it exceeds the bounds.
 *
 * Simplified: (front + position) % size
 */
template <typename T>
T CircularQueue<T>::peek(int position)
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }

    /**
     * Boundary Check: Validates that the requested logical position exists
     * within the current range of enqueued elements.
     *
     * 1. position <= 0: Handles invalid non-positive indexing (since peek is 1-based).
     * 2. count < position: Prevents "reading ahead" into empty slots or stale data
     *    beyond the current number of elements in the queue.
     */
    if (this->countValidElement() < position || position <= 0)
    {
        std::cout << "Out of Bounds: Position " << position << " does not exist!" << std::endl;
        return T();
    }

    return this->data[(this->front + (position - 1) + 1) % this->size];
}

template <typename T>
void CircularQueue<T>::traverse()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return;
    }

    int count = this->countValidElement(); // counter for traversing queue

    /**
     * Start one slot AFTER front, because front itself holds stale
     * (already-dequeued) data, not a current element.
     */
    int circular_index = (this->front + 1) % this->size;
    ;
    for (int i = 0; i < count; i++)
    {
        std::cout << "[" << i + 1 << "]: " << this->data[circular_index] << std::endl;

        /**
         * Advance and wrap around using modulo so we treat the
         * array as circular instead of running off the end.
         */
        circular_index = (circular_index + 1) % this->size;
    }
}

/* -------------------------------------------------------------------------- */
/*                                Main Execution                              */
/* -------------------------------------------------------------------------- */
int main()
{

    CircularQueue<int> *queue = new CircularQueue<int>(10);
    queue->enqueue(23);
    queue->enqueue(24);
    queue->enqueue(25);
    queue->enqueue(26);
    queue->enqueue(27);
    queue->enqueue(28);
    queue->enqueue(29);
    queue->enqueue(30);
    queue->enqueue(31);
    queue->enqueue(32);
    queue->enqueue(33);

    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    queue->enqueue(34);
    queue->enqueue(35);
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    queue->enqueue(36);
    queue->enqueue(37);
    queue->enqueue(38);
    queue->enqueue(39);
    queue->enqueue(40);
    queue->enqueue(41);
    queue->enqueue(42);
    queue->enqueue(43);

    queue->traverse();

    std::cout << "Front element: " << queue->frontElement() << std::endl;
    std::cout << "Last element: " << queue->rearElement() << std::endl;

    std::cout << "Element in position 3: " << queue->peek(3) << std::endl;

    delete queue;

    return 0;
}