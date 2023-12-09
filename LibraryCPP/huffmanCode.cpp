#include "huffmanCode.h"
#include <vector>
#include <map>
#include "array.h"
#include "priorityQueue.h"
#include "huffmanTree.h"
typedef std::map<unsigned char, std::vector<bool>> symbolsTableMap;

struct Byte
{
    uint8_t byte = 0;          // Текущий байт данных
    uint8_t bitsCount = 0;     // Счётчик бит в текущем байте
    std::ifstream& fileIn;     // Ссылка на поток ввода файла
    Byte(std::ifstream& fileInStream) : fileIn(fileInStream) { }
};

void huffman_fileCursorPositionStart(std::ifstream& fileIn)
{
    // Очищаем флаги состояния потока
    // Это нужно сделать, так как при достижении конца файла 
    // устанавливается флаг eof (конец файла), который нужно сбросить перед повторным чтением
    fileIn.clear();

    // Устанавливаем позицию чтения файла в его начало
    // std::ios::beg - начальная позиция в потоке (начало файла)
    fileIn.seekg(0, std::ios::beg);
}

void huffman_makeAlphabet(std::ifstream& fileIn, Array* symbolsCount)
{
    unsigned char symbol; // Переменная для хранения текущего считываемого символа

    // Читаем каждый символ из файла до его конца
    while (fileIn >> std::noskipws >> symbol) // std::noskipws обеспечивает, что пробелы и другие контрольные символы не пропускаются
    {
        // Увеличиваем счётчик для данного символа в массиве symbolsCount
        // array_get возвращает текущее количество данного символа,
        // и мы увеличиваем это значение на 1
        array_set(symbolsCount, symbol, array_get(symbolsCount, symbol) + 1);
    }

    // После подсчёта частот символов, сбрасываем позицию чтения файла в начало
    // Это необходимо для повторного чтения файла при следующих этапах алгоритма
    huffman_fileCursorPositionStart(fileIn);
}

size_t huffman_alphabetGetSymbolsCount(Array* symbolsCount)
{
    size_t count = 0;
    for (uint16_t i = 0; i < 256; i++)
        if (array_get(symbolsCount, i))
            count++;
    return count;
}

void huffman_makeNodesQueue(PriorityQueue* queue, Array* symbolsCount)
{
    for (uint16_t i = 0; i < 256; i++)
        if (array_get(symbolsCount, i))
            priorityQueue_insert(queue, huffman_createLeafNode((unsigned char)i, array_get(symbolsCount, i)));
}

void huffman_writeBitToByte(std::ofstream& fileOut, Byte& byteStruct, bool bitState)
{
    // Сдвигаем текущий байт на один бит влево и добавляем новый бит в его младший разряд.
    // Операция | используется для установки младшего бита в значение bitState.
    // пример: добавить 101 =>5
    byteStruct.byte = (byteStruct.byte << 1) | (uint8_t)bitState;
    // Увеличиваем счётчик битов в текущем байте.
    byteStruct.bitsCount++;
    // Если в байте накоплено 8 бит (полный байт), записываем его в файл.
    if (byteStruct.bitsCount == 8)
    {
        // Сброс счётчика битов, так как байт будет записан в файл.
        byteStruct.bitsCount = 0;
        // Запись полного байта в файл.
        fileOut.put(byteStruct.byte);
        // Сброс байта после записи.
        byteStruct.byte = 0;
    }
}

void huffman_writeSymbolToFile(unsigned char symbol, std::ofstream& fileOut, Byte& byteStruct)
{
    //char = 1 байт = 8 бит
    // Например, если symbol = 'A' (в двоичной системе 0100 0001), то (symbol >> i) & 1 последовательно вернет 1, 0, 0, 0, 0, 1, 0, 0
    for (uint8_t i = 0; i < 8; i++)
        huffman_writeBitToByte(fileOut, byteStruct, ((symbol >> i) & 1));
}

