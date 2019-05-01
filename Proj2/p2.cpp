#include <stdio.h>
#include <list>

using namespace std;

class ResidualArch{
  public:
    int capacity;

    ResidualArch(){}

    ResidualArch(int cap){
      capacity = cap;
    }
};

class Vertex{
  public:
    int production;
    int id;
    
    Vertex *predecessorVertex;
    ResidualArch *predecessorArch;

    list<ResidualArch *> arches; 
    Vertex(){}

    Vertex(int i){
      id = i;
    }

    Vertex(int i, int prod){ 
      id = i; 
      production = prod; 
    }

    void setId(int i){
      id = i;
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
      scanf("%d %d %d", &suppliers, stations, connections);
      if (suppliers <= 0 || stations < 0 || connections < 0){
        return -1;
      }

      vertexes = new Vertex[suppliers+stations+2];
      source = new Vertex();
      hyper = new Vertex(1); //sink

      for (int i = 1; i <= suppliers; i++){
        Vertex *newSupplier = new Vertex(i + 1);
        scanf("%d", &newSupplier->production);
      }

      for (int i = 1; i <= stations; i++){
        Vertex *newStation = new Vertex(suppliers + i + 1);
        scanf("%d", &newStation->production);
      }

      return 0;
    }

};

int main(){
  Graph g;

  if(g.creatGraphFromStdin() == -1){
    return -1;
  }



}