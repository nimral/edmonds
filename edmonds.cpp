#include <cstdio>
#include <vector>

#define even(x) (((x) % 2) == 0)

using namespace std;

class vrchol
{
public:
    vector<pulhrana*> hrany;
    int cislo;
    bool volny();
}

class pulhrana
{
public:
    pulhrana *druha;
    vrchol *odkud;
    bool parovaci;
}

class graf
{
public:
    vector<vrchol*> V;
};


graf nacti_graf()
{
    graf G;

    int n,m;
    scanf("%d %d",&n,&m);
    for (int i = 0; i < n; i++) {
        G.V.push_back(new vrchol());
        G.V.end()->cislo = i;
    }

    for (int i = 0; i < m; i++) {
        int v,w;
        scanf("%d %d",&v,&w);

        pulhrana *p1 = new pulhrana(G.V[v]);
        pulhrana *p2 = new pulhrana(G.V[w]);
        p1->druha = p2;
        p2->druha = p1;
        p1->parovaci = false;
        p2->parovaci = false;


        G.V[v]->hrany.push_back(p1);
        G.V[w]->hrany.push_back(p2);
    }

    return G;
}

//graf nacti_graf()
//{
    //graf G;

    //int n;
    //scanf("%d",&n);

    //G.n = n;

    //for (int i = 0; i < n; i++) {
        //int deg;
        //scanf("%d",&deg);
        //G.zacatky.push_back(G->hrany.size());
        //G.hladina.push_back(-1);
        //G.koren.push_back(-1);
        //G.rodic_hrana.push_back(-1);

        //for (int j = 0; j < deg; j++) {
            //int kam;
            //scanf("%d",&kam);
            //G->hrany.push_back(hrana(kam));
        //}
    //}
    //G.zacatky.push_back(G->hrany.size());

    //return G;
//}
//

