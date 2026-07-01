# Queue Using Stack, and Stack Using Queue — Beginner's Guide

This program builds two classic data structures from scratch (array-based `Stack` and `Queue`), then uses them to simulate **each other**:

- `QueueUsingStack::SQueue` — a Queue built out of two Stacks
- `StackUsingQueue::QStack` — a Stack built out of two Queues

> [!NOTE]
> This is a classic interview/textbook exercise. The goal isn't that this is the "best" way to build a queue or stack — it's to practice converting between LIFO and FIFO behavior using only the operations the other structure allows.

---

## 1. Quick Refresher: Stack vs Queue

| | Stack | Queue |
|---|---|---|
| Order | **LIFO** — Last In, First Out | **FIFO** — First In, First Out |
| Add | `push()` → adds to top | `enqueue()` → adds to back |
| Remove | `pop()` → removes from top | `dequeue()` → removes from front |
| Real-world analogy | A stack of plates | A checkout line |

The whole challenge of this file: **if you only have Stacks, how do you make something behave like a Queue — and vice versa?**

---

## 2. The Base `Stack` Class (array-based)

```cpp
class Stack
{
public:
    int size;
    int top;
    int *data;
    Stack() { this->top = -1; }
    ...
};
```

- `data` — a plain array on the heap (`new int[size]`) that holds the elements.
- `top` — index of the current top element. `-1` means empty.

### `push(value)`
```cpp
void Stack::push(int value)
{
    if (this->isFull()) { ... return; }
    this->top++;
    this->data[this->top] = value;
}
```
Move `top` forward one slot, then write the value there.

### `pop()`
```cpp
int Stack::pop()
{
    if (this->isEmpty()) { ... return -1; }
    int element = this->data[this->top];
    this->top--;
    return element;
}
```
Read the value at `top`, then move `top` back one slot (we don't need to actually erase the old value — it's simply "forgotten" once `top` no longer points to it).

> [!TIP]
> Nothing is physically deleted from the array on `pop()`. The value is still sitting in memory at `data[top+1]`, but since `top` no longer reaches it, the stack treats it as gone. The next `push()` will just overwrite it.

### `hTop()`
Same as `pop()` but without moving `top` — just peek at `data[top]`.

---

## 3. The Base `Queue` Class (array-based, with a twist)

```cpp
class Queue
{
public:
    int size;
    int front;
    int rear;
    int *data;
    Queue() { this->front = this->rear = -1; }
    ...
};
```

> [!IMPORTANT]
> This queue uses the convention that `front` points to one slot **before** the actual front element, not to the front element itself. That's why `qFront()` reads `data[front + 1]`, and `dequeue()` does `front++` before reading.

### `enqueue(value)`
```cpp
void Queue::enqueue(int value)
{
    if (this->isFull())
    {
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
```

When the array runs out of room at the back (`rear == size - 1`), instead of failing, it **shifts all the currently-used elements back to index 0** to reclaim the wasted space at the front (space left behind by earlier `dequeue()` calls), then continues as normal.

**Example** — array of size 5, after some enqueues/dequeues:
```
data:  [ _, _, 7, 8, 10]
front = 1, rear = 4   (slots 0-1 are "used up" — already dequeued)
```
Array is full (`rear == size-1`), so on the next `enqueue`, it shifts:
```
count = rear - front = 3
data:  [7, 8, 10, _, _]
front = -1, rear = 2
```
Now there's room again to `enqueue` at the back.

### `dequeue()`
```cpp
int Queue::dequeue()
{
    if (this->isEmpty()) { ... return -1; }
    this->front++;
    int element = this->data[this->front];
    if (this->front == this->rear) { this->front = this->rear = -1; }
    return element;
}
```
Move `front` forward first, *then* read — matching the "`front` is one behind" convention.

### `isEmpty()`
```cpp
bool Queue::isEmpty()
{
    return this->front == this->rear;
}
```
Both start at `-1`, so an empty queue always has `front == rear`.

---

## 4. Queue Using Two Stacks (`SQueue`)

**The problem:** a Stack only lets you access the *top* (most recently added). A Queue needs access to the *oldest* added item. So we need a trick to "flip the order."

**The trick:** keep two stacks, `sr` (main) and `s2` (helper). On every `enqueue`, temporarily unload `sr` into `s2`, add the new value to the now-empty `sr`, then load everything back from `s2` into `sr`. This reverses the stack twice with the new value slipped in at the very bottom — which places the *oldest* value at the *top* of `sr`, letting `pop()` (LIFO) behave like `dequeue()` (FIFO)!

