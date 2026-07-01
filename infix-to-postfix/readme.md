# Infix to Postfix Conversion — Beginner's Guide

A step-by-step walkthrough of how this C++ program converts a math expression like `(a+b)*(b-c)/(c-a)` into postfix notation `ab+bc-*ca-/` using a **stack built from a linked list**.

---

## 1. What is Infix and Postfix?

| Notation | Example | Meaning |
|---|---|---|
| **Infix** | `a + b` | Operator is *between* operands (how we write math normally) |
| **Postfix** | `a b +` | Operator comes *after* the operands |

Computers prefer postfix because it can be evaluated left-to-right with a simple stack, without worrying about parentheses or precedence rules.

---

## 2. The Building Blocks

This program is built in three layers, like a tower:

```
RpnConverter          <- does the actual conversion logic
   |
   uses
   v
InfStack<T>            <- a Stack (push/pop/top)
   |
   built on top of
   v
SinglyLinkedList<T>     <- a basic linked list (insert/delete at start)
```

### 2.1 `ListNode<T>` — one box in the chain

```cpp
template <typename T>
struct ListNode
{
    T data;
    ListNode *next;
};
```

Think of it as a train car: it carries `data` and has a coupling (`next`) pointing to the car behind it. The last car's `next` is `nullptr`.

### 2.2 `SinglyLinkedList<T>` — a chain of boxes

Key operations:

- **`insertAtStart(data)`** — adds a new node at the *front* of the list.
  ```cpp
  newNode->next = this->head;  // new node points to old first node
  this->head = newNode;        // head now points to new node
  ```
  This is O(1) — instant, no matter how long the list is.

- **`deleteAtStart()`** — removes and returns the front node's data.
  ```cpp
  ListNode<T> *removedNode = this->head;
  this->head = this->head->next;   // skip over the removed node
  T removed_data = removedNode->data;
  delete removedNode;               // free memory
  return removed_data;
  ```

- **`peek(position)`** — walks `position - 1` steps from the head and returns the data there (used to look at the top without removing it).

- **`isEmpty()`** — true if `head == nullptr`.

> [!TIP]
> **Why insert/delete at the *start* and not the *end*?** Inserting/deleting at the end of a singly linked list requires walking the whole list to find the last node (O(n)). Doing it at the start is instant (O(1)) — perfect for a stack.

### 2.3 `InfStack<T>` — a Stack disguised as a linked list

A stack follows **LIFO**: Last In, First Out — like a stack of plates. You can only add/remove from the top.

The trick: **the "top" of the stack = the "head" of the linked list.**

| Stack operation | Linked list operation |
|---|---|
| `push(x)` | `insertAtStart(x)` |
| `pop()` | `deleteAtStart()` |
| `htop()` (peek top) | `peek(1)` |

```cpp
template <typename T>
void InfStack<T>::push(T data)
{
    this->insertAtStart(data);
}
```

Since insert/delete at the start is O(1), push/pop/peek are all O(1) — very efficient.

> [!NOTE]
> `htop()` also handles the empty case safely: if the stack is empty, it returns a default value (`T()`) instead of crashing.

---

## 3. `RpnConverter` — precedence and operator rules

```cpp
int RpnConverter::precedence(char c)
{
    switch (c)
    {
        case '*': return 4;
        case '/': return 3;
        case '+': return 2;
        case '-': return 1;
        default:  return 0;
    }
}
```

Higher number = higher priority = done first. So `*` binds tighter than `+`.

`isOperator(c)` just checks if `c` is one of `+ - * /`.

---

## 4. The Algorithm — Shunting Yard (simplified)

The core idea, scanning the infix string left to right:

1. **Operand** (like `a`, `b`, `1`) → write it *directly* to the output.
2. **`(`** → push it onto the stack (marks "start of sub-expression").
3. **`)`** → pop from the stack and write to output, until you pop the matching `(` (which you discard).
4. **Operator** (`+ - * /`) → compare its precedence with the stack's top:
   - If the current operator has **higher** precedence than the stack top → push it (it should be evaluated first).
   - Otherwise → pop the stack top into the output (it has priority, must go first).
5. At the very end, pop everything left in the stack into the output.

Let's trace it on `(a+b)*(b-c)/(c-a)`.

### Walkthrough Table

