#include <cstdio>
#include <deque>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <sstream>

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
    void vygeneruj(int n, int h);
    void vypis_parovani();
    void vypis_graf(FILE *out);
};

//načte graf ze standardního vstupu
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

//vygeneruje náhodný graf s n vrcholy a h% pravděpodobností, že mezi dvěma vrcholy povede hrana
void graf::vygeneruj(int n, int h)
{
    for (int i = 0; i < n; i++) {
        V.push_back(vrchol());
        V.back().cislo = i;
    }

    for (int v = 0; v < n; v++) {
        for (int w = v+1; w < n; w++) {
            if (rand() % 100 < h) {
                pulhrana *p1 = new pulhrana;
                pulhrana *p2 = new pulhrana;
                p1->odkud_ = &V[v];
                p2->odkud_ = &V[w];
                p1->druha = p2;
                p2->druha = p1;
                p1->parovaci = false;
                p2->parovaci = false;

                V[v].hrany.push_back(p1);
                V[w].hrany.push_back(p2);
            }
        }
    }
}
 
//vypíše graf ve formátu stejném jako je vstupní
void graf::vypis_graf(FILE *out = stdout)
{
    int hran = 0;
    for (int v = 0; v < V.size(); v++) {
        hran += V[v].hrany.size();
    }
    hran /= 2;
    fprintf(out,"%d %d 1\n",V.size(),hran);

    for (int v = 0; v < V.size(); v++) {
        for (int h = 0; h < V[v].hrany.size(); h++) {
            int o = V[v].hrany[h]->odkud();
            int k = V[v].hrany[h]->kam();
            if (o < k) {
                fprintf(out,"%d %d %d\n",o,k,V[v].hrany[h]->parovaci ? 1 : 0);
            }
        }
    }
}

