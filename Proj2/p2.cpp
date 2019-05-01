#include <stdio.h>



class Vertex{
  public:
    int production;
    int id;

    Vertex(){}

    void setId(int i){
      id = i;
    }
  
};

class Graph{
  public:
    int suppliers;
    int stations;
    int connections;
    Vertex *vertexes;

    Graph(){
      vertexes = NULL;
    }

    int creatGraphFromStdin(){
      scanf("%d %d %d", &suppliers, stations, connections);
      if (suppliers <= 0 || stations < 0 || connections < 0){
        return -1;
      }

      vertexes = new Vertex[suppliers+stations+1];

      Vertex *hyper = new Vertex();
      hyper->setId(1);

      for (int i = 1; i <= suppliers; i++){
        Vertex *newSupplier = new Vertex();
        newSupplier->setId(i + 1);
        scanf("%d", &newSupplier->production);
      }

      for (int i = 1; i <= stations; i++){
        Vertex *newStation = new Vertex();
        newStation->setId(suppliers + i + 1);
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