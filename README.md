# Библиотека работы со списками типов (Type List) в C++

Этот заголовочный файл предоставляет набор утилит для **метапрограммирования** с **списками типов** в C++. 
Включает операции трансформации, фильтрации, конкатенации и сортировки типов.

## Возможности

- **Типовой список (`type_list<Types...>`)** — контейнер для нескольких типов.
- **`apply<F, List>`** — применяет шаблон `F` к элементам `List`.
- **`pop_front<List>`** — удаляет первый элемент списка.
- **`front<List>`** — получает первый элемент списка.
- **`concat_fast<Lists...>`** — объединяет несколько списков типов.
- **`conditional<B, T, F>`** — выбирает `T`, если `B == true`, иначе `F`.
- **`filter<Pred, List>`** — фильтрует элементы списка `List` по предикату `Pred`.
- **`count<List>`** — возвращает количество элементов в списке.
- **`enumerate<List>`** — создаёт список пар `(индекс, значение)`.
- **`flip<List>`** — меняет местами элементы в парах `(A, B) → (B, A)`.
- **`index_of_unique<T, List>`** — находит индекс элемента `T` в списке.
- **`flatten<List>`** — уплощает вложенные списки.
- **`merge_sort<List, Compare>`** — сортирует список слиянием с компаратором.

## Использование

### Пример: создание и манипуляция списком типов

```cpp
#include "type_list.hpp"

using namespace tl;

// Определяем список типов
using MyList = type_list<int, double, char>;

// Получаем первый тип
using First = front<MyList>; // int

// Удаляем первый тип
using Tail = pop_front<MyList>; // type_list<double, char>

// Подсчитываем количество элементов
constexpr std::size_t count_types = count<MyList>; // 3

```

### Пример: Фильтрация списка типов

```cpp
template <typename T>
struct is_integral {
    static constexpr bool value = std::is_integral_v<T>;
};

using Filtered = filter<is_integral, type_list<int, double, char>>;
// Результат: type_list<int, char>
```

### Пример: Сортировка списка типов

```cpp
template <typename A, typename B>
struct less {
    static constexpr bool value = (sizeof(A) < sizeof(B));
};

using Sorted = merge_sort<type_list<int, double, char>, less>;
// Результат: type_list<char, int, double>
```
