#include <assert.h>
#include <stdio.h>
#include <climits>
#include <list>
#include <queue>
using namespace std;

class Vertex;

class ResidualArch {
  int capacity;
  int vertex_capacity;

 public:
  int flux;
  Vertex *dest_vertex;
  Vertex *origin_vertex;

  bool processed;

  ResidualArch() : flux(0), processed(false) {}

  ResidualArch(int cap, Vertex *dest) : flux(0), processed(false) {
    capacity = cap;
    dest_vertex = dest;
  }

  ResidualArch(int cap, Vertex *dest, Vertex *origin) : processed(false) {
    capacity = cap;
    dest_vertex = dest;
    origin_vertex = origin;
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

 public:
  int height;
  int excess;

  int production;

  bool visited;
  bool processed;

  Vertex *predecessorVertex;  // if the  predecessor vertex is the source the
                              // vertex is a supplier if not is a station
  ResidualArch *predecessorArch;

  list<ResidualArch *> arches;
  list<ResidualArch *> backArches;
  queue<ResidualArch *> archeLessHeight;
  Vertex() : height(0), excess(0), processed(false) {}

  Vertex(int i) : height(0), excess(0), processed(false) { id = i; }

  Vertex(int i, int h) : excess(0), processed(false) {
    id = i;
    height = h;
  }

  void setId(int i) { id = i; }

  int getId() { return id; }

  // void addFlux(int f) { production = production - f; }

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
      /* printf("|-|-|-|-|| id %d\n", temp->origin_vertex->getId());
      printf("arc->flux %d, arc->getCapacity() %d, height:  %d\n", temp->flux,
             temp->getCapacity(), temp->origin_vertex->height); */

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
};

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
      printf("id: %d -- capacity: %d, excess: %d, --- height: %d\n",
             vertexes[i].getId(), vertexes[i].production, vertexes[i].excess,
             vertexes[i].height);
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

  void printOutput() {
    for (int i = 0; i < suppliers + (2 * stations); i++) {
      if (!vertexes[i].arches.empty())
        for (ResidualArch *arch : vertexes[i].arches) {
          if (arch->dest_vertex->getId() == 1) {
            flow += arch->getFlux();
          }
        }
    }
    printf("%d\n", flow);
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
        stack.push(arch->dest_vertex);
      }
    }
  }

  void relabel(Vertex *u) {
    int min_height = INT_MAX;
    /* printf("RELABEL \n");
    printf("3**** %d\n", u->excess); */
    for (ResidualArch *arc : u->arches) {  // Front Edges
      if (arc->getCapacity() > 0 && arc->dest_vertex->height <= min_height) {
        /* printf(">>>>>>>%d, height: %d, capacity: %d\n",
               arc->dest_vertex->getId(), arc->dest_vertex->height,
               arc->getCapacity()); */
        min_height = arc->dest_vertex->height;
        /* printf("4**** %d\n", u->excess); */
      }
    }

    for (ResidualArch *arc : u->backArches) {  // Back Edges
      if (arc->origin_vertex->height <= min_height && arc->flux > 0) {
        min_height = arc->origin_vertex->height;
      }
      /* printf(
          "<<<<<<< dest: %d, height: %d, min_height: %d,capacity: %d, flux: "
          "%d\n",
          arc->origin_vertex->getId(), arc->origin_vertex->height, min_height,
          arc->getCapacity(), arc->flux); */
    }

    /*  assert(u->excess > 0);
     assert(u->height <= min_height); */
    /*         printf("antes %d\n", min_height);
     */
    u->height = min_height + 1;

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
        /* printf(
            "---- meu %d, orig: %d, dest %d, minha altura: %d, orig height "
            "%d\n",
            u->getId(), arc->origin_vertex->getId(), arc->dest_vertex->getId(),
            u->height, arc->origin_vertex->height); */
        u->archeLessHeight.pop();

        if (arc->getCapacity() > 0 || arc->dest_vertex->getId() == u->getId()) {
          if ((arc->origin_vertex->getId() == u->getId() &&
               u->height > arc->dest_vertex->height) ||
              (arc->dest_vertex->getId() == u->getId() &&
               u->height > arc->origin_vertex->height)) {
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
};

int main() {
  Graph g;
  if (g.creatGraphFromStdin() == -1) {
    return -1;
  }
  PushRelabel *algorithm = new PushRelabel(g);
  algorithm->run(g);
  g.printOutput();
  // g.display();
  return 0;
}