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
  bool processed;

  // ResidualArch *pair;

  ResidualArch() : flux(0), processed(false) {}

  ResidualArch(int cap, Vertex *dest) : flux(0), processed(false) {
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
    // printf("%d\n", capacity);
    flux += f;
    capacity = capacity - f;
    // printf("f: %d\n", flux);
    // printf("%d\n", capacity);
  }

  int getFlux() {
    // printf("%d\n", flux);
    return flux;
  }
};

class Vertex {
 private:
  int id;
  int realId;

 public:
  int height;
  int excess;

  int flow;

  bool station;
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
  queue<ResidualArch *> archeLessHeight;

  Vertex() : height(0), excess(0), flow(0), processed(false) {}

  Vertex(int i) : height(0), excess(0), flow(0), processed(false) { id = i; }

  Vertex(int i, int h) : excess(0), processed(false) {
    id = i;
    height = h;
  }

  void setId(int i) { id = i; }

  int getId() { return id; }

  void setRealId(int i) { realId = i; }

  int getRealId() { return realId; }

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
        /*         printf("&&&&& id : %d altura: %d\n",
         * temp->dest_vertex->getId(), temp->dest_vertex->height);
         */
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
    hyper->processed = true;

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
      vertexes[i + suppliers + stations - 1].setRealId(i + suppliers + 1);
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
          printf("orin: %d -- dest: %d -- fluxo: %d --- processed: %d\n",
                 vertexes[i].getId(), arch->dest_vertex->getId(), arch->flux,
                 arch->processed);
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
    g.source->processed = true;