void huffman_saveTreeToFile(std::ofstream& fileOut, HuffmanNode* node, Byte& byteStruct)
{
    // Проверяем, является ли левый дочерний узел листовым
    if (huffman_nodeIsLeaf(huffman_getLeftNode(node)))
    {
        // Если левый узел является листом, записываем бит 1
        huffman_writeBitToByte(fileOut, byteStruct, 1);
        // Затем записываем символ, соответствующий листовому узлу
        huffman_writeSymbolToFile(huffman_getNodeChar(huffman_getLeftNode(node)), fileOut, byteStruct);
    }
    else
    {
        // Если левый узел не является листом, записываем бит 0
        huffman_writeBitToByte(fileOut, byteStruct, 0);
        // Рекурсивно вызываем функцию для левого дочернего узла
        huffman_saveTreeToFile(fileOut, huffman_getLeftNode(node), byteStruct);
    }

    // Аналогично обрабатываем правый дочерний узел
    if (huffman_nodeIsLeaf(huffman_getRightNode(node)))
    {
        // Если правый узел является листом, записываем бит 1
        huffman_writeBitToByte(fileOut, byteStruct, 1);
        // Записываем символ, соответствующий листовому узлу
        huffman_writeSymbolToFile(huffman_getNodeChar(huffman_getRightNode(node)), fileOut, byteStruct);
    }
    else
    {
        // Если правый узел не является листом, записываем бит 0
        huffman_writeBitToByte(fileOut, byteStruct, 0);
        // Рекурсивно вызываем функцию для правого дочернего узла
        huffman_saveTreeToFile(fileOut, huffman_getRightNode(node), byteStruct);
    }
}

void huffman_saveTotalSymbolsToFile(std::ofstream& fileOut, unsigned long long int total)
{
    // Инициализация переменных для работы с битами и байтами
    uint8_t byte = 0;         // Текущий байт для записи
    uint8_t bitsCount = 0;    // Счетчик бит в текущем байте
    uint8_t totalBytesCount = 0; // Количество байтов, необходимое для хранения общего количества символов

    // Определение необходимого количества байтов для хранения total
    if (total < UINT8_MAX)
        totalBytesCount = 1;
    else if (total < UINT16_MAX)
        totalBytesCount = 2;
    else if (total < UINT32_MAX)
        totalBytesCount = 4;
    else
        totalBytesCount = 8;

    // Цикл для записи total в файл по битам
    for (uint8_t i = 0; i < totalBytesCount * 8; i++)
    {
        // Извлекаем бит из total, начиная со старшего бита и двигаясь к младшему
        // Сдвигаем total на нужное количество бит вправо
        // и маскируем результат, чтобы получить один бит
        byte = byte | ((total >> ((totalBytesCount * 8 - 1) - i)) & 1) << (7 - bitsCount++);

        // Как только накопится полный байт, записываем его в файл
        if (bitsCount == 8)
        {
            bitsCount = 0;
            fileOut.put(byte);
            byte = 0;
        }
    }
}

void huffman_makeTable(HuffmanNode* node, symbolsTableMap& table, std::vector<bool>& code)
{
    // Проверяем, существует ли левый дочерний узел
    if (huffman_getLeftNode(node))
    {
        // Добавляем бит 0 к коду, когда идем налево в дереве Хаффмана
        code.push_back(0);
        // Рекурсивный вызов функции для левого дочернего узла
        huffman_makeTable(huffman_getLeftNode(node), table, code);
    }

    // Проверяем, существует ли правый дочерний узел
    if (huffman_getRightNode(node))
    {
        // Добавляем бит 1 к коду, когда идем направо в дереве Хаффмана
        code.push_back(1);
        // Рекурсивный вызов функции для правого дочернего узла
        huffman_makeTable(huffman_getRightNode(node), table, code);
    }

    // Если текущий узел является листовым узлом дерева
    if (huffman_nodeIsLeaf(node))
    {
        // Сохраняем соответствующий код Хаффмана для символа в таблице
        table[huffman_getNodeChar(node)] = code;
    }

    // После возвращения из рекурсии удаляем последний добавленный бит из кода
    // Это позволяет правильно сформировать коды для других узлов при возврате
    if (!code.empty())
        code.pop_back();
}

void huffman_writeUncompletedByte(std::ofstream& fileOut, Byte& byteStruct)
{
    while (byteStruct.bitsCount)
        huffman_writeBitToByte(fileOut, byteStruct, 0);
}

