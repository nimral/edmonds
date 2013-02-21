#include <cstdio>
#include <vector>

#define even(x) (((x) % 2) == 0)

using namespace std;

class vrchol
{
public:
    list<pulhrana> hrany;
    int cislo;
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

        p1 = pulhrana(G.V[v]);
        p2 = pulhrana(G.V[w]);
        p1.druha = &p2;
        p2.druha = &p1;
        p1.parovaci = false;
        p2.parovaci = false;


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

            
            
int edmonds(graf& G, int vel_parovani)
{
    deque fronta;
    int hladina[G.V.size()];
    int koren[G.V.size()];
    pulhrana *pulhrana_k_rodici[G.V.size()];

    for (int i = 0; i < G.V.size(); i++) {
        bool volny = true;
        for (int j = 0; j < G.V[i]->hrany.size(); j++) {
            if (G.V[i]->hrany[j].parovaci) {
                volny = false;
                break;
            }
        }
        if (volny) {
            fronta.push_back(i);
            hladina[i] = 0;
            koren[i] = i;
        } else {
            hladina[i] = -1;
            koren[i] = -1;
        }
        pulhrana_k_rodici[i] = NULL;
    }

    while (!fronta.empty()) {
        int v = fronta[0];
        fronta.pop_front();

        for (list<pulhrana>::iterator it = G.V[v]->hrany.begin(); it != G.V[v]->hrany.end(); ++it) {
            int w = it->druha.odkud->cislo;

            if (hladina[v] % 2 == 0) {
                if (koren[w] == -1) {
                    if (!it->parovaci) {
                        koren[w] = koren[v];
                        hladina[w] = hladina[v]+1;
                        pulhrana_k_rodici[w] = it->druha;
                        fronta.push_back(w);
                    }
                } else {
                    if (koren[w] != koren[v] && even(hladina[w])) {
                        //musí být nepárovací
                        //našli jsme augmenting path
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

                        //return ?
                    } else if (koren[w] == koren[v] && even(hladina[w])) {
                        //našli jsme květ

                        //najdeme začátek stonku
                        hladina[v] = -2;
                        hladina[w] = -2;

                        //budeme kopírovat graf a vypouštět vrcholy
                        int nove_cislo[G.V.size()];
                        for (int i = 0; i < G.V.size(); i++) {
                            nove_cislo[i] = -1;
                        }


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
                        
                        //označíme vrcholy za květnaté
                        //hladina[v] = -3;
                        //hladina[w] = -3;
                        nove_cislo[v] = nove_cislo[w] = 0;

                        pulhrana *f = pulhrana_k_rodici[v];
                        while (true) {
                            //hladina[f->druha.odkud->cislo] = -3;
                            nove_cislo[f->druha.odkud->cislo] = 0;
                            if (f->druha.odkud == u) {
                                break;
                            }
                            f = pulhrana_k_rodici[f->druha.odkud->cislo];
                        }

                        *f = pulhrana_k_rodici[w];
                        while (true) {
                            //hladina[f->druha.odkud->cislo] = -3;
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

                        for (int v = 0; v < G.V.size(); v++) {
                            for (list<pulhrana>::iterator it = G.V[v]->hrany.begin(); it != G.V[v]->hrany.end(); ++it) {
                                int w = it->druha.odkud->cislo;
                                if ((nove_cislo[v] != 0 || nove_cislo[w] != 0) && (v < w)) {
                                    G2.V[nove_cislo[v]]->hrany.push_back(
            pulhrana *p1 = new pulhrana(G2.V[nove_cislo[v]]);
            pulhrana *p2 = new pulhrana(G2.V[nove_cislo[w]]);
            p1->druha = p2;
            p2->druha = p1;
            p1->parovaci = false;
            p2->parovaci = false;


        G.V[v]->hrany.push_back(p1);
        G.V[w]->hrany.push_back(p2);

                        //vrchol *nove[G.V.size()];
                        //bool spojeny_s_kvetem[G.V.size()];
                        //for (int i = 0; i < G.V.size(); i++) {
                            //nove[i] = new vrchol;
                            //spojeny_s_kvetem[i] = false;
                        //}

                        //for (int v = 0; v < G.V.size(); v++) {
                            //if (hladina[v] != -3) {
                                //nove[v]->cislo = G2.V.size();
                                //G2.V.push_back(nove[v]);
                                //for (list<pulhrana>::iterator it = G.V[v]->hrany.begin(); it != G.V[v]->hrany.end(); ++it) {
                                    //if (hladina[it->druha.odkud->cislo] == -3) {
                                        //if (!spojeny_s_kvetem[v]) {
                                            //pulhrana p1 = pulhrana(nove[it->druha.odkud->cislo]);
                                            //pulhrana p2 = pulhrana(nove[v]);
                                            //p1.druha = &p2;
                                            //p2.druha = &p1;
                                            //nove[v]->hrany.push_back(p1);
                                            //nove[0]->hrany.push_back(p2);
                                            //spojeny_s_kvetem[v] = true;
                                        //}
                                    //} else {
                                            //pulhrana p1 = pulhrana(nove[it->druha.odkud->cislo]);
                                            //pulhrana p2 = pulhrana(nove[v]);
                                            //p1.druha = &p2;
                                            //p2.druha = &p1;
                                            //nove[v]->hrany.push_back(p1);
                                            //nove[it->druha.odkud->cislo]->hrany.push_back(p2);
                                        //}
                                    //}
                                //}
                            //}

                        














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
