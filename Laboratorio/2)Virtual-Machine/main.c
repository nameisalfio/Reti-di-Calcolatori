#include <stdio.h>
#include <stdlib.h>

/*
SISTEMA DI VIRTUALIZZAZIONE

    Un sistema di virtualizzazione consente di far coesistere più macchine all'interno di un sistema operativo,
    ognuna delle quali può anche avere diversi sistemi operativi. 
    Le macchine che creiamo saranno cloni della prima macchina virtuale e di conseguenza occuperanno molte meno 
    risorse rispetto a quelle di partenza. Non tocchiamo la prima macchina, che utilizzeremo per riclonare un eventuale 
    macchina che si danneggia durante la configurazione. Le utility di base verranno installate nella prima macchina
    e si lavora SOLO sulle altre per evitare di corrompere la macchina di partenza.

    Tramite V-Box è possibile configurare al più 8 schede di rete per ogni VM. Di default VBox permette di abilitarne
    solo 4 e quindi si necessita delle modifiche.
    Ad ogni VM viene assegnato dell'hardware standard

    Una VM è una combinazione di file:

    - un file XML che emula il comportamento della macchina
    - un file per accedere al disco fisso per memorizzare file

    NAT (acronimo di "Network Address Translation") è una tecnologia che consente di connettere una rete privata (una rete 
    domestica) a Internet, traducendo gli indirizzi IP della rete privata in un unico indirizzo IP pubblico.
    Sfrutteremo molto questa tecnologia nel rooting, avendo bisogno di una rete privata.
    
*/

int main()
{

}