    for (ResidualArch *arch : g.source->arches) {
      int capacity = arch->getCapacity();
      if (capacity > 0) {
        arch->addFlux(capacity);
        g.source->excess -= capacity;
        // printf("%d\n", arch->getCapacity());
        arch->dest_vertex->excess += capacity;
        // printf("%d\n", arch->dest_vertex->excess);
        arch->dest_vertex->processed = true;
        arch->dest_vertex->addFlux(capacity);
        stack.push(arch->dest_vertex);
      }
    }
  }

  void relabel(Vertex *u) {
    int min_height = INT_MAX;
    for (ResidualArch *arc : u->arches) {  // Front Edges
      if (arc->getCapacity() > 0 && arc->dest_vertex->height <= min_height) {
        /* printf(">>>>>>>%d, height: %d\n", arc->dest_vertex->getId(),
         * arc->dest_vertex->height); */
        min_height = arc->dest_vertex->height;
      }
    }

    for (ResidualArch *arc : u->backArches) {  // Back Edges
      if (arc->origin_vertex->height <= min_height && arc->flux > 0) {
        min_height = arc->origin_vertex->height;
      }
      /* printf("dest: %d, height: %d, min_height: %d\n",
         arc->origin_vertex->getId(), arc->origin_vertex->height, min_height);
       */
    }

    /*  assert(u->excess > 0);
     assert(u->height <= min_height); */
    /*         printf("antes %d\n", min_height);
     */
    u->height = min_height + 1;
    /* printf("minha aluta %d\n", u->height); */

    u->updateArchLessQueue();
  }

  void push(Vertex *u, ResidualArch *arc) {
    int d = 0;
    if (arc->dest_vertex->getId() == u->getId()) {  // Push back
      d = min(u->excess, arc->flux);
      arc->addFlux(-d);
      /* printf("??????? capa %d, d %d \n", arc->getCapacity(), d); */
      arc->origin_vertex->excess += d;
      if (arc->origin_vertex->getId() != 0) {
        /* printf("|?|?|?|?|| id %d\n", arc->origin_vertex->getId());
        printf("u->excess %d, arc->flux %d, arc->getCapacity() %d, DDD: %d\n",
               u->excess, arc->flux, arc->getCapacity(), d); */
        stack.push(arc->origin_vertex);
      }
    } else {  // Push front
      d = min(u->excess, arc->getCapacity());
      /* printf("##### capa %d, d %d \n", arc->getCapacity(), d); */
      arc->addFlux(d);
      arc->dest_vertex->excess += d;
      if (arc->dest_vertex->getId() != 1) {
        /* printf("|#|#|#|#|| id %d\n", arc->origin_vertex->getId()); */
        stack.push(arc->dest_vertex);
        /* printf("///// ALTURA %d \n", u->height); */
      }
    }
    u->excess -= d;
    /* printf("**** %d\n", u->excess); */

    firstElem = stack.front();
  }

  void discharge(Vertex *u) {
    u->updateArchLessQueue();
    while (u->excess > 0) {
      /* printf("+++ +id: %d, excess: %d\n", u->getId(), u->excess); */
      if (u->archeLessHeight.empty()) {
        relabel(u);
        /* printf("id %d\n", u->getId()); */
      } else {
        ResidualArch *arc = u->archeLessHeight.front();
        /*  printf("---- meu %d, orig: %d, dest %d, minha altura: %d, orig
           height %d\n", u->getId(), arc->origin_vertex->getId(),
                arc->dest_vertex->getId(), u->height,
                arc->origin_vertex->height); */
        u->archeLessHeight.pop();

        if (arc->getCapacity() > 0 || arc->dest_vertex->getId() == u->getId()) {
          if ((arc->origin_vertex->getId() == u->getId() &&
               u->height > arc->dest_vertex->height) ||
              (arc->dest_vertex->getId() == u->getId() &&
               u->height > arc->origin_vertex->height)) {
            /* printf("puuuuuusshhh\n"); */
            push(u, arc);
          }
        } /* else {
          printf("popoopopopopo\n");
          u->archeLessHeight.pop();
        } */
      }
    }
  }

  void run(Graph g) {
    maxHeight = g.suppliers + (2 * g.stations) + 2;

    init_pre_flow(g);
    /* printf("------------------------\n"); */
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

    graph.source->predecessorVertex = NULL;
    graph.hyper->predecessorVertex = NULL;

    graph.source->predecessorArch = NULL;
    graph.hyper->predecessorArch = NULL;

    for (int i = 0; i < graph.suppliers + graph.stations; i++) {
      graph.vertexes[i].visited = false;
      graph.vertexes[i].cutVisited = false;
      graph.vertexes[i].canBeCut = true;
      graph.vertexes[i].predecessorVertex = NULL;
      graph.vertexes[i].predecessorArch = NULL;
    }
  }

  void resetVisits() {
    for (int i = 0; i < graph.suppliers + (2 * graph.stations); i++) {
      if (!graph.vertexes[i].augmentingVertex)
        graph.vertexes[i].cutVisited = false;
    }
  }

  void saveAllCuts() {
    setupGraph();

    list<Vertex *> stack;
    stack.push_front(graph.source);
    graph.source->visited = true;

    while (graph.hyper->predecessorVertex == NULL && !stack.empty()) {
      Vertex *temp = stack.front();
      stack.pop_front();
      /*       if (temp->augmentingVertex && temp->getId() > graph.suppliers +
         1) { augmentingStations->insert(temp->getId());
              // printf("---id %d\n", temp->getId());
            }
            if (temp->augmentingVertex) {
              continue;
            } */
      if (temp->augmentingVertex) {
        continue;
      }
      for (ResidualArch *arch : temp->arches) {
        /* printf("----orig %d, dest %d\n", arch->origin_vertex->getId(),
                 arch->dest_vertex->getId()); */
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
    /* printf("id %d mark\n", newCutVertex->getId()); */
    for (ResidualArch *arch : newCutVertex->arches) {
      if (arch->dest_vertex->getId() != 1 && arch->getCapacity() == 0) {
        /* printf("----orig %d, dest %d\n", arch->origin_vertex->getId(),
               arch->dest_vertex->getId()); */
        arch->dest_vertex->canBeCut = false;
        arch->dest_vertex->cutVisited = true;
        arch->dest_vertex->augmentingVertex = false;
      }
      arch->augmentingArch = false;
      arch->canBeCut = false;
    }
  }

  void findGraphCutDFSUtil(Vertex *vertex) {
    vertex->cutVisited = true;
    /* printf("orig id %d, %d, flow: %d cap: %d, \n", vertex->getId(),
           graph.suppliers + 1, vertex->flow, vertex->production); */
    if (vertex->augmentingVertex) return;
    if (vertex->getId() > 1 && vertex->getId() <= graph.suppliers + 1 &&
        vertex->flow == vertex->production) {
      /* printf("vertex augmenint\n"); */
      vertex->augmentingVertex = true;
      resetVisits();
      markNextVertices(vertex);
      findGraphCutDFSUtil(graph.hyper);
      return;
    }
    for (ResidualArch *arch : vertex->backArches) {
      /* printf("orig id %d, dest: %d cap: %d, \n",
         arch->origin_vertex->getId(), arch->dest_vertex->getId(),
         arch->getCapacity()); */
      if (!arch->augmentingArch) {
        if (arch->getCapacity() == 0 && arch->origin_vertex->getId() != 0 &&
            arch->canBeCut && !arch->origin_vertex->isStation()) {
          /* printf("augmenting arch\n"); */
          arch->augmentingArch = true;
        } else if (arch->getCapacity() == 0 &&
                   arch->origin_vertex->getId() != 0 &&
                   arch->origin_vertex->isStation() &&
                   arch->origin_vertex->canBeCut) {
          resetVisits();
          /* printf("---orig %d, dest %d\n", arch->origin_vertex->getId(),
                 arch->dest_vertex->getId()); */
          arch->augmentingArch = true;
          markNextVertices(vertex);
          findGraphCutDFSUtil(graph.hyper);
          /*printf("is vertex station id: %d\n", arch->origin_vertex->getId());
           */
        } else if (arch->origin_vertex->getId() != 0 &&
                   !arch->origin_vertex->cutVisited) {
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