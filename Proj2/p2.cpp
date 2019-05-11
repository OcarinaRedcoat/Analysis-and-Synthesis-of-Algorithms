#include <stdio.h>
#include <climits>
#include <list>
#include <queue>
#include <set>
using namespace std;

class Vertex;

typedef struct {
  Vertex *orig, *dest;
} ArchPair;

class ResidualArch {
  int capacity;
  int vertex_capacity;

 public:
  int flux;
  Vertex *dest_vertex;
  Vertex *origin_vertex;
  bool canBeCut;
  bool augmentingArch;

  ResidualArch() : flux(0) {}

  ResidualArch(int cap, Vertex *dest) : flux(0) {
    capacity = cap;
    dest_vertex = dest;
  }

  ResidualArch(int cap, Vertex *dest, Vertex *orig) {
    capacity = cap;
    dest_vertex = dest;
    origin_vertex = orig;
    canBeCut = true;
  }

  int getCapacity() { return capacity; }

  void addFlux(int f) {
    flux += f;
    capacity = capacity - f;
  }

  int getFlux() {
    return flux;
  }
};

class Vertex {
 private:
  int id;

 public:
  int height;
  int excess;

  int flow;

  bool station;
  int production;
  bool canBeCut;
  bool augmentingVertex;
  bool visited;

  list<ResidualArch *> backArches;
  list<ResidualArch *> arches;
  queue<ResidualArch *> archeLessHeight;

  Vertex() : height(0), excess(0), flow(0) {}

  Vertex(int i) : height(0), excess(0), flow(0) { id = i; }

  Vertex(int i, int h) : excess(0) {
    id = i;
    height = h;
  }

  void setId(int i) { id = i; }

  int getId() { return id; }

  void addFlux(int f) { flow += f; }

  void addResidualBackArch(ResidualArch *residualArch) {
    backArches.push_back(residualArch);
  }

  void addResidualArch(int cap, Vertex *dest) {
    ResidualArch *temp = new ResidualArch(cap, dest, this);
    dest->addResidualBackArch(temp);
    arches.push_back(temp);
  }

  void updateArchLessQueue() {
    for (ResidualArch *temp : backArches) {
      if (temp->origin_vertex->height < height && temp->getFlux() > 0) {
        archeLessHeight.push(temp);
      }
    }

    for (ResidualArch *temp : arches) {
      if (temp->dest_vertex->height < height && temp->getCapacity() > 0) {

        archeLessHeight.push(temp);
      }
    }
  }

  void setStation() { station = true; }
  bool isStation() { return station; }
};

inline bool operator<(const ArchPair &a, const ArchPair &b) {
  if (a.orig->getId() == b.orig->getId())
    return a.dest->getId() < b.dest->getId();
  return a.orig->getId() < b.orig->getId();
}

class Graph {
 public:
  int suppliers;
  int stations;
  int connections;

  Vertex *source;
  Vertex *hyper;  // sink
  Vertex *vertexes;

  int flow;

  Graph() { vertexes = NULL; }

  int creatGraphFromStdin() {
    flow = 0;

    scanf("%d %d %d", &suppliers, &stations, &connections);
    if (suppliers <= 0 || stations < 0 || connections < 0) {
      return -1;
    }

    vertexes = new Vertex[suppliers + (2 * stations)];
    source = new Vertex(0);
    hyper = new Vertex(1);  // sink


    for (int i = 1; i <= suppliers; i++) {
      vertexes[i - 1].setId(i + 1);
      scanf("%d", &vertexes[i - 1].production);
      source->addResidualArch(vertexes[i - 1].production, &vertexes[i - 1]);
    }

    for (int i = 1; i <= stations; i++) {
      vertexes[i + suppliers - 1].setId(i + suppliers + 1);
      vertexes[i + suppliers - 1].setStation();
      vertexes[i + suppliers + stations - 1].setId(i + suppliers + stations +
                                                   1);
      scanf("%d", &vertexes[i + suppliers - 1].production);
      vertexes[i + suppliers - 1].addResidualArch(
          vertexes[i + suppliers - 1].production,
          &vertexes[i + suppliers + stations - 1]);
    }

    for (int i = 1; i <= connections; i++) {
      int origin, destiny, capacity;
      scanf("%d %d %d", &origin, &destiny, &capacity);
      if (destiny == 1) {
        if (origin > suppliers + 1) {
          vertexes[origin + stations - 2].addResidualArch(capacity, hyper);
        } else {
          vertexes[origin - 2].addResidualArch(capacity, hyper);
        }
      } else if (origin > suppliers + 1) {
        vertexes[origin + stations - 2].addResidualArch(capacity,
                                                        &vertexes[destiny - 2]);
      } else {
        vertexes[origin - 2].addResidualArch(capacity, &vertexes[destiny - 2]);
      }
    }
    return 0;
  }

