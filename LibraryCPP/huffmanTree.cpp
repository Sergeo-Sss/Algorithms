#include "huffmanTree.h"

// Структура узла дерева Хаффмана
struct HuffmanNode
{
    bool isLeaf; // Флаг, указывающий, является ли узел листом
    unsigned long long int weight; // Вес узла, используется для построения дерева
    unsigned char symbol; // Символ, соответствующий узлу (только для листьев)

    HuffmanNode* leftNode; // Указатель на левый дочерний узел
    HuffmanNode* rightNode; // Указатель на правый дочерний узел
};

// Функция для создания листового узла
HuffmanNode* huffman_createLeafNode(unsigned char symbol, unsigned long long int weight)
{
    HuffmanNode* node = new HuffmanNode;
    node->isLeaf = true;
    node->symbol = symbol;
    node->weight = weight;
    node->leftNode = NULL;
    node->rightNode = NULL;
    return node;
}

// Функция для создания внутреннего узла
HuffmanNode* huffman_createInternalNode(HuffmanNode* leftNode, HuffmanNode* rightNode)
{
    HuffmanNode* node = new HuffmanNode;
    node->isLeaf = false;
    node->leftNode = leftNode;
    node->rightNode = rightNode;
    node->weight = (leftNode && rightNode) ? leftNode->weight + rightNode->weight : 0;
    return node;
}

// Получение левого дочернего узла
HuffmanNode* huffman_getLeftNode(HuffmanNode* node)
{
    return node ? node->leftNode : NULL;
}

// Получение правого дочернего узла
HuffmanNode* huffman_getRightNode(HuffmanNode* node)
{
    return node ? node->rightNode : NULL;
}

// Установка левого дочернего узла
void huffman_setLeftNode(HuffmanNode* node, HuffmanNode* newNode)
{
    node->leftNode = node->leftNode ? node->leftNode : newNode;
}

// Установка правого дочернего узла
void huffman_setRightNode(HuffmanNode* node, HuffmanNode* newNode)
{
    node->rightNode = node->rightNode ? node->rightNode : newNode;
}

// Проверка, является ли узел листом
bool huffman_nodeIsLeaf(HuffmanNode* node)
{
    return node->isLeaf;
}

// Получение символа из узла
unsigned char huffman_getNodeChar(HuffmanNode* node)
{
    return node->symbol;
}

// Получение веса узла
unsigned long long int huffman_getNodeWeight(HuffmanNode* node)
{
    return node->weight;
}

// Удаление дерева Хаффмана (рекурсивная функция)
HuffmanNode* huffman_deleteTree(HuffmanNode* node)
{
    if (node)
    {
        huffman_deleteTree(node->leftNode); // Удаление левого поддерева
        huffman_deleteTree(node->rightNode); // Удаление правого поддерева
        delete node; // Удаление текущего узла
    }
    return nullptr;
}

// Компаратор для узлов (используется в очереди с приоритетом)
int huffmanNodeComparator(const void* nodeA, const void* nodeB)
{
    HuffmanNode* huffmanNodeA = (HuffmanNode*)nodeA;
    HuffmanNode* huffmanNodeB = (HuffmanNode*)nodeB;

    if (huffmanNodeA->weight < huffmanNodeB->weight)
        return -1;
    else if (huffmanNodeA->weight > huffmanNodeB->weight)
        return 1;
    return 0;
}

// Деструктор для узлов (используется в очереди с приоритетом)
void huffmanNodeDestructor(void* node)
{
    HuffmanNode* huffmanNode = (HuffmanNode*)node;
    huffman_deleteTree(huffmanNode);
}

// Функция для печати дерева Хаффмана (для отладки)
void huffman_printTree(HuffmanNode* node, unsigned int height)
{
    if (node)
    {
        huffman_printTree(node->rightNode, height + 1); // Печать правого поддерева
        for (unsigned int i = 0; i < height; i++) // Отступ для визуализации уровня
            std::cout << "  ";
        if (node->isLeaf)
            std::cout << (unsigned char)node->symbol; // Печать символа листа
        else
            std::cout << node->weight; // Печать веса внутреннего узла
        huffman_printTree(node->leftNode, height + 1); // Печать левого поддерева
    }
    else
        std::cout << std::endl;
}