//vypíše jen párovací hrany
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
    int hladina[G.V.size()];
    int koren[G.V.size()];
    pulhrana *pulhrana_k_rodici[G.V.size()];
    vector<int> volne;

    //v nulté hladině jsou všechny volné vrcholy
    for (int i = 0; i < G.V.size(); i++) {
        if (G.V[i].volny()) {
            volne.push_back(i);
            hladina[i] = 0;
            koren[i] = i;
        } else {
            hladina[i] = -1;
            koren[i] = -1;
        }
        pulhrana_k_rodici[i] = NULL;
    }


    for (int vol_v = 0; vol_v < volne.size(); vol_v++) {

        //procházení do šířky
        deque<int> fronta;
        fronta.push_back(volne[vol_v]);

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

                        //pokud hrana vede mezi sudými hladinami různých stromů
                        if (koren[proch_w] != koren[proch_v] && even(hladina[proch_w]) && even(hladina[proch_v])) {
                            //našli jsme augmenting path

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

                        } else if (koren[proch_w] == koren[proch_v] && even(hladina[proch_w]) && even(hladina[proch_v])) {
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

                            bool uz_hrana[pocet]; //už z daného vrcholu vede v G2 hrana do květu?
                            pulhrana *pulhrana_do_kvetu[pocet]; //odkaz na hranu z vrcholu do květu pro aktualizování párování na této hraně v G2

                            for (int i = 0; i < pocet; i++) {
                                G2.V.push_back(vrchol());
                                G2.V.back().cislo = i;

                                uz_hrana[i] = false;
                                pulhrana_do_kvetu[i] = NULL;
                            }
                            uz_hrana[0] = true;

                            //odkazy na odpovídající hrany ve starém grafu
                            vector<vector<pulhrana*> > odpovidajici;
                            for (int i = 0; i < G2.V.size(); i++) {
                                odpovidajici.push_back(vector<pulhrana*>());
                                for (int j = 0; j < G2.V[i].hrany.size(); j++) {
                                    odpovidajici[i].push_back(NULL);
                                }
                            }


                            for (int v = 0; v < G.V.size(); v++) {

                                int do_kvetu = -1;
                                int z_kvetu = -1;
                                for (int j = 0; j < G.V[v].hrany.size(); j++) {
                                    pulhrana *jhrana = G.V[v].hrany[j];
                                    int w = jhrana->kam();

                                    //pokud má hrana jeden vrchol v květu, druhý mimo květ a nevede z něj ještě v G2 hrana do květu, okopírujeme ji
                                    //pokud jsou oba vrcholy mimo květ, okopírujeme hranu jen když na ni narazíme poprvé (v < w)
                                    if ((nove_cislo[v] != 0) && ((nove_cislo[w] == 0 && !uz_hrana[nove_cislo[v]]) || (nove_cislo[w] != 0 && (v < w)))) {
                                        pulhrana *p1 = new pulhrana;
                                        pulhrana *p2 = new pulhrana;
                                        p1->odkud_ = &G2.V[nove_cislo[v]];
                                        p2->odkud_ = &G2.V[nove_cislo[w]];
                                        p1->druha = p2;
                                        p2->druha = p1;
                                        p1->parovaci = p2->parovaci = G.V[v].hrany[j]->parovaci;


                                        G2.V[nove_cislo[v]].hrany.push_back(p1);
                                        G2.V[nove_cislo[w]].hrany.push_back(p2);


                                        odpovidajici[nove_cislo[v]].push_back(jhrana);
                                        odpovidajici[nove_cislo[w]].push_back(jhrana->druha);

                                        if (nove_cislo[w] == 0) {
                                            uz_hrana[nove_cislo[v]] = true;
                                            pulhrana_do_kvetu[nove_cislo[v]] = p1;
                                            do_kvetu = j;
                                            z_kvetu = G2.V[nove_cislo[w]].hrany.size() - 1;
                                        }
                                    }
                                    //pokud je alespoň jedna hrana z v do květu párovací, bude párovací i hrana z v do květu v G2
                                    if (jhrana->parovaci && pulhrana_do_kvetu[nove_cislo[v]] != NULL && nove_cislo[w] == 0) {
                                        pulhrana_do_kvetu[nove_cislo[v]]->parovaci = pulhrana_do_kvetu[nove_cislo[v]]->druha->parovaci = true;
                                        odpovidajici[nove_cislo[v]][do_kvetu] = jhrana;
                                        //odpovidajici[nove_cislo[v]][z_kvetu] = jhrana->druha;
                                    }
                                }
                            }


                            //pokud algoritmus spuštěný na G2 nenajde větší párování, je vstupní párování maximální
                            if (!edmonds(G2)) {
                                return false;
                            } else { //algoritmus našel na G2 větší párování
                                

                                //je-li květ volný, vrátíme vstupní párování na květu a mimo něj to z G2
                                if (G2.V[0].volny()) {
                                    for (int v = 0; v < G2.V.size(); v++) {

                                        for (int i = 0; i < G2.V[v].hrany.size(); i++) {
                                            if (odpovidajici[v][i] != NULL) {
                                                odpovidajici[v][i]->parovaci = odpovidajici[v][i]->druha->parovaci = G2.V[v].hrany[i]->parovaci;
                                            }

                                            //každou hranu potkáme z jedné strany
                                            //if (odpovidajici[v][i] != NULL) {
                                                //G.V[v].hrany[i]->parovaci = G.V[v].hrany[i]->druha->parovaci = odpovidajici[v][i]->parovaci;
                                            //}
                                        }
                                    }
                                } else { //květ není volný, musíme na něm upravit párování

                                    int hledany = -1; //vrchol v květu, do kterého vedou dvě spárované hrany
                                    bool sparovany[G.V.size()];
                                    //sparovanost na květu, párování je korektní
                                    for (int v = 0; v < G.V.size(); v++) {

                                        sparovany[v] = false;
                                        if (nove_cislo[v] == 0) {
                                            for (int i = 0; i < G.V[v].hrany.size(); i++) {
                                                pulhrana *ihrana = G.V[v].hrany[i];
                                                int w = ihrana->kam();

                                                if (nove_cislo[w] == 0 && ihrana->parovaci) {
                                                    sparovany[v] = sparovany[w] = true;
                                                    break;
                                                }
                                            }
                                        }
                                                    
                                    }
                                    //aktualizujeme párování mimo květ
                                    for (int v = 0; v < G2.V.size(); v++) {

                                        for (int i = 0; i < G2.V[v].hrany.size(); i++) {

                                            if (odpovidajici[v][i] != NULL) {
                                                
                                                pulhrana *odp_hrana = odpovidajici[v][i];
                                                int w = odp_hrana->kam();
                                                
                                                odp_hrana->parovaci = odp_hrana->druha->parovaci = G2.V[v].hrany[i]->parovaci;

                                                if (odp_hrana->parovaci) {
                                                    if (sparovany[odp_hrana->odkud()]) {
                                                        hledany = odp_hrana->odkud(); //jen do vrcholu v květu teď mohou vést dvě párovací hrany
                                                    } else {
                                                        sparovany[odp_hrana->odkud()] = true;
                                                    }
                                                    if (nove_cislo[w] == 0) {
                                                        if (sparovany[w]) {
                                                            hledany = w;
                                                        } else {
                                                            sparovany[w] = true;
                                                        }
                                                    }
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
                                    for (pulhrana *f = pulhrana_k_rodici[proch_v]; ; f = pulhrana_k_rodici[f->kam()]) {
                                        f->parovaci = f->druha->parovaci = nasleduje_parovaci;
                                        if (f->kam() != hledany) {
                                            nasleduje_parovaci = !nasleduje_parovaci;
                                        }
                                        if (f->kam() == zac_stonku) {
                                            break;
                                        }
                                    }

                                    //upravíme část květu, která odpovídá části cesty z proch_w do kořene
                                    if (hledany != proch_w) {
                                        nasleduje_parovaci = even(vzd);
                                    } else {
                                        nasleduje_parovaci = false;
                                    }
                                    for (pulhrana *f = pulhrana_k_rodici[proch_w]; ; f = pulhrana_k_rodici[f->kam()]) {
                                        f->parovaci = f->druha->parovaci = nasleduje_parovaci;
                                        if (f->kam() != hledany) {
                                            nasleduje_parovaci = !nasleduje_parovaci;
                                        }
                                        if (f->kam() == zac_stonku) {
                                            break;
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
    }
    //neexistuje hrana mezi sudými hladinami Edmondsova lesa, tedy neexistuje
    //volná střídavá cesta a párování je maximální
    return false;
}

int main()
{
    graf G;
    G.nacti_graf();

    while (edmonds(G))
        ;

    G.vypis_graf();
}
