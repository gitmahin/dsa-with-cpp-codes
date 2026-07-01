# Linear Queue (Template-based) — Beginner's Guide

A walkthrough of a generic (template) **Linear Queue** built on a plain dynamic array, and why its `dequeue()` operation is more expensive than you might expect.

---

## 1. What Makes This "Linear"?

This is the simplest possible queue: a fixed-size array where:
- New elements go in at the back (`rear`).
- Elements always come out from index `0` (the front).

> [!NOTE]
> It's called "linear" to distinguish it from a **circular queue**, which reuses freed-up space at the front without shifting everything. You'll see exactly why that distinction matters in Section 4.

---

## 2. The Class Layout

```cpp
template <typename DataType>
class LinearQueue
{
private:
    int size;
    DataType *data = nullptr;
    int rear;
public:
    LinearQueue(int size) { ... }
    ~LinearQueue() { ... }
    void enqueue(DataType data);
    DataType dequeue();
    ...
};
```

- **`template <typename DataType>`** — this queue works with `int`, `char`, `std::string`, or any type you plug in. `LinearQueue<int>` creates a queue of ints; `LinearQueue<char>` creates a queue of chars, etc.
- **`rear`** — index of the last used slot. `-1` means empty. Notice there's no separate `front` variable — the front is *always* index `0` by convention.
- **`private` / `public`** — this follows the **ADT (Abstract Data Type)** principle: the internal array and indices are hidden (`private`), and the outside world can only interact through the public methods (`enqueue`, `dequeue`, etc.). This is called **encapsulation**.

---

## 3. Constructor and Destructor

```cpp
LinearQueue(int size)
{
    this->rear = -1;
    this->size = size;
    this->data = new DataType[this->size];
}

~LinearQueue()
{
    delete[] this->data;
}
```

- The constructor allocates the array on the **heap** with `new DataType[size]`. This is necessary because the size is only known at runtime (passed in as a parameter) — you can't declare a fixed-size array like `DataType data[size]` when `size` isn't a compile-time constant.

> [!TIP]
> This class includes a **destructor** (`~LinearQueue()`), unlike the earlier `Stack`/`Queue` examples in the other tutorial. A destructor runs automatically when the object's lifetime ends (e.g., when you `delete queue;` or when a stack-allocated object goes out of scope), freeing the heap array with `delete[]`. This prevents the memory leak.

---

## 4. `enqueue()` — Adding to the Back

```cpp
template <typename T>
void LinearQueue<T>::enqueue(T data)
{
    if (this->isFull())
    {
        std::cout << "Queue is full!" << std::endl;
        return;
    }
    this->rear++;
    this->data[this->rear] = data;
}
```

Straightforward: bump `rear` forward by one, then write the value there. This is **O(1)** — constant time, regardless of how many elements are already in the queue.

`isFull()` checks `rear == size - 1` — i.e., the array's last valid index has been used.

---

## 5. `dequeue()` — Removing from the Front (the expensive part)

```cpp
template <typename T>
T LinearQueue<T>::dequeue()
{
    if (this->isEmpty()) { ... return T(); }

    T dequeued_element = this->data[0];

    for (int i = 0; i < this->rear; i++)
    {
        this->data[i] = this->data[i + 1];
    }

    this->rear--;
    return dequeued_element;
}
```

Since the front is *always* index `0`, removing the front element leaves a gap at index `0`. To fix that, **every remaining element must shift one position to the left**:

```
Before dequeue:   [23, 24, 25, 26]     rear = 3
                    ^front

Save data[0] = 23

Shift loop (i = 0 to rear-1):
  data[0] = data[1]  ->  [24, 24, 25, 26]
  data[1] = data[2]  ->  [24, 25, 25, 26]
  data[2] = data[3]  ->  [24, 25, 26, 26]

rear-- -> rear = 2

After dequeue:    [24, 25, 26, _]      rear = 2
Return: 23
```