```cpp
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
```

### Trace it: `enqueue(23)`, `enqueue(24)`, `enqueue(25)`, `enqueue(26)`

Writing each stack **bottom → top** (left to right), so the rightmost value is what `hTop()`/`pop()` would touch:

| Step | `sr` (bottom → top) | `s2` |
|---|---|---|
| enqueue(23): sr empty, nothing to move; push 23 | `23` | *(empty)* |
| enqueue(24): drain sr→s2 (`23`), push 24 into empty sr, drain s2 back | `24, 23` | *(empty)* |
| enqueue(25): drain sr→s2 (`24,23` becomes `23,24` on s2), push 25, drain back | `25, 24, 23` | *(empty)* |
| enqueue(26): drain sr→s2, push 26, drain back | `26, 25, 24, 23` | *(empty)* |

Since these are stacks, the **rightmost value is the top**. So after all four enqueues, `sr.hTop()` is `23` — the *oldest* enqueued value.

The trick works because **draining reverses order twice**: moving `sr → s2` flips the order once, and moving `s2 → sr` flips it back — except the new value gets inserted in between the two flips, landing it at the bottom instead of the top. Do this every single `enqueue`, and the oldest value always stays pinned at the top of `sr`, letting a plain `pop()` behave exactly like `dequeue()`.

```cpp
int SQueue::dequeue()
{
    if (this->sr.isEmpty()) { ... return -1; }
    int element = this->sr.hTop();
    this->sr.pop();
    return element;
}
```

`dequeue()` just reads and pops `sr`'s current top — no extra flipping needed, since `enqueue()` already did all the rearranging work in advance. Calling `dequeue()` after the four enqueues above returns `23`, then `24`, then `25`, then `26` — correct FIFO order.

---

## 5. Stack Using Two Queues (`QStack`)

**The problem:** a Queue only lets you access the *front* (oldest). A Stack needs access to the *most recently added* item on top.

**The trick:** keep two queues, `qr` (main) and `q2` (helper). On every `push`, drain `qr` into `q2`, enqueue the new value into the now-empty `qr` (making it the *only* element, hence automatically at the front), then move everything from `q2` back into `qr` behind it.

```cpp
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
```

### Trace it: `push(7)`, `push(8)`, `push(10)`, `push(12)`

| Step | `qr` (front → back) |
|---|---|
| push(7) | `7` |
| push(8): drain qr→q2 (`7`), enqueue 8 into empty qr, move q2 back | `8, 7` |
| push(10): drain qr→q2 (`8,7`), enqueue 10, move back | `10, 8, 7` |
| push(12): drain qr→q2 (`10,8,7`), enqueue 12, move back | `12, 10, 8, 7` |

Now `qr.qFront()` correctly returns `12` — the **most recently pushed** value — matching LIFO/stack behavior, since `dequeue()`/`qFront()` always read from the front of `qr`, and every `push` re-installs the newest value at the very front.

```cpp
int QStack::top()  { return this->qr.qFront(); }
int QStack::pop()  { return this->qr.dequeue(); }
```

Both `top()` and `pop()` simply read/remove `qr`'s front — no extra flipping needed, because `push()` already did the rearranging work.

> [!TIP]
> This is the cleaner-looking of the two conversions since the diagram in the code comments matches the trace directly. But both `SQueue` and `QStack` use the same underlying idea: pay the O(n) rearranging cost on the "add" operation (`enqueue`/`push`) so that the "remove/peek" operation (`dequeue`/`front`, `pop`/`top`) stays O(1).

---

## 6. Complexity Summary

| Structure | Operation | Time |
|---|---|---|
| `SQueue` | `enqueue` | O(n) — moves all elements twice |
| `SQueue` | `dequeue` / `front` | O(1) |
| `QStack` | `push` | O(n) — moves all elements twice |
| `QStack` | `pop` / `top` | O(1) |

> [!NOTE]
> This is a common pattern for "build X using Y" problems: one operation absorbs all the O(n) rearranging cost, so the other operation can stay O(1).

---

## 7. Try It Yourself

1. Add `sr.hTop()`/`qr.qFront()` print statements inside `enqueue`/`push` after each inner `while` loop, to watch the value at the top/front change step by step and confirm it matches the traces above.
2. Trace `QStack` by hand with `push(1), push(2), pop(), push(3), top()` — what should `top()` return? Run the code and check.