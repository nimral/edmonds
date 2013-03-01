#include <cstdio>
#include <deque>
#include <vector>
#include <cstdlib>

#define even(x) (((x) % 2) == 0)

using namespace std;

class pulhrana;

class vrchol
{
public:
    vector<pulhrana*> hrany;
    int cislo;
    bool volny();
    ~vrchol();
};

class pulhrana
{
public:
    pulhrana *druha;
    vrchol *odkud_;
    int odkud();
    int kam();
    bool parovaci;
};


vrchol::~vrchol()
{
    for (int i = 0; i < hrany.size(); i++) {
        delete hrany[i];
    }
}

int pulhrana::odkud()
{
    return odkud_->cislo;
}

int pulhrana::kam()
{
    return druha->odkud();
}

class graf
{
public:
    vector<vrchol> V;
    void nacti_graf();
    void vypis_parovani();
    void vypis_graf();
    ~graf();
};


void graf::nacti_graf()
{
    int n,m,s_parovanim;
    scanf("%d %d %d",&n,&m,&s_parovanim);
    for (int i = 0; i < n; i++) {
        V.push_back(vrchol());
        V.back().cislo = i;
    }

    for (int i = 0; i < m; i++) {
        int v,w,p;
        scanf("%d %d",&v,&w);

        pulhrana *p1 = new pulhrana;
        pulhrana *p2 = new pulhrana;
        p1->odkud_ = &V[v];
        p2->odkud_ = &V[w];
        p1->druha = p2;
        p2->druha = p1;
        p1->parovaci = false;
        p2->parovaci = false;

        if (s_parovanim) {
            int p;
            scanf("%d",&p);
            p1->parovaci = p2->parovaci = p;
        }

        V[v].hrany.push_back(p1);
        V[w].hrany.push_back(p2);
        //printf("parovaci: %d\n",V[v].hrany.back()->parovaci);
    }
}

void graf::vypis_graf()
{
    int hran = 0;
    for (int v = 0; v < V.size(); v++) {
        hran += V[v].hrany.size();
    }
    hran /= 2;
    printf("%d %d 1\n",V.size(),hran);

    for (int v = 0; v < V.size(); v++) {
        for (int h = 0; h < V[v].hrany.size(); h++) {
            int o = V[v].hrany[h]->odkud();
            int k = V[v].hrany[h]->kam();
            if (o < k) {
                printf("%d %d %d\n",o,k,V[v].hrany[h]->parovaci ? 1 : 0);
            }
        }
    }
}

void graf::vypis_parovani()
{
    for (int v = 0; v < V.size(); v++) {
        for (int h = 0; h < V[v].hrany.size(); h++) {
            if (V[v].hrany[h]->parovaci) {
                int o = V[v].hrany[h]->odkud();
                int k = V[v].hrany[h]->kam();
                if (o < k) {
                    printf("%d %d\n",o,k);
                }
            }
        }
    }
}

graf::~graf()
{
    for (int i = 0; i < V.size(); i++) {
        for (int j = 0; j < V[i].hrany.size(); j++) {
            delete V[i].hrany[j];
        }
        //delete V[i];
    }
}


