/**
 * Infix to Postfix using Stack
 *
 * Note: 1. Code documentation and Comments in this codebase are fine-tuned by Gemini 3 Flash Preview.
 *       2. Section Heading Design is taken from ChatGPT.
 *       3. Stack is created by Singly Linked List.
 *          You can create Stack by Array.
 *       4. Some Demo for Linked List and Stack is Provided inside the main method but commented.
 *          You can uncomment and test Linked List and Stack.
 */

#include <iostream>
#include <cstring>

/* -------------------------------------------------------------------------- */
/*                                  List Node                                 */
/* -------------------------------------------------------------------------- */
/**
 * A node for linked list.
 * @template T - The type of data stored within the node.
 */
template <typename T>
struct ListNode
{
    T data;
    // Pointer to the next node in the sequence, or `nullptr` if it is the tail.
    ListNode *next;
};

/* -------------------------------------------------------------------------- */
/*                             Singly Linked List                             */
/* -------------------------------------------------------------------------- */
/**
 * A generic implementation of a singly linked list.
 * @typeParam DataType - The type of data elements stored in the list.
 */
template <typename DataType>
class SinglyLinkedList
{
public:
    // Pointer to the first node in the list.

    ListNode<DataType> *head = nullptr;
    void insertAtStart(DataType data);
    DataType deleteAtStart();

    // return data from a specific position
    DataType peek(int position);
    bool isEmpty();
    void traverse();
};

template <typename T>
bool SinglyLinkedList<T>::isEmpty()
{
    // The list is considered empty if the head pointer does not reference a node on the heap.
    if (this->head == nullptr)
    {
        return true;
    }

    return false;
}

template <typename T>
T SinglyLinkedList<T>::peek(int position)
{
    if (this->isEmpty())
    {
        std::cout << "Singly Linked List not created!" << std::endl;
        return T();
    }

    // Initialize a pointer for list traversal, starting at the head.
    ListNode<T> *ptr = this->head;

    for (int i = 0; i < position - 1 && ptr->next != nullptr; i++)
    {
        ptr = ptr->next;
    }

    return ptr->data;
}

template <typename T>
void SinglyLinkedList<T>::insertAtStart(T data)
{
    // Allocate a new node on the heap
    ListNode<T> *newNode = new ListNode<T>;
    // Initialize the new node's data
    newNode->data = data;
    // Point the new node to the current head
    newNode->next = this->head;
    // Reassign the head pointer to the new starting node
    this->head = newNode;
}

template <typename T>
T SinglyLinkedList<T>::deleteAtStart()
{
    if (this->isEmpty())
    {
        std::cout << "Singly Linked List not created!" << std::endl;
        return T();
    }

    // Save the head pointer so we can delete the node after unlinking it
    ListNode<T> *removedNode = this->head;
    // Shift head to the next node; if head was the only element, it becomes nullptr
    this->head = this->head->next;
    // Buffer the data to return it after the node's memory is freed
    T removed_data = removedNode->data;
    // Prevent memory leaks by deallocating the removed node from the heap
    delete removedNode;
    // Return the extracted data
    return removed_data;
}

template <typename T>
void SinglyLinkedList<T>::traverse()
{
    if (this->isEmpty())
    {
        std::cout << "Singly Linked List not created!" << std::endl;
        return;
    }

    ListNode<T> *ptr = this->head;

    int i = 0;
    while (ptr != nullptr)
    {
        std::cout << "[" << i << "]: " << ptr->data << std::endl;
        ptr = ptr->next;
        i++;
    }
}

/* -------------------------------------------------------------------------- */
/*                       Stack Using Singly Linked List                       */
/* -------------------------------------------------------------------------- */

/**
 * InfStack - A stack implementation based on a singly linked list.
 *
 * Provides "infinite" capacity (limited only by system memory) by
 * leveraging dynamic node allocation. Follows LIFO (Last-In, First-Out) logic.
 *
 * @remarks
 * This class functions by mapping Stack operations to the underlying SinglyLinkedList:
 * The stack top is mapped to the list head to ensure O(1) time complexity for all Stack operations except traverse.
 * - {@link push} is performed using `insertAtStart`.
 * - {@link pop} is performed using `deleteAtStart`.
 * - {@link htop} serves as a peek operation, equivalent to `peek(1)`.
 *
 * @typeParam DataType - The type of elements stored in the stack.
 * @extends SinglyLinkedList<DataType>
 */
template <typename DataType>
class InfStack : public SinglyLinkedList<DataType>
{
public:
    DataType *data;

    /**
     * Retrieves the data from the top of the stack without removing it.
     * @returns The data value at the top of the stack.
     */
    DataType htop(bool isVerboseMode = false);

    /**
     * Removes the top element from the stack and returns its value.
     * @returns The data from the removed top node.
     */
    DataType pop(bool isVerboseMode = false);

    /**
     * Adds a new element to the top of the stack.
     * @param data - The value to be pushed onto the stack.
     */
    void push(DataType data);

    /**
     * Traverses the stack from top to bottom, typically for
     * displaying or processing all current elements.
     */
    void traverse();

    /**
     * Determines if the stack contains any elements.
     * @returns `true` if the stack is empty, otherwise `false`.
     */
    bool isEmpty();
};

template <typename T>
bool InfStack<T>::isEmpty()
{
    if (SinglyLinkedList<T>::isEmpty())
    {
        return true;
    }
    return false;
}

