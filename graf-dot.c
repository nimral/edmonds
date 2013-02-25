#include <stdio.h>

/*
 * formát vstupu:
 * na prvním řádku tři celá nezáporná čísla n, m, s_parovanim
 * ~ počet vrcholů, počet hran, je grafu nějaké párování?
 * následuje m řádků ve formátu v, w [parovaci (pokud je graf
 * s_parovanim)]
 *
 * formát výstupu:
 * dot language
 *
 */

int main()
{
    int n,m,s_parovanim;
    scanf("%d %d %d",&n,&m,&s_parovanim);
    
    printf("graph {\n");

    for (int i = 0; i < m; i++) {
        int v,w;
        scanf("%d %d",&v,&w);
        printf("%d -- %d",v,w);

        if (s_parovanim) {
            int parovaci;
            scanf("%d",&parovaci);
            if (parovaci) {
                printf("[color=red,penwidth=3.0]");
            }
        }

        printf(";\n");
    }

    printf("}\n");
}
