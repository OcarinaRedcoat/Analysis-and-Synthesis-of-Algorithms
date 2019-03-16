#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stack>

#define WHITE 0
#define BLACK 1

using namespace std;

class SCC;
class Vertex;
class Graph;
class Tarjan;

class Vertex {
 private:
  int _id;

 public:
  int _d;
  int _low;
  bool _visit;
  int _scc;
  list<Vertex *> _cons;

  Vertex() {
    _d = -1;
    _visit = false;
  }

  void set(int id) { _id = id; }

  int get() { return _id; }

  void addCon(Vertex *vertex) { _cons.push_back(vertex); }

  void display() {
    list<Vertex *>::iterator it;
    for (it = _cons.begin(); it != _cons.end(); ++it) {
      printf("%d - ", (*it)->get());
    }
  }
};

class Graph {
 private:
  int _n;
  int _m;

 public:
  Vertex *vertexes;

  Graph() {  // Constructor
    vertexes = NULL;
  }

  ~Graph() {  // Destructor
    delete[] vertexes;
  }

  int creatGraph() {
    scanf("%d", &_n);
    scanf("%d", &_m);

    if (_n < 2 || _m < 1) {
      return -1;
    }

    vertexes = new Vertex[_n];

    for (int i = 0; i < _n; i++) {
      vertexes[i].set(i + 1);
    }

    for (int i = 1; i <= _m; i++) {
      int first;
      int second;
      scanf("%d", &first);
      scanf("%d", &second);
      vertexes[first - 1].addCon(
          &vertexes[second - 1]);  // bi-directional graph
      vertexes[second - 1].addCon(
          &vertexes[first - 1]);  // bi-directional graph
    }
    return 0;
  }

  int getNumberVertexes() { return _n; }

  void dislay() {
    for (int i = 0; i < _n; i++) {
      printf("%d: ", vertexes[i].get());
      vertexes[i].display();
      printf("NULL\n");
    }
  }
};

class Tarjan {
  Graph *_g;
  int _visited;
  int _numberOfSCC;
  stack<Vertex *> _l;
  list<Vertex *> _roots;

  void tarjanVisit(Vertex &v) {
    v._d = _visited;
    v._low = _visited;
    _visited++;
    _l.push(&v);

    for (Vertex *it : v._cons) {
      if (it->_d == -1) {
        tarjanVisit(*it);
      }
      if (!it->_visit) {
        v._low = min(it->_low, v._low);
      }
    }

    if (v._d == v._low) {
      Vertex *vertex_stack;

      do {
        vertex_stack = _l.top();
        vertex_stack->_scc = _numberOfSCC;
        if (_roots.back() == NULL ||
            _roots.back()->_scc != vertex_stack->_scc) {
          _roots.push_back(vertex_stack);
        } else {
          Vertex *it = _roots.back();
          if (it->get() < vertex_stack->get() &&
              it->_scc == vertex_stack->_scc) {
            _roots.pop_back();
            _roots.push_back(vertex_stack);
          }
        }
        vertex_stack->_visit = true;
        _l.pop();
      } while (&v != vertex_stack);
      _numberOfSCC++;
    }
  }

 public:
  Tarjan(Graph *g) {
    _visited = 1;
    _g = g;
    _numberOfSCC = 0;
  }

  ~Tarjan() {}

  void SCC_Tarjan() {
    for (int i = 0; i < _g->getNumberVertexes(); i++) {
      if (_g->vertexes[i]._d == -1) {
        tarjanVisit(_g->vertexes[i]);
      }
    }
  }

  void output() {
    printf("%d\n", _numberOfSCC);
    _roots.sort();
    for (Vertex *v : _roots) {
      printf("%d ", v->get());
    }
    printf("\n");
  }

  int getNumberOfSCC() { return _numberOfSCC; }
};

int main(int argc, char **argv) {
  Graph graph;

  if (graph.creatGraph() == -1) {
    return -1;
  }

  Tarjan *t = new Tarjan(&graph);
  t->SCC_Tarjan();
  // printf("%d\n", t->getNumberOfSCC());
  t->output();
  // graph.dislay();

  return 0;
}