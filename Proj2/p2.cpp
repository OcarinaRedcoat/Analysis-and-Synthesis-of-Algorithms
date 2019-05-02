#include <stdio.h>
#include <list>

using namespace std;

class Vertex;

class ResidualArch{
  public:
    int capacity;

    ResidualArch(){}

    ResidualArch(int cap){
      capacity = cap;
    }


};

class Vertex{
  private:
    int id;
  public:
    int production;

    Vertex *predecessorVertex;
    ResidualArch *predecessorArch;

    list<ResidualArch *> arches; 
    Vertex(){ // probably source and source has no predecessor Vertex or predecessor Arch
      predecessorVertex = NULL;
      predecessorArch = NULL;
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

    void addResidualArch(int cap){
      ResidualArch *temp = new ResidualArch(cap); //TODO: missing capacity ask teacher
      arches.push_back(temp); //
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
      source = new Vertex();
      hyper = new Vertex(1); //sink

      for (int i = 1; i <= suppliers; i++){
        vertexes[i].setId(i +1);
        scanf("%d", &vertexes[i].production);
        source->addResidualArch(vertexes[i].getId());
      }

      for (int i = 1; i <= stations; i++){
        vertexes[i + suppliers].setId(i + suppliers + 1);
        scanf("%d", &vertexes[i + suppliers].production);
      }

      for (int i = 1; i <= connections; i++){
        int origin, destiny, capacity;
        scanf("%d %d %d", &origin, &destiny, &capacity);
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