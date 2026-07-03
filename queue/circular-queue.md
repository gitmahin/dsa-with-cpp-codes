# Circular Queue (Template-based) — Beginner's Guide

A walkthrough of a generic **Circular Queue** that fixes the Linear Queue's biggest weakness: instead of shifting every element left on `dequeue()`, it "wraps around" the array using modulo arithmetic, keeping both `enqueue` and `dequeue` at O(1).

> [!NOTE]
> If you haven't gone through the [Linear Queue tutorial](./linear-queue.md) yet, it's worth reading first — this implementation is a direct upgrade of that one, and several comparisons below assume you've seen the O(n) shifting problem it's solving.

---

## 1. The Core Idea: A Circle, Not a Line

Picture the array not as a straight row of boxes, but as a **clock face**. Both `front` and `rear` are hands on that clock that only ever move **clockwise**. When a hand reaches the last index, its next step wraps back around to index `0`:

```cpp
this->rear = (this->rear + 1) % this->size;
```

That single `% this->size` is the entire trick. It turns:

```
index 9 -> next step -> index 10 -> next step -> index 0  (wraps!)
```

instead of running off the end of the array.

---

## 2. Why One Slot Is Always Left Empty

```cpp
CircularQueue(int size)
{
    this->front = this->rear = 0;
    this->size = size + 1;   // <-- one extra slot!
    this->data = new DataType[this->size];
}
```

If you request a capacity of `10`, the array actually allocates **11** slots.

> [!IMPORTANT]
> This is intentional, and it's the standard trick for array-based circular queues. If `front` and `rear` could ever point to the _same_ slot in two different situations — both "completely empty" and "completely full" — there'd be no way to tell those two states apart just by checking `front == rear`. Sacrificing one slot (never filling it) means "full" and "empty" always look numerically different:
>
> - **Empty**: `front == rear`
> - **Full**: `(rear + 1) % size == front` (the _next_ slot rear would move into is where front already sits)

So a queue constructed with `CircularQueue<int>(10)` can hold **10** real elements, using 11 array slots.

---

## 3. `front` Is a Sentinel, Not a Real Element

```cpp
this->front = this->rear = 0;
```

> [!IMPORTANT]
> `front` does **not** point at a real element. It points at the slot of the item that was _most recently removed_ (or, before anything is dequeued, an empty starting sentinel slot). The actual first valid element always lives at `(front + 1) % size`. You'll see this `+ 1` pattern repeated in `frontElement()`, `peek()`, and `traverse()`.

---

## 4. `enqueue()` and `dequeue()` — Both O(1)

```cpp
template <typename T>
void CircularQueue<T>::enqueue(T data)
{
    if (this->isFull()) { ... return; }
    this->rear = (this->rear + 1) % this->size;
    this->data[this->rear] = data;
}
```

Move `rear` one step clockwise, then write there. No shifting, ever.

```cpp
template <typename T>
T CircularQueue<T>::dequeue()
{
    if (this->isEmpty()) { ... return T(); }
    this->front = (this->front + 1) % this->size;
    return this->data[this->front];
}
```

Move `front` one step clockwise (onto the real first element), then read it. Compare this to the Linear Queue's `dequeue()`, which had to shift every remaining element left — here it's just two arithmetic operations, no loop at all.

---

## 5. Counting Elements (`countValidElement`)

This is the trickiest part of the whole class, and the code comments already explain it in detail with a clock diagram. Here's the short version:

```cpp
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
```

**Case 1 — `rear > front`:** `rear` hasn't wrapped around yet, so the count is just the plain difference: `rear - front`.

**Case 2 — `rear <= front`:** `rear` _has_ wrapped past index `0` and is now numerically behind `front`, even though it's still logically "ahead" if you walk clockwise. The trick: `front - rear` measures the **empty** stretch (walking counter-clockwise from front back to rear), so subtracting that from the total `size` gives you the **filled** stretch.

> [!TIP]
> Try tracing `front = 6, rear = 2, size = 11` by hand, walking clockwise from `front`: `6 → 7 → 8 → 9 → 10 → 0 → 1 → 2`. That's 7 steps to reach `rear`, so 7 valid elements. Check it against the formula: `size - (front - rear) = 11 - (6 - 2) = 7`. ✅ Matches.

---

## 6. Tracing `main()` Step by Step

The queue is constructed with `CircularQueue<int>(10)` → real array size is `11` (indices `0`–`10`), holding up to `10` elements. Both `front` and `rear` start at `0`.

### First 11 enqueues: `23` through `33`

