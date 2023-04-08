#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>


using namespace std;

// FUNKCJE ODPOWIADAJĄCE ZA WCZYTYWANIE DO PLIKU
// ###################################################################################################################
int getGraphSize() {
    /*
     * PARAMETRY: brak
     * ZWRACA: liczbę linii w pliku
     * Funkcja służąca do określenia wielkości grafu
     * Z racji, że formatem jest lista następników, to rozmiar jest określany po ilości linii w pliku
     */
    ifstream file("input_graph.txt");
    int number_of_lines = 0;
    string single_line;
    while(getline(file,single_line)){
        number_of_lines++;
    }
    return number_of_lines;
}

vector<vector<int>> readFile(int size) {
    /*
     * PARAMETRY: rozmiar grafu (liczba wierzchołków)
     * ZWRACA: wektor wektorów liczb będący wczytaną listą następników
     * Funkcja służąca do stworzenia właściwej reprezentacji grafu
     * Przekształca każdą linię do postaci strumienia,
     * po czym iteruje taką linię liczb od początku do końca,
     * aby wpisać ją pod właściwy indeks w wektorze wektorów.
     */
    ifstream file("input_graph.txt");
    vector<vector<int>> graph(size);
    int line_counter = 0;
    string single_line;

    while(getline(file,single_line)){
        istringstream streamline(single_line);
        graph[line_counter] = vector<int>(istream_iterator<int>(streamline), istream_iterator<int>());
        line_counter++;
    }
    return graph;
}

void saveToFile(vector<vector<int>> graph) {
    /*
     * PARAMETRY: graf oryginalny przekształcony do listy następników
     * ZWRACA: brak (kod) / plik wyjściowy
     * Funkcja służąca do zapisania grafu do pliku tekstowego
     * Funkcja każdy indeks będący następnikami sprowadza do postaci liczb oddzielonych spacjami
     * za pomocą iteratora
     */
    fstream file;
    file.open("newGraph.txt", ios::out);
    ostream_iterator<int> myIter (file, " ");
    for(const auto& vt : graph) {
        copy(vt.cbegin(), vt.cend(),myIter);
        file << "\n";
    }
}
// ###################################################################################################################

// FUNKCJE ODPOWIADAJĄCE ZA WYŚWIETLANIE
// ###################################################################################################################
void printGraph(vector<vector<int>> graph) {
    /*
     * PARAMETRY: lista następników/poprzedników
     * ZWRACA: brak
     * Funkcja służąca do wyświetlania zawartości przyjętego grafu w postaci komunikatu w konsoli
     * "-->" oddziela wierzchołek pierwotny od następników
     * "," oddziela kolejne następniki. Jeśli po tym nie pojawia się liczba, to znaczy, że nie ma więcej następników
     * "brak" oznacza, że wierzchołek nie ma następników.
     */
    for (int i = 0; i < graph.size(); i++) {
        cout << i << " ---> ";
        if(!graph[i].empty()){
            for(auto j: graph[i]) {cout << j << ", ";}
        } else {cout << "brak";}
        cout << "\n";
    }
}

void printSuccPred(vector<vector<int>> succ, vector<vector<int>> pred){
    /*
     * PARAMETRY: lista następników i poprzedników
     * ZWRACA: brak
     * Wrapper dla wyświetlania
     */
    cout << "READ GRAPH:\n";
    printGraph(succ);
    cout << "\n";
    cout << "LIST OF PREDECESSORS\n";
    printGraph(pred);
    cout << "\n";
}

void printEdge (vector<pair<int,int>> edges) {
    /*
     * PARAMETRY: graf pod postacią listy krawędzi
     * ZWRACA: brak
     * Funkcja służąca do wyświetlania listy krawędzi ukazanych jako wektor par w postaci komunikatu w konsoli
     */
    for (int original_vert = 0; original_vert < edges.size(); original_vert++)  {
        cout << "Verticle in Adjoint: " << original_vert << " ---> " "(" << edges[original_vert].first << "," << edges[original_vert].second << ")";
        cout << "\n";
    }
}

// ###################################################################################################################

vector<vector<int>> createPredecessors(vector<vector<int>> graph) {
    /*
     * PARAMETRY: lista następników
     * ZWRACA: wektor wektorów liczb będący listą poprzedników.
     * Funkcja służąca do stworzenia listy poprzedników, potrzebnej do zbadania liniowości
     * Funkcja przechodzi graf wierzchołek po wierzchołku by w nowym wektorze wektorów, pod indeks odpowiadający następnikowi, dorzucić wierzchołek pierwotny
     */
    vector<vector<int>> pred(graph.size());
    for (int vert = 0; vert < graph.size(); vert++) {
        for (int successor: graph[vert]) {
            pred[successor].push_back(vert);
        }
    }
    return pred;
}