bool vrchol::volny()
{
    bool volny = true;
    for (vector<pulhrana*>::iterator it = hrany.begin(); it != hrany.end(); ++it) {
        if ((*it)->parovaci) {
            volny = false;
            break;
        }
    }
    return volny;
}
    
            
//podařilo se na grafu G zvětšit párování?
bool edmonds(graf& G)
{
    //konstruujeme Edmondsův les procházením do šířky
    deque<int> fronta;
    int hladina[G.V.size()];
    int koren[G.V.size()];
    pulhrana *pulhrana_k_rodici[G.V.size()];

    //v nulté hladině jsou všechny volné vrcholy
    for (int i = 0; i < G.V.size(); i++) {
        if (G.V[i].volny()) {
            fprintf(stderr,"volny: %d\n",i);
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
        int proch_v = fronta[0];
        fronta.pop_front();

        //for (vector<pulhrana*>::iterator it = G.V[v].hrany.begin(); it != G.V[v].hrany.end(); ++it) {
        for (int h = 0; h < G.V[proch_v].hrany.size(); h++) {
            pulhrana *proch_hrana = G.V[proch_v].hrany[h];

            //v Edmondsově lese se pravidelně střídají párovací a nepárovací hrany
            if (even(hladina[proch_v]) != proch_hrana->parovaci) {
                int proch_w = proch_hrana->kam();
                
                //nenavštívený vrchol, přidáme jej do stromu
                if (koren[proch_w] == -1) {

                    koren[proch_w] = koren[proch_v];
                    hladina[proch_w] = hladina[proch_v]+1;
                    pulhrana_k_rodici[proch_w] = proch_hrana->druha;
                    fronta.push_back(proch_w);

                } else { //navštívený vrchol

                    //pokud hrana vede mezi sudými/lichými hladinami různých stromů
                    if (koren[proch_w] != koren[proch_v] && even(hladina[proch_w]) == even(hladina[proch_v])) {
                        //našli jsme augmenting path
                        fprintf(stderr,"našli jsme augmenting path\n");

                        //zalternujeme ji
                        proch_hrana->parovaci = true;
                        proch_hrana->druha->parovaci = true;

                        for (pulhrana *f = pulhrana_k_rodici[proch_v]; f != NULL; f = pulhrana_k_rodici[f->kam()]) {
                            f->parovaci = !f->parovaci;
                            f->druha->parovaci = !f->druha->parovaci;
                        }

                        for (pulhrana *f = pulhrana_k_rodici[proch_w]; f != NULL; f = pulhrana_k_rodici[f->kam()]) {
                            f->parovaci = !f->parovaci;
                            f->druha->parovaci = !f->druha->parovaci;
                        }

                        //zvětšili jsme párování
                        return true;

                    } else if (koren[proch_w] == koren[proch_v]) { // && even(hladina[proch_w])) && even(hladina[proch_v])
                        //našli jsme květ

                        //najdeme začátek stonku:
                        //změníme hladiny všech vrcholů na cestě od v ke kořeni na -2
                        //pak budeme stoupat od w dokud nenarazíme na vrchol na hladině -2
                        hladina[proch_v] = -2;
                        hladina[proch_w] = -2;

                        for (pulhrana *f = pulhrana_k_rodici[proch_v]; f != NULL; f = pulhrana_k_rodici[f->kam()]) {
                            hladina[f->kam()] = -2;
                        }

                        int zac_stonku; //začátek stonku
                        for (pulhrana *f = pulhrana_k_rodici[proch_w]; f != NULL; f = pulhrana_k_rodici[f->kam()]) {
                            if (hladina[f->kam()] == -2) {
                                zac_stonku = f->kam();
                                break;
                            }
                        }
                        

                        //vytvoříme graf G2, ve kterém bude nalezený květ zkontrahovaný
                        
                        //budeme kopírovat graf a vypouštět vrcholy -- ty se nám sesypou
                        int nove_cislo[G.V.size()];
                        for (int i = 0; i < G.V.size(); i++) {
                            nove_cislo[i] = -1;
                        }


                        //květ bude v novém grafu na pozici 0
                        nove_cislo[proch_v] = nove_cislo[proch_w] = 0;

                        //označíme si vrcholy ve květu
                        pulhrana *f = pulhrana_k_rodici[proch_v];
                        while (true) {
                            nove_cislo[f->kam()] = 0;
                            if (f->kam() == zac_stonku) {
                                break;
                            }
                            f = pulhrana_k_rodici[f->kam()];
                        }

                        f = pulhrana_k_rodici[proch_w];
                        while (true) {
                            nove_cislo[f->kam()] = 0;
                            if (f->kam() == zac_stonku) {
                                break;
                            }
                            f = pulhrana_k_rodici[f->kam()];
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
                            G2.V.push_back(vrchol());
                            G2.V.back().cislo = i;
                        }

                        //odkazy na odpovídající hrany v novém grafu
                        vector<vector<pulhrana*> > odpovidajici;
                        for (int i = 0; i < G.V.size(); i++) {
                            for (int j = 0; j < G.V[i].hrany.size(); j++) {
                                odpovidajici[i].push_back(NULL);
                            }
                        }


                        for (int v = 0; v < G.V.size(); v++) {
                            //for (vector<pulhrana*>::iterator it = G.V[v].hrany.begin(); it != G.V[v].hrany.end(); ++it) {
                            for (int j = 0; j < G.V[v].hrany.size(); j++) {
                                pulhrana *jhrana = G.V[v].hrany[j];
                                int w = jhrana->kam();

                                //pokud hrana má aspoň jeden konec mimo květ, okopírujeme ji
                                //(ale jen jednou, pokud v < w)
                                if ((nove_cislo[v] != 0 || nove_cislo[w] != 0) && (v < w)) {
                                    pulhrana *p1 = new pulhrana;
                                    pulhrana *p2 = new pulhrana;
                                    p1->odkud_ = &G2.V[nove_cislo[v]];
                                    p2->odkud_ = &G2.V[nove_cislo[w]];
                                    p1->druha = p2;
                                    p2->druha = p1;
                                    p1->parovaci = p2->parovaci = G.V[v].hrany[j]->parovaci;

                                    G2.V[nove_cislo[v]].hrany.push_back(p1);
                                    G2.V[nove_cislo[w]].hrany.push_back(p2);

                                    //odpovídající hranu uložíme jen pro jednu stranu, ale to nevadí, vždycky se podíváme na obě dvě
                                    odpovidajici[v][j] = p1;
                                }
                            }
                        }

                        //pokud algoritmus spuštěný na G2 nenajde větší párování, je vstupní párování maximální
                        if (!edmonds(G2)) {
                            return false;
                        } else { //algoritmus našel na G2 větší párování

                            //je-li květ volný, vrátíme vstupní párování na květu a mimo něj to z G2
                            if (G2.V[0].volny()) {
                                for (int v = 0; v < G.V.size(); v++) {

                                    for (int i = 0; i < G.V[v].hrany.size(); i++) {
                                        //každou hranu potkáme z jedné strany
                                        if (odpovidajici[v][i] != NULL) {
                                            G.V[v].hrany[i]->parovaci = G.V[v].hrany[i]->druha->parovaci = odpovidajici[v][i]->parovaci;
                                        }
                                    }
                                }
                            } else { //květ není volný, musíme na něm upravit párování

                                int hledany = -1; //vrchol v květu, do kterého vedou dvě spárované hrany
                                vector<bool> sparovany(G.V.size(),false);
                                for (int v = 0; v < G.V.size(); v++) {

                                    for (int i = 0; i < G.V[v].hrany.size(); i++) {
                                        pulhrana *ihrana = G.V[v].hrany[v];
                                        int w = ihrana->kam();
                                        //každou hranu potkáme z jedné strany
                                        if (odpovidajici[v][i] != NULL) {
                                            ihrana->parovaci = ihrana->druha->parovaci = odpovidajici[v][i]->parovaci;
                                        }
                                        if (ihrana->parovaci) {
                                            if (sparovany[v]) {
                                                hledany = v; //jen do vrcholu v květu teď mohou vést dvě párovací hrany
                                            } else {
                                                sparovany[v] = true;
                                            }
                                            if (sparovany[w]) {
                                                hledany = w;
                                            } else {
                                                sparovany[w] = true;
                                            }
                                        }
                                    }

                                }

                                int vzd = 0; //počet hran na květu od proch_v do hledany
                                int i;
                                bool nasleduje_parovaci; //párovací a nepárovací hrany se musejí střídat
                                for (i = proch_v; i != hledany && i != zac_stonku; i = pulhrana_k_rodici[i]->kam()) {
                                    vzd++;
                                }
                                if (i != hledany) {
                                    vzd = 0;
                                    for (i = proch_w; i != hledany; i = pulhrana_k_rodici[i]->kam()) {
                                        vzd++;
                                    }
                                }

                                //upravíme část květu, která odpovídá části cesty z proch_v do kořene
                                if (hledany != proch_v) {
                                    nasleduje_parovaci = even(vzd);
                                } else {
                                    nasleduje_parovaci = false;
                                }
                                for (pulhrana *f = pulhrana_k_rodici[proch_v]; f->odkud() != zac_stonku; f = pulhrana_k_rodici[f->kam()]) {
                                    f->parovaci = f->druha->parovaci = nasleduje_parovaci;
                                    if (f->druha->odkud() != hledany) {
                                        nasleduje_parovaci = !nasleduje_parovaci;
                                    }
                                }

                                //upravíme část květu, která odpovídá části cesty z proch_w do kořene
                                if (hledany != proch_w) {
                                    nasleduje_parovaci = even(vzd);
                                } else {
                                    nasleduje_parovaci = false;
                                }
                                for (pulhrana *f = pulhrana_k_rodici[proch_w]; f->odkud() != zac_stonku; f = pulhrana_k_rodici[f->kam()]) {
                                    f->parovaci = f->druha->parovaci = nasleduje_parovaci;
                                    if (f->druha->odkud() != hledany) {
                                        nasleduje_parovaci = !nasleduje_parovaci;
                                    }
                                }

                                //zbývá upravit hranu mezi proch_v a proch_w
                                if (hledany == proch_v || hledany == proch_w) {
                                    proch_hrana->parovaci = proch_hrana->druha->parovaci = false;
                                } else {
                                    proch_hrana->parovaci = proch_hrana->druha->parovaci = !even(vzd);
                                }
                            }
                                    
                            return true;
                        }
                    }
                }
            }
        }
    }
    //neexistuje hrana mezi sudými hladinami Edmondsova lesa, tedy neexistuje
    //volná střídavá cesta a párování je maximální
    return false;
}

int main()
{
    graf G;
    G.nacti_graf();
    //G.vypis_graf();
    //printf("\n");

    while (edmonds(G))
        ;

    G.vypis_graf();
}
