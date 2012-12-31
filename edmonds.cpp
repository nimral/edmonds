#include <cstdio>
#include <vector>

#define even(x) (((x) % 2) == 0)

using namespace std;

class hrana
{
public:
    int kam;
    bool parovaci;
    hrana(int k) : kam(k), parovaci(false) {}
};

class graf
{
public:
    vector<int> zacatky;
    vector<int> hladiny;
    vector<int> koren;
    vector<hrana> hrany;
    vector<int> rodic_hrana;
    int n;
};

graf nacti_graf()
{
    graf G;

    int n;
    scanf("%d",&n);

    G.n = n;

    for (int i = 0; i < n; i++) {
        int deg;
        scanf("%d",&deg);
        G.zacatky.push_back(G.hrany.size());
        G.hladiny.push_back(-1);
        G.koren.push_back(-1);
        G.rodic_hrana.push_back(-1);

        for (int j = 0; j < deg; j++) {
            int kam;
            scanf("%d",&kam);
            G.hrany.push_back(hrana(kam));
        }
    }
    G.zacatky.push_back(G.hrany.size());

    return G;
}
            
            
int edmonds(graf& G, int vel_parovani)
{
    deque fronta;

    for (int i = 0; i < G.n; i++) {
        bool volny = true;
        for (int j = G.zacatky[i]; j < G.zacatky[i+1]; j++) {
            if (G.hrany[j].parovaci) {
                volny = false;
                break;
            }
        }
        if (volny) {
            fronta.push_back(i);
            G.hladiny[i] = 0;
            G.koren[i] = i;
        }
    }

    while (!fronta.empty()) {
        int v = fronta[0];
        fronta.pop_front();

        for (int e = G.zacatky[v]; e < G.zacatky[v+1]; e++) {
            int w = G.hrany[e].kam;

            if (G.hladiny[v] % 2 == 0) {
                if (G.koren[w] == -1) {
                    if (!G.hrany[e].parovaci) {
                        G.koren[w] = G.koren[v];
                        G.hladiny[w] = G.hladiny[v]+1;
                        fronta.push_back(w);
                    }
                } else {
                    if (G.koren[w] != G.koren[v] && even(G.hladiny[w])) {
                        //musí být nepárovací
                        //našli jsme augmenting path
                        G.hrany[e].parovaci = true;
                        for (int f = G.rodic_hrana[v]; f > -1;
                        f = G.rodic_hrana[G.hrany[f].kam]) {
                            G.hrany[f].parovaci = !G.hrany[f].parovaci;
                        }
                        for (int f = G.rodic_hrana[w]; f > -1;
                        f = G.rodic_hrana[G.hrany[f].kam]) {
                            G.hrany[f].parovaci = !G.hrany[f].parovaci;
                        }
                        //return ?
                    } else if (G.koren[w] == G.koren[v] && even(G.hladiny[w])) {
                        //našli jsme květ

                        //najdeme začátek stonku
                        G.hladiny[v] = -2;
                        G.hladiny[w] = -2;

                        for (int f = G.rodic_hrana[v]; f > -1;
                        f = G.rodic_hrana[G.hrany[f].kam]) {
                            G.hladiny[G.hrany[f].kam] = -2;
                        }

                        int u = w;
                        for (int f = G.rodic_hrana[w]; f > -1;
                        f = G.rodic_hrana[G.hrany[f].kam]) {
                            if (G.hladiny[f.kam] == -2) {
                                u = f.kam;
                                break;
                            }
                        }
                        

                        //kontrahujeme květ
                        for (int x = v; ; x = G.hrany[G.rodic_hrana[x]].kam) {
                            for (int f = G.zacatky[x]; f < G.zacatky[x+1]; f++) {
                                G.hrany[f].kam
                       
                                

                        






                        

                }

}

int main()
{
    graf G = nacti_graf();

    int minule = 0;
    while (true) {
        int velikost = edmonds(G,minule);
        if (velikost == minule) {
            break;
        }
        minule = velikost;
    }

    vypis_parovani(G);
}