// FUNKCJE ODPOWIADAJĄCE ZA KWALIFIKACJE GRAFU
// ###################################################################################################################
bool is1Graph(vector<vector<int>> graph) {
    /*
     * PARAMETRY: lista następników
     * ZWRACA: werdykt czy graf jest 1-grafem
     * 1-Graf to taki, który nie posiada krawędzi wielokrotnych.
     * Funkcja sprawdza wszystkie następniki danego wierzchołka, poprzez adjacent_find().
     * Jeśli w danych następnikach występuje taka sama wartość, to iterator zwróci pierwsze napotkanie powtórzenia,
     * a zatem jeśli iterator znajduje się na innej pozycji niż iteracyjny koniec wektora, to werdykt jest negatywny
     */
    for (auto & vert_successors : graph) {
        if (!vert_successors.empty()) {
            if (adjacent_find(vert_successors.begin(), vert_successors.end()) != vert_successors.end()) {
                return false;
            }
        }
    }
    return true;
}

bool isAdjoint(vector<vector<int>> graph) {
    /*
     * PARAMETRY: lista następników
     * ZWRACA: werdykt, czy graf jest sprzężony.
     * Graf sprzężony jest, wtedy kiedy:
     * a) następniki dowolnej pary wierzchołków nie mają części wspólnej,
     * b) jeśli jednak mają część wspólną, to wszystkie następniki tej pary muszą być równe sobie.
     * Funkcja zatem analizuje ten warunek za pomocą podwójnej pętli, tworząc wszystkie możliwe kombinacje par, po czym za pomocą set_intersection
     * tworzy wektor części wspólnych
     */
    for (int i = 0; i < graph.size()-1; i++) {
        for (int j = i+1; j < graph.size(); j++) {
            vector<int> possible_intersection;
            set_intersection(graph[i].begin(),graph[i].end(),graph[j].begin(),graph[j].end(), back_inserter(possible_intersection));
            if (!possible_intersection.empty() && !graph[i].empty() && !graph[j].empty()) {
                if (graph[i] != graph[j]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isLinear(vector<vector<int>> succ, vector<vector<int>> pred) {
    /*
     * PARAMETRY: lista następników i poprzedników
     * ZWRACA: werdykt, czy graf jest liniowy.
     * Funkcja podobna w działaniu do isAdjoint(), z tą różnicą, że dodatkowo dochodzi badanie części wspólnych dla poprzedników, w przypadku części wspólnych dla następników
     */
    for (int i = 0; i < succ.size()-1; i++) {
        for (int j = i+1; j < succ.size(); j++) {
            vector<int> possible_intersection_succ;
            set_intersection(succ[i].begin(),succ[i].end(),succ[j].begin(),succ[j].end(), back_inserter(possible_intersection_succ));
            if (!possible_intersection_succ.empty() && !succ[i].empty() && !succ[j].empty()) {
                vector<int> possible_intersection_pred;
                set_intersection(pred[i].begin(),pred[i].end(),pred[j].begin(),pred[j].end(), back_inserter(possible_intersection_pred));
                if (succ[i] != succ[j] || !possible_intersection_pred.empty() ) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool grantAccessToFurtherWork(vector<vector<int>> succ, vector<vector<int>> pred){
    /*
     * PARAMETRY: lista następników i poprzedników
     * ZWRACA: werdykt, czy należy transformować graf do postaci oryginalnej
     * Funkcja pełni rolę wrappera dla is1Graph(), isAdjoint() i isLinear()
     * Dodatkowo wyświetla odpowiednie komunikaty i wydaje flagę, która decyduje, czy kontynuować działanie programu
     */
    bool flag;
    bool one_graph_res = is1Graph(succ);
    if(one_graph_res){
        cout << "Graph is 1-Graph ";
        bool adjoint_res = isAdjoint(succ);
        if(adjoint_res){
            cout << "and Adjoint (thus it qualifies graph to further transformation) ";
            flag = true;
            bool linear_res = isLinear(succ,pred);
            if(linear_res){cout << "and Linear\n";}
            else {cout << "BUT NOT Linear\n";}
        } else {
            cout << "BUT NOT adjoint thus it does not qualify to further transformation";
            cout << "\n";
            flag = false;
        }
    } else {
        cout << "Graph is not 1-Graph thus it does not qualify to further transformation";
        cout << "\n";
        flag = false;
    }
    return flag;
}
// ###################################################################################################################

// FUNKCJE ODPOWIADAJĄCE ZA TWORZENIE GRAFU ORYGINALNEGO
// ###################################################################################################################

void correction(vector<pair<int,int>> &edges, vector<vector<int>>graph){
    /*
     * PARAMETRY: lista krawędzi i lista następników
     * ZWRACA "skorygowaną listę połączonych krawędzi"
     * Działanie teoretyczne opisane w pairEdges (ZAAWANSOWANA)
     * Funkcja iteruje po liście następników, wynajdując wierzchołki z takimi samymi następnikami
     * Następnie, analizuje wszystkie reprezentacje krawędziowe znajdujące się pod znalezionymi wierzchołkami oraz zapamiętuje reprezentacje ostatniego następnika,
     * Na koniec nadpisuje końce reprezentacji krawędziowej tych indeksów, które nie miały szans zostać zamienione prawidłowo w podstawowym parowaniu.
     */
    vector<int>targeted_verts;
    for (int i=0; i < graph.size()-1; i++){
        for (int c=i+1; c < graph.size(); c++) {
            if (graph[c] == graph[i] && !graph[i].empty()) {
                targeted_verts.push_back(i);
                targeted_verts.push_back(c);
                int last_successor = graph[i].back();
                for (int inconsistent_vert:targeted_verts) {
                    if (edges[inconsistent_vert].second != edges[last_successor].first) { //&& edges[inconsistent_vert].second != edges[inconsistent_vert + 1].second) {
                        edges[inconsistent_vert].second = edges[last_successor].first;
                    }
                }
            }
        }
        targeted_verts.clear();
    }
}
void pairEdges(vector<pair<int,int>> &edges, vector<vector<int>>graph) {
    /*
     * PARAMETRY: lista niepołączonych krawędzi i lista następników
     * ZWRACA: "połączone prawidłowo krawędzie" ("" ponieważ jest to void())
     * Funkcja działa na dwóch płaszczyznach:
     * PODSTAWOWA: łączy ze sobą dwie krawędzie poprzez zależność, że początek krawędzi następnika, musi być końcem krawędzi wierzchołka pierwotnego
     * ZAAWANSOWANA: W wypadku, gdy wierzchołki mają identyczne następniki, to występuje wielokrotne nadpisywanie tych samych punktów. Korekta obejmuje znalezienie wartości przed ostatnim nadpisaniem i sprowadzenie ich do obecnej, ostatniej wartości
     */
    for (int original_vert = 0; original_vert < graph.size(); original_vert++) {
        for (int successor: graph[original_vert]) {
            edges[successor].first = edges[original_vert].second;
        }
    }
    correction(edges, graph);
}

void handleBeginningOfEdge(vector<pair<int,int>> &edges, int current_edge, int &tracker, vector<int>&already_used){
    /*
     * PARAMETRY: graf oryginalny jako lista krawędzi, marker obecnego wierzchołka, licznik reindeksujący, wektor kontrolujący użyte wartości
     * ZWRACA: "Przeindeksowany graf w odniesieniu do wartości początku krawędzi"
     * Funkcja działa na trzech frontach
     * a) Jeśli wartość początku jest wyższa niż licznik, to zapamiętaj tą wartość i przejdź całą listę by odpowiednio pozamieniać wartości
     * b) Jeśli wartość już została użyta, to zachowaj licznik taki jaki jest
     * c) Jeśli wartość jest równa/mniejsza licznikowi, to zwiększ licznik i zapamiętaj obecną wartość
     */
    if (edges[current_edge].first > tracker) {
        int rem = edges[current_edge].first;
        edges[current_edge].first = tracker;
        if (edges[current_edge].second == rem) {
            edges[current_edge].second = tracker;
        }
        for (int next_edge= current_edge + 1; next_edge < edges.size(); next_edge++) {
            if (edges[next_edge].first == rem) {
                edges[next_edge].first = tracker;
            } if (edges[next_edge].second == rem) {
                edges[next_edge].second = tracker;
            }
        }
        already_used.push_back(tracker);
        tracker++;
    } else if (find(already_used.begin(),already_used.end(),edges[current_edge].first) != already_used.end()){
        tracker = tracker;
    } else {
        tracker++;
        already_used.push_back(edges[current_edge].first);
    }
}


void handleEndingOfEdge(vector<pair<int,int>> &edges, int current_edge, int &tracker, vector<int>&already_used){
    /*
     * PARAMETRY: graf oryginalny jako lista krawędzi, marker obecnego wierzchołka, licznik reindeksujący, wektor kontrolujący użyte wartości
     * ZWRACA: "Przeindeksowany graf w odniesieniu do wartości początku krawędzi"
     * Funkcja działa na trzech frontach
     * a) Jeśli wartość początku jest wyższa niż licznik, to zapamiętaj tą wartość i przejdź całą listę by odpowiednio pozamieniać wartości
     * b) Jeśli wartość już została użyta, to zachowaj licznik taki jaki jest
     * c) Jeśli wartość jest równa/mniejsza licznikowi, to zwiększ licznik i zapamiętaj obecną wartość
     */
    if (edges[current_edge].second > tracker) {
        int rem = edges[current_edge].second;
        edges[current_edge].second = tracker;
        for (int next_edge = current_edge + 1; next_edge < edges.size(); next_edge++) {
            if (edges[next_edge].first == rem) {
                edges[next_edge].first = tracker;
            } if (edges[next_edge].second == rem) {
                edges[next_edge].second = tracker;
            }
        }
        already_used.push_back(tracker);
        tracker++;
    } else if (find(already_used.begin(),already_used.end(),edges[current_edge].second) != already_used.end()){
        tracker = tracker;
    } else {
        tracker++;
        already_used.push_back(edges[current_edge].second);
    }
}

void reindexEdges(vector<pair<int,int>> &edges) {
    /*
     * PARAMETRY: Graf oryginalny w postaci łuków
     * ZWRACA: "Przeindeksowany graf oryginalny w postaci łuków"
     * Wrapper dla handleBeginning/EndingOfEdge()
     */
    vector<int>usedValues;
    int reindexing_value = 0;
    for (int i=0; i < edges.size(); i++) {
        handleBeginningOfEdge(edges, i, reindexing_value, usedValues);
        handleEndingOfEdge(edges, i, reindexing_value, usedValues);
    }
}


vector<pair<int,int>> createListOfEdges(vector<vector<int>> graph) {
    /*
     * PARAMETRY: lista następników
     * ZWRACA w pełni przygotowaną listę krawędzi
     * Funkcja głównie pełni rolę wrappera dla pairEdges(), reindexEdges() i printEdge()
     * Funkcja przygotowuje listę rozłącznych krawędzi, które następnie są posyłane i transformowane dalej
     */
    vector<pair<int, int>> edge_list(graph.size());
    int counter = 0;
    for (int i = 0; i < graph.size(); i++) {
        edge_list[i] = make_pair(counter, counter + 1);
        counter = counter + 2;
    }
    cout << "Edges before pairing:" << endl;
    printEdge(edge_list);
    cout << "\n";
    pairEdges(edge_list, std::move(graph));
    cout << "Pairs before reindexing: " << endl;
    printEdge(edge_list);
    cout << "\n";
    reindexEdges(edge_list);
    cout << "Pairs after reindexing" << endl;
    printEdge(edge_list);
    cout << "\n";
    return edge_list;
}

int possibleSize(vector<pair<int,int>> edges) {
    /*
     * PARAMETRY: Przeindeksowany graf oryginalny w postaci listy krawędzi
     * ZWRACA: Nowy rozmiar dla grafu oryginalnego w postaci listy następników
     * Funkcja znajduje największą wartość w liście krawędzi/par, ponieważ ona indukuje rozmiar grafu, gdzie posługiwanie się starym rozmiarem mogłoby być niepotrzebne
     */
    vector<int>allValues;
    for (auto & edge : edges) {
        allValues.push_back(edge.first);
        allValues.push_back(edge.second);
    }
    int size = *max_element(allValues.begin(),allValues.end());
    return size+1;
}

vector<vector<int>> createOriginalAsSuccessors(vector<pair<int,int>> edges){
    /*
     * PARAMETRY: przeindeksowany graf oryginalny w postaci listy łuków
     * ZWRACA: graf oryginalny w postaci listy następników
     * Funkcja tworzy pusty wektor wektorów liczb będący przestrzenią do uzupełnienia.
     * Uzupełnianie odbywa się poprzez dodanie do wektora, będącego pod indeksem początku krawędzi, wartości będącej końcem krawędzi.
     */
    int new_size = possibleSize(edges);
    vector<vector<int>> original_graph(new_size);
    for(int single_edge=0; single_edge < edges.size(); single_edge++){
        original_graph[edges[single_edge].first].push_back(edges[single_edge].second);
    }
    return original_graph;
}

int main() {
    int graph_size = getGraphSize();
    vector<vector<int>> successors = readFile(graph_size);
    vector<vector<int>> predecessors = createPredecessors(successors);
    printSuccPred(successors,predecessors);
    bool qualifications = grantAccessToFurtherWork(successors,predecessors);
    if(!qualifications){
        cout << "Program ended!\n";
        exit(0);
    } else {
        cout << "BEGINNING TRANSFORMATION\n";
        vector<pair<int,int>> original_graph_as_edges = createListOfEdges(successors);
        vector<vector<int>> original_graph_as_successors = createOriginalAsSuccessors(original_graph_as_edges);
        cout << "Original graph as list of successors:\n";
        printGraph(original_graph_as_successors);
        saveToFile(original_graph_as_successors);
    }
    return 0;
}
