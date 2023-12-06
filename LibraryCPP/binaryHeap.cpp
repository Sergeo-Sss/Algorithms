#include "binaryHeap.h"

struct BinaryHeap
{
    void** heapData;    // Массив указателей для хранения элементов кучи
    size_t heapSize;    // Максимальное количество элементов, которое может хранить куча
    size_t dataCount;   // Текущее количество элементов в куче
    Comparator compare; // Функция-компаратор для сравнения элементов кучи
    Destructor destroy; // Функция-деструктор для удаления элементов
};

BinaryHeap* binaryHeap_create(const size_t size, Comparator comp, Destructor dest)
{
    BinaryHeap* heap = new BinaryHeap;
    heap->heapData = new void* [size];
    heap->heapSize = size;
    heap->dataCount = 0;
    heap->compare = comp;
    heap->destroy = dest;
    return heap;
}

void binaryHeap_swapData(BinaryHeap* heap, const size_t firstIndex, const size_t secondIndex)
{
    void* temp = heap->heapData[firstIndex];
    heap->heapData[firstIndex] = heap->heapData[secondIndex];
    heap->heapData[secondIndex] = temp;
}

void binaryHeap_heapify(BinaryHeap* heap, int i)
{
    // Вычисляем индексы левого и правого дочерних элементов для данного узла
    int left = 2 * i + 1;  // Индекс левого дочернего элемента
    int right = 2 * i + 2; // Индекс правого дочернего элемента
    int smallest = i;      // Индекс наименьшего элемента, начинаем с текущего узла

    // Проверяем, меньше ли левый дочерний элемент текущего узла
    // и находится ли он в пределах размера кучи
    if (left < (int)heap->dataCount && heap->compare(heap->heapData[left], heap->heapData[smallest]) < 0)
        smallest = left;

    // Аналогично проверяем правый дочерний элемент
    if (right < (int)heap->dataCount && heap->compare(heap->heapData[right], heap->heapData[smallest]) < 0)
        smallest = right;

    // Если один из дочерних элементов меньше текущего элемента, меняем их местами
    if (smallest != i)
    {
        binaryHeap_swapData(heap, i, smallest);
        // Рекурсивно применяем heapify к измененному дочернему элементу
        binaryHeap_heapify(heap, smallest);
    }
}

void binaryHeap_insert(BinaryHeap* heap, void* node)
{
    // Проверяем, не достигнут ли максимальный размер кучи.
    // Если достигнут, то новый элемент добавить нельзя.
    if (heap->dataCount == heap->heapSize)
        return;

    // Увеличиваем количество элементов в куче на единицу.
    heap->dataCount++;
    // Рассчитываем индекс, где будет размещён новый элемент.
    size_t i = heap->dataCount - 1;
    // Размещаем новый элемент в конец массива кучи.
    heap->heapData[i] = node;

    // Проходим циклом вверх по куче для восстановления её свойств.
    // Пока не достигнут корень кучи и пока родительский элемент больше нового элемента...
    while (i != 0 && heap->compare(heap->heapData[(i - 1) / 2], heap->heapData[i]) > 0)
    {
        // Обмениваем новый элемент с его родителем.
        binaryHeap_swapData(heap, (i - 1) / 2, i);
        // Перемещаем индекс на позицию родительского элемента.
        i = (i - 1) / 2;
    }
}

void* binaryHeap_extractMin(BinaryHeap* heap)
{
    // Проверяем, есть ли элементы в куче. Если нет, возвращаем NULL.
    if (heap->dataCount <= 0)
        return NULL;

    // Если в куче только один элемент, уменьшаем количество элементов
    // и возвращаем этот единственный элемент.
    if (heap->dataCount == 1)
    {
        heap->dataCount--;
        return heap->heapData[0];
    }

    // Сохраняем корневой элемент (минимальный элемент кучи) для последующего возвращения.
    void* root = heap->heapData[0];

    // Заменяем корневой элемент последним элементом в куче.
    heap->heapData[0] = heap->heapData[heap->dataCount - 1];

    // Уменьшаем количество элементов в куче на единицу.
    heap->dataCount--;

    // Применяем процедуру "heapify" начиная с корня, чтобы восстановить свойства кучи
    // и убедиться, что все родительские элементы меньше своих дочерних.
    binaryHeap_heapify(heap, 0);

    // Возвращаем изначальный корневой элемент, который является минимальным элементом кучи.
    return root;
}

void* binaryHeap_getMin(BinaryHeap* heap)
{
    if (heap->dataCount == 0)
        return NULL;
    return heap->heapData[0];
}

size_t binaryHeap_getSize(BinaryHeap* heap)
{
    return heap->dataCount;
}

void binaryHeap_delete(BinaryHeap* heap)
{
    for (size_t i = 0; i < heap->dataCount; i++)
        heap->destroy(heap->heapData[i]);
    delete[] heap->heapData;
    delete heap;
}