template <typename T>
T InfStack<T>::htop(bool isVerboseMode)
{
    if (this->isEmpty())
    {
        if (isVerboseMode)
        {
            std::cout << "Stack Underflow!" << std::endl;
        }
        return T();
    }

    return this->peek(1);
}

template <typename T>
void InfStack<T>::push(T data)
{
    this->insertAtStart(data);
}

template <typename T>
void InfStack<T>::traverse()
{
    if (this->isEmpty())
    {
        std::cout << "Cannot Traverse! Stack Underflow!" << std::endl;
        return;
    }

    SinglyLinkedList<T>::traverse();
}

template <typename T>
T InfStack<T>::pop(bool isVerboseMode)
{
    if (this->isEmpty())
    {
        if (isVerboseMode)
        {
            std::cout << "Stack Underflow!" << std::endl;
        }
        return T();
    }

    return this->deleteAtStart();
}

/* -------------------------------------------------------------------------- */
/*                          Infix to Postfix Converter                        */
/* -------------------------------------------------------------------------- */
class RpnConverter // Reverse Polish Notation
{
public:
    const char *infix;

    RpnConverter(const char *infix)
    {
        this->infix = infix;
    }
    int precedence(char c);
    bool isOperator(char c);
    char *convertToPostFix();
};

int RpnConverter::precedence(char c)
{

    switch (c)
    {
    case '*':
    {
        return 4;
    }
    case '/':
    {
        return 3;
    }
    case '+':
    {
        return 2;
    }
    case '-':
    {
        return 1;
    }
    default:
    {
        return 0;
    }
    }
}

bool RpnConverter::isOperator(char c)
{
    switch (c)
    {
    case '+':
    case '-':
    case '/':
    case '*':
        return true;
    default:
        return false;
    }
}

/**
 * Converts an infix expression string to a postfix (Reverse Polish Notation) string.
 *
 * This implementation uses the Shunting-yard algorithm logic with a stack-based
 * approach to handle operator precedence and parentheses.
 *
 * @param infix - The input string representing the infix expression.
 * @returns A dynamically allocated string containing the postfix expression.
 *          Note: The caller is responsible for freeing the returned memory.
 */
char *RpnConverter::convertToPostFix()
{
    // Initialize a stack on the heap to manage operators and parentheses
    InfStack<char> *myStack = new InfStack<char>;

    // Calculate length (including null terminator) for memory allocation
    int size = strlen(this->infix) + 1;

    // create new char on the heap with the size
    char *postfix = new char[size];

    int i = 0; // Index for infix scanning
    int j = 0; // Index for postfix construction

    // Scan the infix string until the null terminator is reached
    while (this->infix[i] != '\0')
    {

        // Opening parenthesis - always push to stack to mark a sub-expression
        if (this->infix[i] == '(')
        {
            myStack->push(this->infix[i]);
            i++;
        }
        // Closing parenthesis - pop and append until the matching '(' is found
        else if (this->infix[i] == ')')
        {
            /**
             * If stack is not empty run the loop
             * cause if we find )
             * we have to pop element from the stack until we find (
             */
            while (!myStack->isEmpty())
            {

                char element = myStack->pop();

                // Found the matching '(', sub-expression is resolved
                if (element == '(')
                {
                    break;
                }

                // Discard Opening parenthesis; only append actual operators to the result
                postfix[j] = element;
                j++;
            }
            // update the i so that it can go to next itretion and continue scanning
            // otherwise it will stack in infinite loop
            i++;
        }
        else
        {
            // Operand (Number/Variable) - append directly to postfix output
            if (!isOperator(this->infix[i]))
            {
                // if char is not an operator push directly to postfix
                postfix[j] = this->infix[i];
                // update both itration counter so that it can go to next itreation
                i++;
                j++;
            }
            else
            {

                /**
                 * Operator - handle based on precedence rules
                 * If current operator has higher precedence than stack top, push it.
                 * Otherwise, pop lower-precedence operators to the output first.
                 */
                if (precedence(this->infix[i]) > precedence(myStack->htop()))
                {
                    myStack->push(this->infix[i]);
                    i++;
                }
                else
                {
                    postfix[j] = myStack->pop();
                    j++;
                }
            }
        }
    }

    // Drain any remaining operators from the stack into the postfix expression
    while (!myStack->isEmpty())
    {
        char element = myStack->pop();
        if (element != '(' && element != ')')
        {
            postfix[j] = element;
            j++;
        }
    }

    // Finalize the string and clean up allocated stack memory
    postfix[j] = '\0';
    delete myStack;
    return postfix;
}

int main()
{
    /**
     * Singly Linked List Demo
     * Supports any data type using template
     */
    // SinglyLinkedList<int> *singly = new SinglyLinkedList<int>;
    // singly->insertAtStart(45);
    // singly->insertAtStart(46);
    // singly->insertAtStart(47);
    // singly->traverse();

    /**
     * Infinite Stack using Singly linked list Demo
     * Supports any data type using template
     */
    // InfStack<char> *myStack = new InfStack<char>;
    // myStack->push('a');
    // myStack->push('b');
    // myStack->push('c');

    // std::cout<< "Popped: " << myStack->pop()<<std::endl;
    // std::cout<< "Stack top: " << myStack->htop()<<std::endl;

    // myStack->traverse();

    RpnConverter *rpnconverter = new RpnConverter("(a+b)*(b-c)/(c-a)");
    char *postfix = rpnconverter->convertToPostFix();
    std::cout << postfix << std::endl; // ANS = ab+bc-*ca-/

    delete[] postfix;
    delete rpnconverter;

    return 0;
}