This shifting loop runs once for every element still in the queue, so `dequeue()` is **O(n)** — linear time. Compare that to `enqueue()`'s O(1). This asymmetry is the defining tradeoff of a Linear Queue.

> [!WARNING]
> This is exactly what a **circular queue** avoids: instead of physically shifting elements, it wraps the `front` index around using modulo arithmetic, keeping both `enqueue` and `dequeue` at O(1).

---

## 6. `peek(position)` vs `frontElement()` / `rearElement()`

```cpp
template <typename T>
T LinearQueue<T>::peek(int position)
{
    ...
    return this->data[position - 1];
}
```

> [!IMPORTANT]
> `peek()` uses **1-based** positions for the caller (`peek(1)` gets the first element), but the underlying array is **0-based**, hence `position - 1`. `frontElement()` (`data[0]`) and `rearElement()` (`data[rear]`) are just convenience shortcuts for the two ends, both O(1) since they access a known index directly.

---

## 7. Tracing `main()`

The queue is created with capacity `11`:
```cpp
LinearQueue<int> *queue = new LinearQueue<int>(11);
```

Then 12 values are enqueued: `23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34`.

| Enqueue call | `rear` before | Result |
|---|---|---|
| `enqueue(23)` … `enqueue(33)` | `-1` → `9` | 11 values fit, filling indices `0`–`10` |
| `enqueue(34)` (the 12th) | `rear == 10 == size-1` → `isFull()` is `true` | Prints `"Queue is full!"`, `34` is **not** added |

> [!CAUTION]
> Capacity is `11`, but the code tries to `enqueue` **12** values. The last call (`enqueue(34)`) fails (after printing a warning) — `34` never makes it into the queue. This isn't a bug in the class itself (the full-check correctly prevents overflow), the queue only ever holds `23` through `33`.

After that, three `dequeue()` calls run:

| Call | Removes | Queue front→rear after |
|---|---|---|
| `dequeue()` | `23` | `24, 25, 26, ..., 33` |
| `dequeue()` | `24` | `25, 26, ..., 33` |
| `dequeue()` | `25` | `26, 27, ..., 33` |

Each of these triggers the O(n) shifting loop described in Section 5.

Finally, `traverse()` prints everything currently in the queue — indices `1` through `9` (since `26` through `33` is 8 elements... let's recount: after removing `23,24,25`, the remaining values are `26,27,28,29,30,31,32,33` — 8 elements, printed as `[1]` through `[8]`).

```cpp
template <typename T>
void LinearQueue<T>::traverse()
{
    ...
    for (int i = 0; i <= this->rear; i++)
    {
        std::cout << "[" << i + 1 << "]: " << this->data[i] << std::endl;
    }
}
```

> [!NOTE]
> The loop condition is `i <= this->rear`, not `i < this->rear`. Since `rear` is the index of the *last valid element* (not a count), using `<` would skip printing that last element. This is a classic off-by-one spot to double check whenever `rear`/`top`-style index variables are involved.

---

## 8. Complexity Summary

| Operation | Time | Why |
|---|---|---|
| `enqueue` | O(1) | Direct write at `rear`, then increment |
| `dequeue` | O(n) | Must shift all remaining elements left |
| `frontElement` / `rearElement` / `peek` | O(1) | Direct index access |
| `traverse` | O(n) | Visits every element once |
| `isEmpty` / `isFull` | O(1) | Simple comparison |

---

## 9. Try It Yourself

1. Change the constructor call to `LinearQueue<int>(11)` → `LinearQueue<int>(12)` and confirm all 12 enqueues now succeed.
2. Add a `LinearQueue<std::string>` in `main()` and enqueue a few names — confirm the template works for non-`int` types without changing the class at all.
3. As a challenge: try converting this into a **circular queue** by replacing the `for` shifting loop in `dequeue()` with a `front` index that wraps around using `% size` — see Section 5's warning for the motivation.