  void display() {
    printf("id: %d -- height: %d\n", source->getId(), source->height);

    printf("id: %d -- height: %d\n", hyper->getId(), hyper->height);

    for (int i = 0; i < suppliers + (2 * stations); i++) {
      printf("id: %d -- capacity: %d, flow: %d, excess: %d, --- height: %d\n",
             vertexes[i].getId(), vertexes[i].production, vertexes[i].flow,
             vertexes[i].excess, vertexes[i].height);
    }

    for (int i = 0; i < suppliers + (2 * stations); i++) {
      if (!vertexes[i].arches.empty())
        for (ResidualArch *arch : vertexes[i].arches) {
          printf("orin: %d -- dest: %d -- fluxo: %d\n",
                 vertexes[i].getId(), arch->dest_vertex->getId(), arch->flux);
        }
    }
  }

  int getMaxFlow() {
    for (int i = 0; i < suppliers + (2 * stations); i++) {
      if (!vertexes[i].arches.empty())
        for (ResidualArch *arch : vertexes[i].arches) {
          if (arch->dest_vertex->getId() == 1) {
            flow += arch->getFlux();
          }
        }
    }
    return flow;
  }
};

class PushRelabel {
 private:
  int flow;
  int maxHeight;

 public:
  queue<Vertex *> stack;
  Graph graph;
  Vertex *firstElem;
  set<int> augmentingStations;
  set<ArchPair> augmentingArchs;

  PushRelabel(Graph g) : flow(0) { graph = g; }

  void init_pre_flow(Graph g) {
    g.source->height = maxHeight;

    for (ResidualArch *arch : g.source->arches) {
      int capacity = arch->getCapacity();
      if (capacity > 0) {
        arch->addFlux(capacity);
        g.source->excess -= capacity;
        arch->dest_vertex->excess += capacity;
        arch->dest_vertex->addFlux(capacity);
        stack.push(arch->dest_vertex);
      }
    }
  }

  void relabel(Vertex *u) {
    int min_height = INT_MAX;
    for (ResidualArch *arc : u->arches) {  // Front Edges
      if (arc->getCapacity() > 0 && arc->dest_vertex->height <= min_height) {
        min_height = arc->dest_vertex->height;
      }
    }

    for (ResidualArch *arc : u->backArches) {  // Back Edges
      if (arc->origin_vertex->height <= min_height && arc->flux > 0) {
        min_height = arc->origin_vertex->height;
      }
    }

    u->height = min_height + 1;

    u->updateArchLessQueue();
  }

  void push(Vertex *u, ResidualArch *arc) {
    int d = 0;
    if (arc->dest_vertex->getId() == u->getId()) {  // Push back
      d = min(u->excess, arc->flux);
      arc->addFlux(-d);
      arc->origin_vertex->excess += d;
      if (arc->origin_vertex->getId() != 0) {
        stack.push(arc->origin_vertex);
      }
    } else {  // Push front
      d = min(u->excess, arc->getCapacity());
      arc->addFlux(d);
      arc->dest_vertex->excess += d;
      if (arc->dest_vertex->getId() != 1) {
        stack.push(arc->dest_vertex);
      }
    }
    u->excess -= d;

    firstElem = stack.front();
  }

  void discharge(Vertex *u) {
    u->updateArchLessQueue();
    while (u->excess > 0) {
      if (u->archeLessHeight.empty()) {
        relabel(u);
      } else {
        ResidualArch *arc = u->archeLessHeight.front();

        u->archeLessHeight.pop();

        if (arc->getCapacity() > 0 || arc->dest_vertex->getId() == u->getId()) {
          if ((arc->origin_vertex->getId() == u->getId() &&
               u->height > arc->dest_vertex->height) ||
              (arc->dest_vertex->getId() == u->getId() &&
               u->height > arc->origin_vertex->height)) {
            push(u, arc);
          }
        } 
      }
    }
  }

  void run(Graph g) {
    maxHeight = g.suppliers + (2 * g.stations) + 2;

    init_pre_flow(g);
    while (!stack.empty()) {
      firstElem = stack.front();

      stack.pop();

      discharge(firstElem);
      firstElem = stack.front();
    }
  }