void huffman_compress(std::ifstream& fileIn, const std::string& compressedFileName)
{
    Byte byteStruct(fileIn);
    Array* symbolsCount = array_create(256);
    huffman_makeAlphabet(fileIn, symbolsCount);
    PriorityQueue* nodesQueue = priorityQueue_create(huffman_alphabetGetSymbolsCount(symbolsCount), huffmanNodeComparator, huffmanNodeDestructor);
    huffman_makeNodesQueue(nodesQueue, symbolsCount);
    while (priorityQueue_getSize(nodesQueue) > 1) {

        HuffmanNode* leftNode = (HuffmanNode*)priorityQueue_extractMin(nodesQueue);
        HuffmanNode* rightNode = (HuffmanNode*)priorityQueue_extractMin(nodesQueue);
        HuffmanNode* internalNode = huffman_createInternalNode(leftNode, rightNode);
        priorityQueue_insert(nodesQueue, internalNode);
    }

    HuffmanNode* huffmanTree = (HuffmanNode*)priorityQueue_getMin(nodesQueue);
    priorityQueue_delete(nodesQueue);
    std::ofstream fileOut(compressedFileName, std::ios::binary);
    if (!huffman_nodeIsLeaf(huffmanTree))
        huffman_writeBitToByte(fileOut, byteStruct, 0);
    else
        huffman_writeBitToByte(fileOut, byteStruct, 1);

    // Сохранение дерева Хаффмана в файл
    huffman_saveTreeToFile(fileOut, huffmanTree, byteStruct);
    // Дополнение последнего байта нулями, если он не полный
    huffman_writeUncompletedByte(fileOut, byteStruct);

    // Запись специального маркера и общего количества символов в исходном файле
    fileOut.put((unsigned char)255);
    huffman_saveTotalSymbolsToFile(fileOut, huffman_getNodeWeight(huffmanTree));
    fileOut.put((unsigned char)255);

    // Создание таблицы кодов Хаффмана для каждого символа
    symbolsTableMap table;
    std::vector<bool> symbolCode;
    huffman_makeTable(huffmanTree, table, symbolCode);
    huffmanTree = huffman_deleteTree(huffmanTree);
    // Чтение исходного файла и запись сжатых данных в выходной файл
    while (!fileIn.eof())
    {
        symbolCode = table[(unsigned char)fileIn.get()];
        for (uint8_t i = 0; i < symbolCode.size(); i++)
            huffman_writeBitToByte(fileOut, byteStruct, symbolCode[i]);
    }
    // Дополнение последнего байта нулями и закрытие файла
    huffman_writeUncompletedByte(fileOut, byteStruct);
    fileOut.close();
}


/* DECOMPRESS FUNCTIONS */


bool huffman_getBitFromByte(Byte& byteStruct)
{
    // Извлекаем текущий бит из byteStruct.byte.
    // Операция сдвига вправо на (7 - byteStruct.bitsCount) позиций перемещает желаемый бит на младшую позицию. 
    // После чего применяется побитовое И с 1 для извлечения только этого бита.
    bool result = (byteStruct.byte >> (7 - byteStruct.bitsCount++)) & 1;

    // Если были прочитаны все биты в текущем байте...
    if (byteStruct.bitsCount == 8)
    {
        // Сбрасываем счетчик битов, так как сейчас будет прочитан новый байт.
        byteStruct.bitsCount = 0;
        // Читаем следующий байт из входного файла.
        byteStruct.byte = (uint8_t)byteStruct.fileIn.get();
    }

    return result;
}

unsigned char huffman_getSymbolFromByte(Byte& byteStruct)
{
    // Инициализируем переменную для хранения восстанавливаемого символа.
    unsigned char symbol = 0;

    // Цикл для считывания 8 битов (одного байта).
    for (uint8_t i = 0; i < 8; i++)
    {
        // Вызываем функцию huffman_getBitFromByte для получения одного бита.
        // Сдвигаем полученный бит на i позиций влево.
        // Это позволяет разместить бит на его правильной позиции в символе.
        // Затем применяем побитовое ИЛИ к символу для добавления этого бита.
        symbol = symbol | (huffman_getBitFromByte(byteStruct) << i);
    }

    // Возвращаем восстановленный символ.
    return symbol;
}

bool huffman_getCurrentBitState(const Byte& byteStruct)
{
    return (byteStruct.byte >> (7 - byteStruct.bitsCount)) & 1;
}

HuffmanNode* huffman_rebuildHuffmanTree(Byte& byteStruct, HuffmanNode* node)
{
    // Читаем один бит и определяем, является ли следующий узел листовым или внутренним.
    bool isLeaf = huffman_getBitFromByte(byteStruct);

    // Если текущий узел не существует, создаем его.
    if (!node)
    {
        // Если узел является листовым...
        if (isLeaf)
        {
            // Читаем символ, соответствующий этому листу.
            unsigned char symbol = huffman_getSymbolFromByte(byteStruct);
            // Создаем листовой узел с этим символом. Вес узла устанавливаем в 0,
            // так как он не используется в процессе декомпрессии.
            node = huffman_createLeafNode(symbol, 0);
        }
        else
        {
            // Если это внутренний узел, создаем его без дочерних узлов.
            node = huffman_createInternalNode(NULL, NULL);
        }
    }

    // Если узел не является листовым, т.е. является внутренним узлом...
    if (!isLeaf)
    {
        // Рекурсивно восстанавливаем левый дочерний узел и устанавливаем его в качестве левого ребенка.
        huffman_setLeftNode(node, huffman_rebuildHuffmanTree(byteStruct, huffman_getLeftNode(node)));
        // Аналогично восстанавливаем правый дочерний узел.
        huffman_setRightNode(node, huffman_rebuildHuffmanTree(byteStruct, huffman_getRightNode(node)));
    }

    // Возвращаем созданный или обновленный узел.
    return node;
}