`enqueue` checks `isFull()` _before_ moving `rear`. Since the queue can hold 10 real elements, the first **10** enqueues (`23`...`32`) succeed, landing at `rear = 1, 2, 3, ..., 10`. The **11th** call, `enqueue(33)`, finds `(rear+1) % size == (10+1) % 11 == 0 == front` → **full**, so `33` is rejected and `"Queue is full! 33"` is printed.

| State after            | `front` | `rear`                      | Contents (front+1 → rear)                |
| ---------------------- | ------- | --------------------------- | ---------------------------------------- |
| 10 successful enqueues | `0`     | `10`                        | `23, 24, 25, 26, 27, 28, 29, 30, 31, 32` |
| `enqueue(33)`          | `0`     | `10` (unchanged — rejected) | _(same, 33 never added)_                 |

### Four dequeues

Each `dequeue()` moves `front` forward by one and returns that slot: removes `23`, then `24`, then `25`, then `26`.

| After      | `front` | `rear` | Contents                 |
| ---------- | ------- | ------ | ------------------------ |
| 4 dequeues | `4`     | `10`   | `27, 28, 29, 30, 31, 32` |

### `enqueue(34)`, `enqueue(35)`

Room freed up by the dequeues gets reused via wraparound: `rear` goes `10 → 0 → 1`.

| After         | `front` | `rear` | Contents                         |
| ------------- | ------- | ------ | -------------------------------- |
| both enqueues | `4`     | `1`    | `27, 28, 29, 30, 31, 32, 34, 35` |

### Two more dequeues

Removes `27`, then `28`.

| After      | `front` | `rear` | Contents                 |
| ---------- | ------- | ------ | ------------------------ |
| 2 dequeues | `6`     | `1`    | `29, 30, 31, 32, 34, 35` |

### `enqueue(36)` through `enqueue(43)` (8 calls)

Only **4** more slots are free (queue holds max 10, currently has 6). `rear` moves `1 → 2 → 3 → 4 → 5`, successfully adding `36, 37, 38, 39`. The 5th call, `enqueue(40)`, checks `(rear+1) % size == (5+1) % 11 == 6 == front` → **full** — rejected. The same happens for `enqueue(41)`, `enqueue(42)`, `enqueue(43)` — the queue stays full, so all four are rejected.

| After all 8 enqueue attempts | `front` | `rear` | Contents                                 |
| ---------------------------- | ------- | ------ | ---------------------------------------- |
| (only 4 of 8 succeeded)      | `6`     | `5`    | `29, 30, 31, 32, 34, 35, 36, 37, 38, 39` |

### Final `traverse()` output

Walking clockwise from `front + 1 = 7` for `count = 10` steps:

```
[1]: 29
[2]: 30
[3]: 31
[4]: 32
[5]: 34
[6]: 35
[7]: 36
[8]: 37
[9]: 38
[10]: 39
```

### `frontElement()`, `rearElement()`, and `peek(3)`

```cpp
std::cout << "Front element: " << queue->frontElement() << std::endl; // 29
std::cout << "Last element: " << queue->rearElement() << std::endl;   // 39
std::cout << "Element in position 3: " << queue->peek(3) << std::endl;
```

- `peek(3)` runs a **boundary check** first:
  ```cpp
  if (this->countValidElement() < position || position <= 0)
  {
      std::cout << "Out of Bounds: Position " << position << " does not exist!" << std::endl;
      return T();
  }
  ```
  `countValidElement()` is `10`, and `position` is `3`, so `10 < 3` is false and `3 <= 0` is false — the check passes. The index formula then runs: `data[(front + (position-1) + 1) % size]` = `data[(6 + 2 + 1) % 11]` = `data[9]` = `31`. Cross-checking against the `traverse()` output above: `[3]: 31` — matches exactly, since `peek(3)` and traverse's 3rd printed line refer to the same logical element.

> [!TIP]
> This boundary check is what makes `peek()` safe to call with _any_ position — try `peek(0)` or `peek(11)` (beyond the current 10 valid elements) and it prints `"Out of Bounds: Position ... does not exist!"` instead of silently returning stale data from an unrelated slot.

---

## 7. Complexity Summary

| Operation                               | Time     | Compared to Linear Queue                |
| --------------------------------------- | -------- | --------------------------------------- |
| `enqueue`                               | O(1)     | Same                                    |
| `dequeue`                               | **O(1)** | **Improved from O(n)** — no shifting    |
| `frontElement` / `rearElement` / `peek` | O(1)     | Same                                    |
| `traverse`                              | O(n)     | Same — still has to visit every element |
| `isEmpty` / `isFull`                    | O(1)     | Same                                    |

The whole point of the circular design: turning `dequeue()` from O(n) into O(1) by trading one array slot for the ability to distinguish "full" from "empty" without shifting anything.