bool vrchol::volny()
{
    bool volny = true;
    for (vector<pulhrana*>::iterator it = hrany.begin(); it != hrany.end(); ++it) {
        if (it->parovaci) {
            volny = false;
            break;
        }
    }
    return volny;
}
    
            
int edmonds(graf& G)
{
    //konstruujeme Edmondsův les procházením do šířky
    deque fronta;
    int hladina[G.V.size()];
    int koren[G.V.size()];
    pulhrana *pulhrana_k_rodici[G.V.size()];

    //v nulté hladině jsou všechny volné vrcholy
    for (int i = 0; i < G.V.size(); i++) {
        if (G.V[i].volny()) {
            fronta.push_back(i);
            hladina[i] = 0;
            koren[i] = i;
        } else {
            hladina[i] = -1;
            koren[i] = -1;
        }
        pulhrana_k_rodici[i] = NULL;
    }

    //procházení do šířky
    while (!fronta.empty()) {
        int v = fronta[0];
        fronta.pop_front();

        for (vector<pulhrana*>::iterator it = G.V[v]->hrany.begin(); it != G.V[v]->hrany.end(); ++it) {
            int w = it->druha.odkud->cislo;
            
            if (even(hladina[v])) {
                //nenavštívený vrchol, přidáme jej do stromu na lichou hladinu
                if (koren[w] == -1) {
                    if (!it->parovaci) {
                        koren[w] = koren[v];
                        hladina[w] = hladina[v]+1;
                        pulhrana_k_rodici[w] = it->druha;
                        fronta.push_back(w);
                    }
                } else { //navštívený vrchol
                    if (koren[w] != koren[v] && even(hladina[w])) {
                        //hrana vede mezi sudými hladinami různých stromů
                        //musí být nepárovací
                        //našli jsme augmenting path

                        //zalternujeme ji
                        it->parovaci = true;
                        it->druha.parovaci = true;

                        for (pulhrana *f = pulhrana_k_rodici[v]; f != NULL;
                        f = pulhrana_k_rodici[f->druha.odkud->cislo]) {
                            f->parovaci = !f->parovaci;
                            f->druha.parovaci = !f->parovaci;
                        }

                        for (pulhrana *f = pulhrana_k_rodici[w]; f != NULL;
                        f = pulhrana_k_rodici[f->druha.odkud->cislo]) {
                            f->parovaci = !f->parovaci;
                            f->druha.parovaci = !f->parovaci;
                        }

                        //zvětšili jsme párování
                        return true;

                    } else if (koren[w] == koren[v] && even(hladina[w])) {
                        //našli jsme květ

                        //najdeme začátek stonku:
                        //změníme hladiny všech vrcholů na cestě od v ke kořeni na -2
                        //pak budeme stoupat od w dokud nenarazíme na vrchol na hladině -2
                        hladina[v] = -2;
                        hladina[w] = -2;

                        for (pulhrana *f = pulhrana_k_rodici[v]; f != NULL;
                        f = pulhrana_k_rodici[f->druha.odkud->cislo]) {
                            hladina[f->druha.odkud->cislo] = -2;
                        }

                        vrchol *u;
                        for (pulhrana *f = pulhrana_k_rodici[w]; f != NULL;
                        f = pulhrana_k_rodici[f->druha.odkud->cislo]) {
                            if (hladina[f->druha.odkud->cislo] == -2) {
                                u = f->druha.odkud;
                                break;
                            }
                        }
                        

                        //vytvoříme graf G2, ve kterém bude nalezený květ zkontrahovaný
                        //budeme kopírovat graf a vypouštět vrcholy
                        
                        int nove_cislo[G.V.size()];
                        for (int i = 0; i < G.V.size(); i++) {
                            nove_cislo[i] = -1;
                        }


                        //květ bude v novém grafu na pozici 0
                        nove_cislo[v] = nove_cislo[w] = 0;

                        pulhrana *f = pulhrana_k_rodici[v];
                        while (true) {
                            nove_cislo[f->druha.odkud->cislo] = 0;
                            if (f->druha.odkud == u) {
                                break;
                            }
                            f = pulhrana_k_rodici[f->druha.odkud->cislo];
                        }

                        *f = pulhrana_k_rodici[w];
                        while (true) {
                            nove_cislo[f->druha.odkud->cislo] = 0;
                            if (f->druha.odkud == u) {
                                break;
                            }
                            f = pulhrana_k_rodici[f->druha.odkud->cislo];
                        }


                        //spočítáme nová čísla
                        int pocet = 1;
                        for (int i = 0; i < G.V.size(); i++) {
                            if (nove_cislo[i] != 0) {
                                nove_cislo[i] = pocet;
                                pocet++;
                            }
                        }


                        //vybudujeme graf s kontrahovaným květem
                        
                        graf G2;

                        for (int i = 0; i < pocet; i++) {
                            G2.V.push_back(new vrchol);
                        }

                        //odkazy na odpovídající hrany v novém grafu
                        vector<vector<pulhrana*> odpovidajici;
                        for (int i = 0; i < G.V.size(); i++) {
                            for (int j = 0; j < G.V[i].hrany.size(); j++) {
                                odpovidajici[i].push_back(NULL);
                            }
                        }


                        for (int v = 0; v < G.V.size(); v++) {
                            //for (vector<pulhrana*>::iterator it = G.V[v]->hrany.begin(); it != G.V[v]->hrany.end(); ++it) {
                            for (int j = 0; j < G.V[v]->hrany.size(); j++) {
                                int w = G.V[v]->hrany[j].druha->cislo;

                                //pokud hrana má aspoň jeden konec mimo květ, okopírujeme ji
                                //(ale jen jednou, pokud v < w)
                                if ((nove_cislo[v] != 0 || nove_cislo[w] != 0) && (v < w)) {
                                    pulhrana *p1 = new pulhrana(G2.V[nove_cislo[v]]);
                                    pulhrana *p2 = new pulhrana(G2.V[nove_cislo[w]]);
                                    p1->druha = p2;
                                    p2->druha = p1;
                                    p1->parovaci = p2->parovaci = G.V[v]->hrany[j].parovaci;

                                    G2.V[nove_cislo[v]]->hrany.push_back(p1);
                                    G2.V[nove_cislo[w]]->hrany.push_back(p2);

                                    //odpovídající hranu uložíme jen pro jednu stranu, ale to nevadí
                                    odpovidajici[v][j] = p1;
                                }
                            }
                        }

                        //pokud algoritmus spuštěný na G2 nenajde větší párování, je vstupní párování maximální
                        if (!edmonds(G2,vel_parovani)) {
                            return false;
                        } else { //algoritmus našel na G2 větší párování
                            //je-li květ volný, vrátíme vstupní párování na květu a mimo něj to z G2
                            if (G2.V[0].volny()) {
                                for (int v = 0; v < G.V.size(); v++) {

                                    for (int i = 0; i < G.V[v]->hrany.size(); i++) {
                                        //každou hranu potkáme z jedné strany
                                        if (odpovidajici[v][i] != NULL) {
                                            G.V[v]->hrany[i].parovaci = G.V[v]->hrany[i]->druha.parovaci = odpovidajici[v][i].parovaci;
                                        }
                                    }
                                }
                                return true;
                            } else {
                                


                        }
                    }
                }
            }
            if (hladina[w]
        }
    }
    return false;
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
