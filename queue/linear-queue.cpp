#include <iostream>

/* -------------------------------------------------------------------------- */
/*                              Abstract Data Type                            */
/* -------------------------------------------------------------------------- */
/**
 * A Template-based Linear Queue implementation using dynamic arrays.
 * Abstract Data Type (ADT).
 *
 * Time Complexities:
 * - enqueue()       : O(1) - Constant time insertion at the rear.
 * - dequeue()       : O(n) - Linear time due to shifting elements after removal.
 * - frontElement()  : O(1) - Direct access to the first index.
 * - rearElement()   : O(1) - Direct access to the rear index.
 * - peek()          : O(1) - Direct access to a specific index.
 * - traverse()      : O(n) - Linear time to visit every element.
 * - isEmpty/isFull(): O(1) - Simple conditional checks.
 */
template <typename DataType>
class LinearQueue
{
public:
    LinearQueue(int size)
    {
        // set rear -1 means queue is empty;
        this->rear = -1;
        // Assing the size of the queue
        this->size = size;
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
    ~LinearQueue() // ~ Means Destructor. It runs when the lifetime of an object ends
    {
        delete[] this->data;
    }

    // Follows the ADT principles of encapsulation
private:
    int size;                 // Maximum capacity of the queue
    DataType *data = nullptr; // Pointer to dynamically allocated array
    int rear;                 // Index of the last element in the queue

public:
    void enqueue(DataType data);
    DataType dequeue();
    DataType frontElement();
    DataType rearElement();
    DataType peek(int position);
    void traverse();
    bool isEmpty();
    bool isFull();
};

// Check if queue is empty or not
template <typename T>
bool LinearQueue<T>::isEmpty()
{
    if (this->rear == -1)
    {
        return true;
    }
    return false;
}

// Check if queue is full or not
template <typename T>
bool LinearQueue<T>::isFull()
{
    if (this->rear == this->size - 1) // When rear number is equal to array last index value
    {
        return true;
    }
    return false;
}

// Inserts a new element at the end (rear) of the queue.
template <typename T>
void LinearQueue<T>::enqueue(T data)
{
    if (this->isFull())
    {
        std::cout << "Queue is full!" << std::endl;
        return;
    }

    this->rear++; // Increment the rear as rear is already in last element of the queue
                  // or 'rear' was initialized to -1 for an empty queue

    // Place the new element at the updated rear position
    this->data[this->rear] = data;
}

/**
 * Removes and returns the front element from the queue.
 * Performs an O(n) operation by shifting all remaining elements.
 */
template <typename T>
T LinearQueue<T>::dequeue()
{
    // Ensure the queue is not empty before removal
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T(); // Return a default-constructed value
    }
    // Temporarily store the element at the front (index 0)
    T dequeued_element = this->data[0];

    // Shift all subsequent elements one position to the left to fill the gap
    for (int i = 0; i < this->rear; i++)
    {
        this->data[i] = this->data[i + 1];
    }
    // Decrement the rear pointer as the total number of elements has decreased
    this->rear--;
    //    Return the previously stored front element
    return dequeued_element;
}

// Get front element from the queue
template <typename T>
T LinearQueue<T>::frontElement()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }
    return this->data[0];
}

// Get last element from the queue
template <typename T>
T LinearQueue<T>::rearElement()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }

    return this->data[this->rear]; // As rear number is the queue last element
}

template <typename T>
T LinearQueue<T>::peek(int position)
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return T();
    }

    return this->data[position - 1]; // If position is 1 means fetch array first element which is in index 0
}

template <typename T>
void LinearQueue<T>::traverse()
{
    if (this->isEmpty())
    {
        std::cout << "Queue is Empty!" << std::endl;
        return;
    }

    for (int i = 0; i <= this->rear; i++) // <= cause rear number is the last element of the queue.
                                          // without <= it will avoid printing last element. We know about loop :)
    {
        std::cout << "[" << i + 1 << "]: " << this->data[i] << std::endl;
    }
}

/* -------------------------------------------------------------------------- */
/*                                Main Execution                              */
/* -------------------------------------------------------------------------- */
int main()
{

    LinearQueue<int> *queue = new LinearQueue<int>(11);
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
    queue->enqueue(34);

    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;
    std::cout << "Dequeued: " << queue->dequeue() << std::endl;

    std::cout << "Traversing Queue" << std::endl;
    queue->traverse();

    std::cout << "Front element of the queue: " << queue->frontElement() << std::endl;
    std::cout << "Last element of the queue: " << queue->rearElement() << std::endl;

    delete queue; // Free the dynamically allocated memory

    return 0;
}