unsigned long long int huffman_readSymbolsCountFromFile(Byte& byteStruct)
{
    // Инициализация счетчика байтов, которые используются для хранения общего числа символов.
    uint8_t symbolsBytesCount = 0;
    // Инициализация переменной для хранения общего количества символов.
    unsigned long long int symbolsCount = 0;

    // Чтение байтов из файла, пока не встретится маркер (255, или 0xFF в шестнадцатеричной системе).
    while ((byteStruct.byte = (uint8_t)byteStruct.fileIn.get()) != 255)
    {
        // Сдвигаем текущее значение symbolsCount на 8 бит влево на каждом шаге.
        // Это делается для корректного добавления следующего байта в symbolsCount.
        symbolsCount = symbolsCount << (symbolsBytesCount * 8);
        // Добавляем считанный байт к symbolsCount, используя побитовое ИЛИ.
        // Это позволяет корректно собрать значение из последовательности байтов.
        symbolsCount = symbolsCount | (unsigned long long int) byteStruct.byte;
        // Увеличиваем счетчик считанных байтов.
        symbolsBytesCount++;
    }

    // Возвращаем общее количество символов.
    return symbolsCount;
}

void huffman_decompress(std::ifstream& fileIn, const std::string& decompressedFileName)
{
    // Инициализируем структуру Byte для чтения битов из файла.
    Byte byteStruct(fileIn);
    // Читаем первый байт из файла.
    byteStruct.byte = (uint8_t)fileIn.get();

    // Открываем выходной файл для записи декомпрессированных данных.
    std::ofstream fileOut;
    fileOut.open(decompressedFileName, std::ios::binary);

    // Восстанавливаем дерево Хаффмана из сжатого файла.
    HuffmanNode* huffmanTree = nullptr;
    huffmanTree = huffman_rebuildHuffmanTree(byteStruct, huffmanTree);

    // Возвращаемся к началу файла для чтения сжатых данных.
    huffman_fileCursorPositionStart(fileIn);
    // Пропускаем данные до маркера (255), указывающего на конец заголовка.
    while ((byteStruct.byte = (uint8_t)fileIn.get()) != 255);

    // Читаем общее количество символов, которые нужно декодировать.
    bool flag = true;
    unsigned long long int symbolsRead = 0;
    unsigned long long int symbolsCount = huffman_readSymbolsCountFromFile(byteStruct);

    // Начинаем декомпрессию с корня дерева Хаффмана.
    HuffmanNode* currentNode = huffmanTree;

    // Проверяем, что дерево Хаффмана существует.
    if (!currentNode) {
        throw std::runtime_error("Ошибка: Дерево Хаффмана пусто");
    }

    // Читаем первый байт сжатых данных.
    byteStruct.byte = (uint8_t)fileIn.get();
    byteStruct.bitsCount = 0;

    // Основной цикл декомпрессии.
    while (flag)
    {
        // Если прочитан полный байт...
        if (byteStruct.bitsCount == 8)
        {
            // Читаем следующий байт.
            byteStruct.byte = (uint8_t)fileIn.get();
            byteStruct.bitsCount = 0;

            // Если достигнут конец файла, завершаем цикл.
            if (fileIn.eof())
                break;
        }

        // Обрабатываем каждый бит в байте.
        for (uint8_t i = 0; i < 8; i++)
        {
            // Двигаемся по дереву вправо или влево в зависимости от бита.
            if (huffman_getBitFromByte(byteStruct))
                currentNode = huffman_getRightNode(currentNode);
            else
                currentNode = huffman_getLeftNode(currentNode);

            // Если достигнут листовой узел...
            if (huffman_nodeIsLeaf(currentNode))
            {
                // Записываем символ в выходной файл.
                fileOut.put(huffman_getNodeChar(currentNode));
                // Возвращаемся к корню дерева для следующего символа.
                currentNode = huffmanTree;
                // Увеличиваем счетчик прочитанных символов.
                symbolsRead++;

                // Если прочитано требуемое количество символов, завершаем цикл.
                if (symbolsRead == symbolsCount)
                {
                    flag = false;
                    break;
                }
            }
        }
    }

    // Очищаем память, выделенную под дерево Хаффмана.
    huffmanTree = huffman_deleteTree(huffmanTree);
    // Закрываем выходной файл.
    fileOut.close();
}
