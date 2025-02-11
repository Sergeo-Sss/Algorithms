#ifndef GRAPH_H
#define GRAPH_H

#include "vector.h"

template<typename Data>
class Graph {
public:
    Graph(size_t vertex_amount, Data vertex_data) {
        vertexes = new Vector<Vertex*>;
        // ���������� ���������
        vertexes->resize(vertex_amount);
        for (size_t i = 0; i < vertex_amount; i++) {
            vertexes->set(i, new Vertex(vertex_data));
        }
        // �������� ������� ���������
        edgeMatrix = new Vector<Edge*>;
        edgeMatrix->resize(vertex_amount * vertex_amount);

        for (size_t i = 0; i < edgeMatrix->size(); i++) {
            edgeMatrix->set(i, nullptr);
        }

    }
    ~Graph() {
        for (size_t i = 0; i < vertexes->size(); i++) {
            delete vertexes->get(i);
        }
        for (size_t i = 0; i < edgeMatrix->size(); i++) {
            delete edgeMatrix->get(i);
        }
        delete vertexes;
        delete edgeMatrix;
    }
    Graph(const Graph& other) {
        vertexes = nullptr;
        edgeMatrix = nullptr;
        *this = other;
    }

    Graph& operator=(const Graph& other) {
        if (this == &other) {
            return *this;  // �������� �� ���������������� - ����� ��� �������������� �������� ������� ����� �����
        }

        // ������� ������ �������� ����� ����� ������������
        if (vertexes) {
            for (size_t i = 0; i < vertexes->size(); i++) {
                delete vertexes->get(i);  // �������� ������ ������� ������� ������
            }
            delete vertexes;  // �������� ������� ������
        }

        if (edgeMatrix) {
            for (size_t i = 0; i < edgeMatrix->size(); i++) {
                delete edgeMatrix->get(i);  // �������� ������� ����� � ������� ���������
            }
            delete edgeMatrix;  // �������� ������� ���������
        }

        // �������� ������ ������� ������ ��� ��������� �����������
        vertexes = new Vector<Vertex*>;
        vertexes->resize(other.vertexes->size());  // ��������� ������� ��� ������ ������� ������� �����
        for (size_t i = 0; i < other.vertexes->size(); ++i) {
            vertexes->set(i, new Vertex(*(other.vertexes->get(i))));  // ����������� ������ ������ �������
        }

        // �������� ����� ������� ��������� ��� ��������� �����������
        edgeMatrix = new Vector<Edge*>;
        edgeMatrix->resize(other.edgeMatrix->size());  // ��������� ������� ��� ������ ������� ������� �����
        for (size_t i = 0; i < other.edgeMatrix->size(); ++i) {
            Edge* edge = other.edgeMatrix->get(i);
            if (edge) {
                edgeMatrix->set(i, new Edge(*(edge)));  // ����������� ������ ������� �����, ���� ��� ����������
            }
            else {
                edgeMatrix->set(i, nullptr);  // ��������� nullptr, ���� ����� �� ����������
            }
        }

        return *this;  // ������� ������ �� ������� ������ ��� ����������� ������� ������������
    }
    struct Vertex {
    private:
        Data vertex_data;
    public:
        Vertex(Data vertex_data) {
            this->vertex_data = vertex_data;
        }
        void setVertexData(Data data) {
            this->vertex_data = data;
        }
        Data getVertexData() {
            return vertex_data;
        }
    };

    struct Edge {
    private:
        Data edge_data;
    public:
        Edge(Data data) {
            this->edge_data = data;
        }
        void setEdgeData(Data data) {
            this->edge_data = data;
        }
        Data getEdgeData() {
            return edge_data;
        }
    };

    size_t getVertexAmount() {
        return vertexes->size();
    }

    // ��������� ��������� Iterator ��� ������ ������ �����, ������� � �������� ��������� ��������
    struct Iterator {
    private:
        Graph* graph;  // ��������� �� ����, �� �������� ����� ������������� ��������
        size_t start;  // ������ ��������� ������� ��� ��������
        int end = -1;  // ������ ��������� ������� �������. ���������������� ��� -1, ��� �������� ����� ��������

