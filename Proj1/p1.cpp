#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stack>

#define WHITE 0
#define BLACK 1
#define DEBUG 1

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
  list<Vertex *> _cons;
  int _parentId;
  bool _ap;
  bool _ignore;

  Vertex() {
    _d = -1;
    _visit = false;
    _parentId = -1;
    _ap = false;
    _low = -1;
    _ignore = false;
  }

  void setId(int id) { _id = id; }

  int getId() { return _id; }

  void addCon(Vertex *vertex) { _cons.push_back(vertex); }

  void removeCon(Vertex *vertex) {
    list<Vertex *>::iterator it;
    for (it = _cons.begin(); it != _cons.end(); ++it) {
      if (vertex->getId() == (*it)->getId()) {
        _cons.erase(it);
        break;
      }
    }
  }

  void resetVertex() {
    _d = -1;
    _visit = false;
    _parentId = -1;
    _ap = false;
    _low = -1;
  }

  void display() {
    list<Vertex *>::iterator it;
    for (it = _cons.begin(); it != _cons.end(); ++it) {
      if (!(*it)->_ignore)
        printf("id: %d, d: %d, low: %d - ", (*it)->getId(), (*it)->_d,
               (*it)->_low);
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

  int creatGraphFromStdin() {
    scanf("%d", &_n);
    scanf("%d", &_m);

    if (_n < 2 || _m < 1) {
      return -1;
    }

    vertexes = new Vertex[_n];

    for (int i = 0; i < _n; i++) {
      vertexes[i].setId(i + 1);
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

  void resetGraph() {
    for (int i = 0; i < _n; i++) {
      vertexes[i].resetVertex();
    }
  }

  void dislay() {
    for (int i = 0; i < _n; i++) {
      if (!vertexes[i]._ignore) {
        printf("%d: ", vertexes[i].getId());
        vertexes[i].display();
        printf("NULL\n");
      }
    }
  }
};

class Tarjan {
  Graph *_g;
  int _visited;
  int _numberOfSCC;
  int _biggestSCC;
  int _nArticulationPoints;
  stack<Vertex *> _l;
  list<Vertex *> _roots;

  void tarjanVisit(Vertex &v) {
    if (v._ignore) return;  // Ignore removed vertexes
    int treeChildren = 0;
    v._d = _visited;
    v._low = _visited;
    _visited++;
    _l.push(&v);

    for (Vertex *it : v._cons) {
      if (it->_ignore) continue;  // Ignore removed vertexes
      if (it->_d == -1) {
        treeChildren++;
        it->_parentId = v.getId();

        tarjanVisit(*it);
        v._low = min(it->_low, v._low);

        // Case 2 If v is not root and low value of one of its child is bigger
        // than the discovery value of v.
        if (v._parentId != -1 && it->_low >= v._d && !v._ap) {
          v._ap = true;
          _nArticulationPoints++;
        }

      } else if (v.getId() != it->_parentId) {
        v._low = min(it->_d, v._low);
      }
    }

    // Case 1, v is root of the tree wich has two or more chilren.
    if (v._parentId == -1 && treeChildren > 1 && !v._ap) {
      v._ap = true;
      _nArticulationPoints++;
    };

    if (v._d == v._low) {
      Vertex *vertex_stack;
      int currentSCCSize = 0;
      bool newSCC = true;

      do {
        currentSCCSize++;
        vertex_stack = _l.top();

        if (_roots.back() == NULL || newSCC) {
          _roots.push_back(vertex_stack);
          newSCC = false;
        } else {
          Vertex *it = _roots.back();
          if (it->getId() < vertex_stack->getId()) {
            _roots.pop_back();
            _roots.push_back(vertex_stack);
          }
        }

        // Remove articulation points
        if (vertex_stack->_ap) {
          vertex_stack->_ignore = true;
        }

        vertex_stack->_visit = true;
        _l.pop();
      } while (&v != vertex_stack);
      if (currentSCCSize > _biggestSCC) _biggestSCC = currentSCCSize;
      _numberOfSCC++;
    }
  }

 public:
  Tarjan(Graph *g) {
    _visited = 1;
    _g = g;
    _numberOfSCC = 0;
    _biggestSCC = 0;
    _nArticulationPoints = 0;
  }

  ~Tarjan() {}

  void SCC_Tarjan() {
    for (int i = 0; i < _g->getNumberVertexes(); i++) {
      if (_g->vertexes[i]._d == -1) {
        tarjanVisit(_g->vertexes[i]);
      }
      if (_visited == _g->getNumberVertexes()) return;
    }
  }

  void output() {
    bool first = true;
    printf("%d\n", _numberOfSCC);
    _roots.sort();
    for (Vertex *v : _roots) {
      if (!first) printf(" ");
      printf("%d", v->getId());
      first = false;
    }
    printf("\n");
    printf("%d\n", _nArticulationPoints);
  }

  int getNumberOfSCC() { return _numberOfSCC; }
  int getBiggestSCC() { return _biggestSCC; }
};

int main() {
  Graph *graph = new Graph();

  if (graph->creatGraphFromStdin() == -1) {
    return -1;
  }

  Tarjan *t = new Tarjan(graph);
  t->SCC_Tarjan();  // O(V+E)

  // Print results
  t->output();
  delete t;

  graph->resetGraph();

  Tarjan *newTarjan = new Tarjan(graph);
  newTarjan->SCC_Tarjan();  // O(V+E)

  // Print results
  printf("%d\n", newTarjan->getBiggestSCC());

  delete newTarjan;
  delete graph;

  return 0;
}