  void setupGraph() {
    graph.source->visited = false;
    graph.hyper->visited = false;

    for (int i = 0; i < graph.suppliers + graph.stations; i++) {
      graph.vertexes[i].visited = false;
      graph.vertexes[i].canBeCut = true;
    }
  }

  void resetVisits() {
    for (int i = 0; i < graph.suppliers + (2 * graph.stations); i++) {
      if (!graph.vertexes[i].augmentingVertex)
        graph.vertexes[i].visited = false;
    }
  }

  void saveAllCuts() {
    setupGraph();

    list<Vertex *> stack;
    stack.push_front(graph.source);
    graph.source->visited = true;

    while (!stack.empty()) {
      Vertex *temp = stack.front();
      stack.pop_front();

      if (temp->augmentingVertex) {
        continue;
      }
      for (ResidualArch *arch : temp->arches) {

        if (arch->augmentingArch) {
          if (arch->origin_vertex->isStation()) {
            augmentingStations.insert(temp->getId());

          } else {
            ArchPair newAugmentingArch;
            if (arch->origin_vertex->backArches.front()
                    ->origin_vertex->isStation()) {
              newAugmentingArch.orig =
                  arch->origin_vertex->backArches.front()->origin_vertex;
            } else {
              newAugmentingArch.orig = arch->origin_vertex;
            }
            newAugmentingArch.dest = arch->dest_vertex;
            augmentingArchs.insert(newAugmentingArch);
          }
          continue;
        }
        if (!arch->dest_vertex->visited) {
          arch->dest_vertex->visited = true;
          stack.push_back(arch->dest_vertex);
        }
      }
    }
  }

  void markNextVertices(Vertex *newCutVertex) {
    list<Vertex *> stack;
    stack.push_front(newCutVertex);
    for (ResidualArch *arch : newCutVertex->arches) {
      if (arch->dest_vertex->getId() != 1 && arch->getCapacity() == 0) {
        arch->dest_vertex->canBeCut = false;
        arch->dest_vertex->visited = true;
        arch->dest_vertex->augmentingVertex = false;
      }
      arch->augmentingArch = false;
      arch->canBeCut = false;
    }
  }

  void findGraphCutDFSUtil(Vertex *vertex) {
    vertex->visited = true;
    if (vertex->augmentingVertex) return;
    if (vertex->getId() > 1 && vertex->getId() <= graph.suppliers + 1 &&
        vertex->flow == vertex->production) {
      vertex->augmentingVertex = true;
      resetVisits();
      markNextVertices(vertex);
      findGraphCutDFSUtil(graph.hyper);
      return;
    }
    for (ResidualArch *arch : vertex->backArches) {

      if (!arch->augmentingArch) {
        if (arch->getCapacity() == 0 && arch->origin_vertex->getId() != 0 &&
            arch->canBeCut && !arch->origin_vertex->isStation()) {
          arch->augmentingArch = true;
        } else if (arch->getCapacity() == 0 &&
                   arch->origin_vertex->getId() != 0 &&
                   arch->origin_vertex->isStation() &&
                   arch->origin_vertex->canBeCut) {
          resetVisits();
          arch->augmentingArch = true;
          markNextVertices(vertex);
          findGraphCutDFSUtil(graph.hyper);
        } else if (arch->origin_vertex->getId() != 0 &&
                   !arch->origin_vertex->visited) {
          findGraphCutDFSUtil(arch->origin_vertex);
        }
      }
    }
  }

  void findGraphCutDFS() {
    setupGraph();
    findGraphCutDFSUtil(graph.hyper);
    saveAllCuts();
  }

  void printOutput() {
    bool first = true;
    int flow = graph.getMaxFlow();
    printf("%d\n", flow);
    for (int stationId : augmentingStations) {
      if (first) {
        printf("%d", stationId);
        first = !first;
      } else
        printf(" %d", stationId);
    }
    printf("\n");
    for (ArchPair archPair : augmentingArchs) {
      printf("%d %d\n", archPair.orig->getId(), archPair.dest->getId());
    }
  }
};

int main() {
  Graph g;
  if (g.creatGraphFromStdin() == -1) {
    return -1;
  }
  PushRelabel *algorithm = new PushRelabel(g);
  algorithm->run(g);
  algorithm->findGraphCutDFS();
  algorithm->printOutput();
  // g.display();
  return 0;
}