| Char read | Action | Stack (top → bottom) | Postfix so far |
|---|---|---|---|
| `(` | push | `(` | |
| `a` | operand → output | `(` | `a` |
| `+` | stack top is `(`, push `+` | `+ (` | `a` |
| `b` | operand → output | `+ (` | `ab` |
| `)` | pop until `(`: pop `+` → output, pop `(` → discard | *(empty)* | `ab+` |
| `*` | stack empty → push | `*` | `ab+` |
| `(` | push | `( *` | `ab+` |
| `b` | operand → output | `( *` | `ab+b` |
| `-` | top is `(` → push | `- ( *` | `ab+b` |
| `c` | operand → output | `- ( *` | `ab+bc` |
| `)` | pop until `(`: pop `-` → output, pop `(` → discard | `*` | `ab+bc-` |
| `/` | precedence(`/`)=3 not > precedence(`*`)=4 → pop `*` → output, then push `/` | `/` | `ab+bc-*` |
| `(` | push | `( /` | `ab+bc-*` |
| `c` | operand → output | `( /` | `ab+bc-*c` |
| `-` | top is `(` → push | `- ( /` | `ab+bc-*c` |
| `a` | operand → output | `- ( /` | `ab+bc-*ca` |
| `)` | pop until `(`: pop `-` → output, pop `(` → discard | `/` | `ab+bc-*ca-` |
| *(end of string)* | drain stack: pop `/` → output | *(empty)* | `ab+bc-*ca-/` |

Final result: **`ab+bc-*ca-/`** ✅ Matches the expected answer in the code comment.

---

## 5. Reading the Code Section by Section

```cpp
char *RpnConverter::convertToPostFix()
{
    InfStack<char> *myStack = new InfStack<char>;
    int size = strlen(this->infix) + 1;
    char *postfix = new char[size];
    int i = 0; // reading position in infix
    int j = 0; // writing position in postfix
```

- `postfix` is allocated with the same length as `infix` — postfix never has *more* characters than infix (it's the same operators/operands, just reordered), so this is always safely large enough.
- `i` scans the input, `j` writes the output.

### Case 1: Opening parenthesis
```cpp
if (this->infix[i] == '(')
{
    myStack->push(this->infix[i]);
    i++;
}
```
Simple — just remember that a sub-expression started here.

### Case 2: Closing parenthesis
```cpp
else if (this->infix[i] == ')')
{
    while (!myStack->isEmpty())
    {
        char element = myStack->pop();
        if (!myStack->isEmpty() && element != '(')
        {
            postfix[j] = element;
            j++;
        }
        else
        {
            break;
        }
    }
    i++;
}
```
Pop everything until we find and discard the matching `(`.

### Case 3: Operand or operator
```cpp
else
{
    if (!isOperator(this->infix[i]))
    {
        postfix[j] = this->infix[i];
        i++;
        j++;
    }
    else
    {
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
```
- Operand → straight to output.
- Operator → compare precedence with stack top, decide whether to push or pop-then-retry.

> [!IMPORTANT]
> Notice `i` does **not** advance in the pop branch — the loop re-checks the same operator against the new stack top on the next iteration. This is intentional and needed for chains like `a-b-c`.

### Draining the stack at the end
```cpp
while (!myStack->isEmpty())
{
    char element = myStack->pop();
    if (element != '(' && element != ')')
    {
        postfix[j] = element;
        j++;
    }
}
```
Anything left on the stack after scanning is an operator waiting to be output (a well-formed expression shouldn't have leftover parentheses, but this check guards against it anyway).

```cpp
postfix[j] = '\0';
delete myStack;
return postfix;
```
Null-terminate the C-string and free the stack.

> [!CAUTION]
> The returned `postfix` buffer is still owned by the caller — remember to `delete[]` it after use, since it was allocated with `new char[size]`. Forgetting this causes a memory leak.

---

## 6. Common Bugs You Might Hit While Building This Yourself

| Symptom | Likely Cause |
|---|---|
| Infinite loop | Forgetting `i++` inside the `)` branch |
| Only one operator popped per `)` | Using `break` right after writing the first popped element, instead of only breaking on `(` |
| Garbage/undefined comparisons | Calling `precedence(myStack->htop())` when the stack is empty and not handling it |
| Crash / wrong output near start | `htop()` or `pop()` called without checking `isEmpty()` first |
| Memory leak | Forgetting `delete[] postfix` or `delete myStack` after use |

---

## 7. Try It Yourself

Pick a few expressions and trace them by hand using the table method from Section 4:

- `a+b*c` → expected: `abc*+`
- `(a+b)*c` → expected: `ab+c*`
- `a*b+c*d` → expected: `ab*cd*+`

> [!TIP]
> If your hand-traced answer doesn't match what the code prints, add `myStack->traverse()` after each character is processed to see the stack's state at every step — that's the fastest way to spot where logic diverges from the expected trace.