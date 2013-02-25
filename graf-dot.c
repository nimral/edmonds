#include <stdio.h>

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
                printf("[penwidth=2.0]");
            }
        }

        printf(";\n");
    }

    printf("}\n");
}