        // ��������������� ����� ��� ������ ������� ��������� �������, ������� � �������
        int getNearVertexIndex() {
            // �������� ���������� � ������� ��������� �� ��������� ��������� ������� ��������
            for (size_t i = end + 1; i < graph->getVertexAmount(); i++) {
                // ���� ����� ��������� � ����������� �������� ���� �����, ���������� � ������
                if (graph->isEdgeExist(start, i)) {
                    return static_cast<int>(i);
                }
            }
            return -1;  // ���� ������� ������ ������ ���, ���������� -1
        }
    public:
        // ����������� ���������
        Iterator(Graph* graph, size_t start) {
            this->graph = graph;  // ��������� ��������� �� ����
            this->start = start;  // ��������� ��������� ������� ��� ��������
            this->end = getNearVertexIndex();  // ����� ������ ������� �������
        }

        // �������� ������������� ��� ��������� ������� ������� �������
        Vertex* operator *() {
            if (end != -1) {
                // ���� ������ ������� ������� ������������, ���������� ��������� �� ��� �������
                return graph->getVertex(end);
            }
            else {
                // ���� ������� ������ ���, ���������� nullptr
                return nullptr;
            }
        }

        // ���������� �������� ���������� ��� �������� � ��������� ������� �������
        void operator ++() {
            end = getNearVertexIndex();  // ���������� ������� ������� �������
        }

        // ����� ��� ��������� ������� ������� ������� �������
        size_t getEnd() const {
            return end;
        }
    };

    Iterator getIterator(size_t start) {
        return Iterator(this, start);
    }
    size_t addVertex(Data vertex_data) {
        // ��������� �������� ���������� ������ � ����� ��� ����������� ������� ����� �������
        size_t index = vertexes->size();
        // ���������� ������� ������ ��� ��������� ����� �������
        vertexes->resize(index + 1);
        // �������� ����� ������� � ������� `vertex_data` � ��������� � � ������ ������ �� ����� ������
        vertexes->set(index, new Vertex(vertex_data));

        // ��������� ����������� ���������� ������ ����� ���������� �����
        size_t vertex_amount = getVertexAmount();

        // �������� ��������� ������� ��������� ��� ��������� ����� �������
        Vector<Edge*>* buffMatrix = new Vector<Edge*>;
        // ��������� ������� ����� ������� ���������, ������ �� ������ ���������� ������
        buffMatrix->resize(vertex_amount * vertex_amount);
        for (size_t i = 0; i < vertex_amount; i++) {
            for (size_t j = 0; j < vertex_amount; j++) {
                // ����������� ������������ ���� � ����� ������� ���������
                // ������ ��� ��������� ����� �� ������ ������� ��������� ����������� ��� ����� ����� �������
                buffMatrix->set((i * vertex_amount) + j, edgeMatrix->get(i * index + j));
            }
        }
        // �������� ������ ���� �� �������� ������� ���������
        for (size_t i = 0; i < edgeMatrix->size(); i++) {
            if (edgeMatrix->get(i)) {
                delete edgeMatrix->get(i);
            }
        }
        // ������������ ������, ���������� ������ �������� ���������
        delete edgeMatrix;
        // ��������� ����� ������� ��������� � �������� ������� ��� �����
        edgeMatrix = buffMatrix;
        // ����������� �������, �� ������� ���� ��������� ����� �������
        return index;
    }
    void removeVertex(size_t index) {
        // ��������� �������� ���������� ������ � �����
        size_t _vertex_amount = getVertexAmount();

        // ���� ��������� ������ ������� �� ������� ���������� ������, ������� �� �������
        if (index >= _vertex_amount) {
            return;
        }

        // �������� ������� �� ������� ������
        Vertex* vertex = vertexes->get(index);
        delete vertex; // ������������ ������, ������� �������� ��������

        // ����� ������ � ������� ������ �� ���� ������� �����
        // ��� ���������� "����" ����� �������� �������
        for (size_t i = index; i < _vertex_amount - 1; i++) {
            vertexes->set(i, vertexes->get(i + 1));
        }
        vertexes->resize(_vertex_amount - 1); // ���������� ������� ������� ������

        // �������� ���� ����, ����������� �������� �������
        for (size_t i = 0; i < _vertex_amount; i++) {
            Edge* edge1 = edgeMatrix->get(index * _vertex_amount + i);
            Edge* edge2 = edgeMatrix->get(i * _vertex_amount + index);
            if (edge1) {
                delete edge1; // �������� �����, �������� � �������� �������
            }
            if (edge2) {
                delete edge2; // �������� �����, ���������� �� �������� �������
            }
        }

        // ����� �������� ������� ��������� ���������� ������ ��� ����� ������� ���������
        size_t vertex_amount = getVertexAmount();

        // �������� ����� ������� ��������� ��� ��������� ����������� ������� �����
        Vector<Edge*>* buffMatrix = new Vector<Edge*>;
        buffMatrix->resize(vertex_amount * vertex_amount);
        for (size_t i = 0; i < vertex_amount; i++) {
            for (size_t j = 0; j < vertex_amount; j++) {
                // ����������� ���� �� ������ ������� ��������� � �����,
                // �������� �������� ����
                // ��������� ����� �� ������ ������� ��������� � ������ �������� ����� �������� �������.
                // ���� ������� ������� ������ (i) ��� ������� (j) ������ ��� ����� ������� �������� �������,
                // ���������� ������ �������� � ������ �������, ����� ����������� ��������������� ����.
                Edge* edge = edgeMatrix->get(((i + (i >= index)) * _vertex_amount) + (j + (j >= index)));
                buffMatrix->set((i * vertex_amount) + j, edge);
            }
        }

        // ������������ ������, ������� ������ �������� ���������
        delete edgeMatrix;
        // ��������� ����� ������� ��������� ��� ������� ������� �����
        edgeMatrix = buffMatrix;
    }
    Vertex* getVertex(size_t index) {
        return vertexes->get(index);
    }

