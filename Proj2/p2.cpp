#include <stdio.h>
#include <list>
#include <climits>
using namespace std;

class Vertex;

class ResidualArch{
    int capacity;
  public:
    int flux;

    Vertex *dest_vertex;
    ResidualArch *pair;

    ResidualArch(){}

    ResidualArch(int cap, Vertex *dest){
      capacity = cap;
      dest_vertex = dest;
    }

    int getCapacity(){
      return capacity;
    }

        void addFlux(int f)
    {
        flux += f;
        capacity = capacity - flux;
    }

};

class Vertex{
  private:
    int id;
  public:
    int production;

    bool visited;
    bool processed;

    Vertex *predecessorVertex; // if the  predecessor vertex is the source the vertex is a supplier if not is a station
    ResidualArch *predecessorArch;

    list<ResidualArch *> arches; 
    Vertex(){ // probably source and source has no predecessor Vertex or predecessor Arch
      // predecessorVertex = NULL;
      // predecessorArch = NULL;
    }

    Vertex(int i){ 
      id = i; 
    }

    void setId(int i){
      id = i;
    }

    int getId(){
      return id;
    }

    void addResidualArch(int cap, Vertex *dest){
      ResidualArch *temp = new ResidualArch(cap, dest);
      arches.push_back(temp); 
    }
  
};

class Graph{
  public:
    int suppliers;
    int stations;
    int connections;

    Vertex *source;
    Vertex *hyper; //sink
    Vertex *vertexes;

    Graph(){
      vertexes = NULL;
    }

    int creatGraphFromStdin(){

      scanf("%d %d %d", &suppliers, &stations, &connections);
      if (suppliers <= 0 || stations < 0 || connections < 0){
        return -1;
      }

      vertexes = new Vertex[suppliers+stations+2];
      source = new Vertex(0);
      hyper = new Vertex(1); //sink

      for (int i = 1; i <= suppliers; i++){
        vertexes[i - 1].setId(i + 1);
        scanf("%d", &vertexes[i - 1].production);
        source->addResidualArch(vertexes[i - 1].production, &vertexes[i - 1]);
      }

      for (int i = 1; i <= stations; i++){
        vertexes[i + suppliers - 1].setId(i + suppliers + 1);
        scanf("%d", &vertexes[i + suppliers - 1].production);
      }

      for (int i = 1; i <= connections; i++){
        int origin, destiny, capacity;
        scanf("%d %d %d", &origin, &destiny, &capacity);
        if (destiny == 1){
          vertexes[origin - 2].addResidualArch(capacity , hyper);
        }else{
          vertexes[origin - 2].addResidualArch(capacity , &vertexes[destiny - 2]);
        }
      }
      return 0;
    }
};

class BFS{
  public:
    void setupGraph(Graph g){
        g.source->visited = false;
        g.hyper->visited = false;

        g.source->predecessorVertex = NULL;
        g.hyper->predecessorVertex = NULL;

        g.source->predecessorArch = NULL;
        g.hyper->predecessorArch = NULL;

        for (int i = 0; i < g.suppliers + g.stations; i++){
          g.vertexes[i].visited = false;
          g.vertexes[i].predecessorVertex = NULL;
          g.vertexes[i].predecessorArch = NULL;
        }
    }

    list<ResidualArch *> execute(Graph g){
      setupGraph(g);

      list<Vertex *>stack;
      stack.push_front(g.source);
      g.source->visited = true;

      while (g.hyper->predecessorVertex == NULL && !stack.empty()){
        
        Vertex *temp = stack.front();
        stack.pop_front();

        for (ResidualArch *arch: temp->arches){

          if (!arch->dest_vertex->visited && arch->getCapacity() > 0){

            stack.push_back(arch->dest_vertex);
            arch->dest_vertex->predecessorVertex = temp;
            arch->dest_vertex->predecessorArch = arch;
            arch->dest_vertex->visited = true;

            if (arch->dest_vertex == g.hyper){
              break;
            }

          }
        }
      }

      list<ResidualArch *> resultBFS;

      if (g.hyper->visited == false){
        return resultBFS;
      }
      else{
        Vertex *temp = g.hyper;
        while (temp->predecessorArch != NULL){
          resultBFS.push_front(temp->predecessorArch);
          temp = temp->predecessorVertex;
        }
        return resultBFS;
      }
    }
};

class EdmondsKarp{
  private:
    int flow;
  public:
    EdmondsKarp(){}

    int execute(Graph g){
      flow = 0;

      while (true){
        BFS bfsAlgorithm;

        list<ResidualArch *> path = bfsAlgorithm.execute(g);

        if (path.size() != 0){
          //check how much flow we can send
          int df = INT_MAX;
          for (ResidualArch *arch : path){
            if(arch->dest_vertex->predecessorVertex->getId() > 2*g.stations+1){
              df = min(df, arch->dest_vertex->production);
            }else{
              df = min(df, arch->getCapacity());
            }
          }
          for (ResidualArch *arch : path){
            if(arch->dest_vertex->predecessorVertex->getId() > 2*g.stations+1){
              arch->addFlux(df);
            }else{
              arch->addFlux(df);
              //arc->pair->addFlux(-df);
              flow += df;
          }
          }
        }
          else{
          printf("%d\n", flow);
          return flow;
          }
        }
      }
    
};



int main(){

  Graph g;
  if(g.creatGraphFromStdin() == -1){
    return -1;
  }
  EdmondsKarp karp;
  karp.execute(g);
  return 0;

}