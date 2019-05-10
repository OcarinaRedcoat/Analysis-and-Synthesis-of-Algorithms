#include <stdio.h>
#include <climits>
#include <list>
#include <set>
using namespace std;

class Vertex;

typedef struct {
  Vertex *orig, *dest;
} ArchPair;

class ResidualArch {
  int capacity;

 public:
  Vertex *dest_vertex;
  Vertex *orig_vertex;
  bool canBeCut;
  bool augmentingArch;
  // ResidualArch *pair;

  ResidualArch() {}

  ResidualArch(int cap, Vertex *dest, Vertex *orig) {
    capacity = cap;
    dest_vertex = dest;
    orig_vertex = orig;
    canBeCut = true;
  }

  int getCapacity() { return capacity; }

  void addFlux(int f) { capacity = capacity - f; }
};

class Vertex {
 private:
  int id;

 public:
  int production;
  bool cutVisited;
  bool canBeCut;
  bool augmentingVertex;
  bool visited;
  bool processed;

  Vertex *predecessorVertex;  // if the  predecessor vertex is the source the
                              // vertex is a supplier if not is a station
  ResidualArch *predecessorArch;

  list<ResidualArch *> backArches;
  list<ResidualArch *> arches;
  Vertex() {  // probably source and source has no predecessor Vertex or
              // predecessor Arch
    // predecessorVertex = NULL;
    // predecessorArch = NULL;
  }

  Vertex(int i) { id = i; }

  void setId(int i) { id = i; }

  int getId() { return id; }

  void addFlux(int f) { production = production - f; }

  void addResidualBackArch(ResidualArch *residualArch) {
    backArches.push_back(residualArch);
  }

  void addResidualArch(int cap, Vertex *dest) {
    ResidualArch *temp = new ResidualArch(cap, dest, this);
    dest->addResidualBackArch(temp);
    arches.push_back(temp);
  }
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

  Graph() { vertexes = NULL; }

  int creatGraphFromStdin() {
    scanf("%d %d %d", &suppliers, &stations, &connections);
    if (suppliers <= 0 || stations < 0 || connections < 0) {
      return -1;
    }

    vertexes = new Vertex[suppliers + stations];
    source = new Vertex(0);
    hyper = new Vertex(1);  // sink

    for (int i = 1; i <= suppliers; i++) {
      vertexes[i - 1].setId(i + 1);
      scanf("%d", &vertexes[i - 1].production);
      source->addResidualArch(vertexes[i - 1].production, &vertexes[i - 1]);
    }

    for (int i = 1; i <= stations; i++) {
      vertexes[i + suppliers - 1].setId(i + suppliers + 1);
      scanf("%d", &vertexes[i + suppliers - 1].production);
    }

    for (int i = 1; i <= connections; i++) {
      int origin, destiny, capacity;
      scanf("%d %d %d", &origin, &destiny, &capacity);
      if (destiny == 1) {
        vertexes[origin - 2].addResidualArch(capacity, hyper);
      } else {
        vertexes[origin - 2].addResidualArch(capacity, &vertexes[destiny - 2]);
      }
    }
    return 0;
  }

  void display() {
    for (int i = 0; i < suppliers + stations; i++) {
      printf("id: %d -- capacity: %d\n", vertexes[i].getId(),
             vertexes[i].production);
    }

    for (int i = 0; i < suppliers + stations; i++) {
      if (!vertexes[i].arches.empty())
        for (ResidualArch *arch : vertexes[i].arches) {
          printf("orin: %d -- dest: %d -- capacity: %d\n", vertexes[i].getId(),
                 arch->dest_vertex->getId(), arch->getCapacity());
        }
    }
  }
};

class BFS {
 public:
  set<int> *augmentingStations;
  set<ArchPair> *augmentingArchs;
  void setupGraph(Graph g) {
    g.source->visited = false;
    g.hyper->visited = false;

    g.source->predecessorVertex = NULL;
    g.hyper->predecessorVertex = NULL;

    g.source->predecessorArch = NULL;
    g.hyper->predecessorArch = NULL;

    for (int i = 0; i < g.suppliers + g.stations; i++) {
      g.vertexes[i].visited = false;
      g.vertexes[i].cutVisited = false;
      g.vertexes[i].canBeCut = true;
      g.vertexes[i].predecessorVertex = NULL;
      g.vertexes[i].predecessorArch = NULL;
    }
  }

  void resetVisits(Graph *g) {
    for (int i = 0; i < g->suppliers + g->stations; i++) {
      if (!g->vertexes[i].augmentingVertex) g->vertexes[i].cutVisited = false;
    }
  }

