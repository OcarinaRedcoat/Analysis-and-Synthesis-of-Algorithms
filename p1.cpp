#include <iostream>
#include <list>

#define WHITE 0
#define GREY 1
#define BLACK 2

using namespace std;

class Vertex{
    private:
        int _id;
    public:
        int _d;
        int _low;
        int _color;
        list<Vertex *> _cons;

        Vertex(){
            _d = -1;
            _color = WHITE;
        }

        void set(int id){
            _id = id;
        }
        int get(){
            return _id;
        }
        void addCon(Vertex *vertex){
            _cons.push_back(vertex);  
        }

        void display(){
            list<Vertex *>::iterator it;
            for (it = _cons.begin(); it != _cons.end(); ++it){
                printf("%d - ", (*it)->get());
            }
        }
};

class Graph{
    private:
        int _n;
        int _m;
    public:
        Vertex *vertexes;

        Graph(){ //Constructor
            vertexes = NULL;
        }

        ~Graph(){ //Destructor
            delete[] vertexes;
        }

        int creatGraph(){
            scanf("%d", &_n);
            scanf("%d", &_m);

            if (_n < 2 || _m < 1){
                return -1;
            }

            vertexes = new Vertex[_n];

            for (int i = 0; i < _n; i++){
                vertexes[i].set(i + 1);
            }

            for (int i = 1; i <= _m; i++){
                int first;
                int second;
                scanf("%d", &first);
                scanf("%d", &second);
                vertexes[first -1].addCon(&vertexes[second - 1]); //bi-directional graph
                vertexes[second - 1].addCon(&vertexes[first - 1]); //bi-directional graph
            }
            return 0;
        }

        int getNumberVertexes(){
            return _n;
        }

        void dislay(){
            for (int i = 0; i < _n; i++){
                printf("%d: ", vertexes[i].get());
                vertexes[i].display();
                printf("NULL\n");
            }
        }
};



int main(int argc, char **argv){

    Graph graph;

    if (graph.creatGraph() == -1){
        return -1;
    }
    graph.dislay();
    
    return 0;

}