    void addEdge(size_t start_vertex_index, size_t end_vertex_index, Data edge_data) {
        // ��������� ������ ���������� ������ � ����� ��� ������� �������� � ������� ���������
        size_t vertex_amount = getVertexAmount();

        // ������� �������� ������������ ����� �� ������� ��������� �� �������� ��������� � �������� ������
        Edge* existingEdge = edgeMatrix->get(start_vertex_index * vertex_amount + end_vertex_index);

        if (existingEdge) {
            // ���� ����� ����� ���������� ��������� ��� ����������,
            // ��������� ��� ������ � ����� ��������� `edge_data`
            existingEdge->setEdgeData(edge_data);
        }
        else {
            // ���� ����� ����� ��������� �����������,
            // ������� ����� ����� � ������� `edge_data`
            Edge* newEdge = new Edge(edge_data);
            // ��������� ����� ����� � ������� ��������� �� �������, ��������������� �������� ������
            edgeMatrix->set(start_vertex_index * vertex_amount + end_vertex_index, newEdge);
        }
    }

    void removeEdge(size_t start_vertex_index, size_t end_vertex_index) {
        size_t vertex_amount = getVertexAmount();
        Edge* edge = edgeMatrix->get(start_vertex_index * vertex_amount + end_vertex_index);
        delete edge;  // ������������ ������ ��� �����
        edgeMatrix->set(start_vertex_index * vertex_amount + end_vertex_index, nullptr);
    }

    Edge* getEdge(size_t start_vertex_index, size_t end_vertex_index) {
        size_t vertex_amount = getVertexAmount();
        return edgeMatrix->get(start_vertex_index * vertex_amount + end_vertex_index);
    }

    bool isEdgeExist(size_t start_vertex_index, size_t end_vertex_index) {
        return getEdge(start_vertex_index, end_vertex_index) != nullptr;
    }
private:
    Vector<Vertex*>* vertexes; //�������
    Vector<Edge*>* edgeMatrix; //������� ���������
};

#endif