  list<ResidualArch *> execute(Graph g) {
    setupGraph(g);

    list<Vertex *> stack;
    stack.push_front(g.source);
    g.source->visited = true;

    while (g.hyper->predecessorVertex == NULL && !stack.empty()) {
      Vertex *temp = stack.front();
      stack.pop_front();

      for (ResidualArch *arch : temp->arches) {
        if (!arch->dest_vertex->visited && arch->getCapacity() > 0) {
          if (arch->dest_vertex->getId() > g.suppliers + 1 &&
              arch->dest_vertex->production == 0) {
            continue;
          } else {
            stack.push_back(arch->dest_vertex);
            arch->dest_vertex->predecessorVertex = temp;
            arch->dest_vertex->predecessorArch = arch;
            arch->dest_vertex->visited = true;
          }
          if (arch->dest_vertex == g.hyper) {
            break;
          }
        }
      }
    }

    list<ResidualArch *> resultBFS;

    if (g.hyper->visited == false) {
      return resultBFS;
    } else {
      Vertex *temp = g.hyper;
      while (temp->predecessorArch != NULL) {
        resultBFS.push_front(temp->predecessorArch);
        temp = temp->predecessorVertex;
      }
      return resultBFS;
    }
  }

  void saveAllCuts(Graph g, set<int> *augmentingStations,
                   set<ArchPair> *augmentingArchs) {
    setupGraph(g);

    list<Vertex *> stack;
    stack.push_front(g.source);
    g.source->visited = true;

    while (g.hyper->predecessorVertex == NULL && !stack.empty()) {
      Vertex *temp = stack.front();
      stack.pop_front();
      if (temp->augmentingVertex && temp->getId() > g.suppliers + 1) {
        augmentingStations->insert(temp->getId());
        // printf("---id %d\n", temp->getId());
      }
      if (temp->augmentingVertex) {
        continue;
      }
      for (ResidualArch *arch : temp->arches) {
        /* printf("----orig %d, dest %d\n", arch->orig_vertex->getId(),
                 arch->dest_vertex->getId()); */
        if (arch->augmentingArch) {
          ArchPair newAugmentingArch;
          newAugmentingArch.orig = arch->orig_vertex;
          newAugmentingArch.dest = arch->dest_vertex;
          augmentingArchs->insert(newAugmentingArch);

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
        arch->dest_vertex->cutVisited = true;
        arch->dest_vertex->augmentingVertex = false;
      }
      arch->augmentingArch = false;
      arch->canBeCut = false;
    }
  }

  void findGraphCutDFSUtil(Graph g, Vertex *vertex) {
    vertex->cutVisited = true;
    if (vertex->augmentingVertex) return;
    if (vertex->getId() != 0 && vertex->canBeCut && vertex->production == 0) {
      vertex->augmentingVertex = true;
      resetVisits(&g);
      markNextVertices(vertex);
      findGraphCutDFSUtil(g, g.hyper);
      /* printf("id %d\n", vertex->getId()); */
      return;
    }
    for (ResidualArch *arch : vertex->backArches) {
      if (!vertex->augmentingVertex) {
        if (arch->getCapacity() == 0 && arch->orig_vertex->getId() != 0 &&
            arch->canBeCut) {
          /* printf("orig %d, dest %d\n", arch->orig_vertex->getId(),
                 arch->dest_vertex->getId()); */
          arch->augmentingArch = true;
        } else if (arch->orig_vertex->getId() != 0 &&
                   !arch->orig_vertex->cutVisited) {
          findGraphCutDFSUtil(g, arch->orig_vertex);
        }
      }
    }
  }

  void findGraphCutDFS(Graph g) { findGraphCutDFSUtil(g, g.hyper); }
};

class EdmondsKarp {
 private:
  int flow;
  set<int> augmentingStations;
  set<ArchPair> augmementingArchs;

 public:
  EdmondsKarp() {}

  void execute(Graph g) {
    flow = 0;

    BFS bfsAlgorithm;
    while (true) {
      list<ResidualArch *> path = bfsAlgorithm.execute(g);

      if (!path.empty()) {
        int df = INT_MAX;
        for (ResidualArch *arch : path) {
          if (arch->dest_vertex->getId() > g.suppliers + 1) {
            df = min(df, arch->dest_vertex->production);
          }
          df = min(df, arch->getCapacity());
        }
        flow += df;
        for (ResidualArch *arch : path) {
          if (arch->orig_vertex->getId() <= g.suppliers + 1) {
            arch->orig_vertex->addFlux(df);
          }
          if (arch->dest_vertex->getId() > g.suppliers + 1) {
            arch->dest_vertex->addFlux(df);
          }
          arch->addFlux(df);
        }
      } else {
        break;
      }
    }
    // End of while

    bfsAlgorithm.findGraphCutDFS(g);
    bfsAlgorithm.saveAllCuts(g, &augmentingStations, &augmementingArchs);
  }

  void printOutput() {
    bool first = true;
    printf("%d\n", flow);
    for (int stationId : augmentingStations) {
      if (first) {
        printf("%d", stationId);
        first = !first;
      } else
        printf(" %d", stationId);
    }
    printf("\n");
    for (ArchPair archPair : augmementingArchs) {
      printf("%d %d\n", archPair.orig->getId(), archPair.dest->getId());
    }
  }
};

int main() {
  Graph g;
  if (g.creatGraphFromStdin() == -1) {
    return -1;
  }
  EdmondsKarp karp;
  karp.execute(g);
  karp.printOutput();
  // g.display();
  return 0;
}