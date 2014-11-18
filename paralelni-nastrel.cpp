#define DNO_ZASOBNIKU -2
#define VYCHOZI_VELIKOST_ZASOBNIKU 10000

#define ODDELOVAC 0

#define zadost_o_praci 10
#define konec_vypoctu 20 
#define nemam_praci 30
#define posilam_praci 40
#define posilam_token 50
#define reseni 60

#define MODULO 100      
#define MRTVY 99
#define DELKA 50

//#define DEBUG 1
//#define DEBUG1 1

#define BLACK 0
#define WHITE 1



MPI_Status status;
int my_rank, p, flag;
int citac_cyklu = 0;
double t1, t2;

Zasobnik zasobnik, pom;
Data * data;

int dalsiDarce, token, prijatyToken;
bool mamToken;

//------------------------------------------------------------------------------

int getRandomProcessorNumber() {
    int r;
    r = (rand() % 100) % p;
    return r;
}

bool mamePraci() {

#ifdef DEBUG
    cout << "P" << my_rank << ": ZKOUMAM ZASOBNIK, POZICE=" << zasobnik.get_pozice() << endl;
#endif
    return zasobnik.mamePraci();
}

void posliZadostOpraci(int dst) {
    MPI_Send(&my_rank, 1, MPI_INT, dst, zadost_o_praci, MPI_COMM_WORLD);
#ifdef DEBUG
    cout << "P" << my_rank << ": POSILAM ZADOST O PRACI PROCESORU " << dst << endl;
#endif
}

void posliNemamPraci(int dst) {
    MPI_Send(&my_rank, 1, MPI_INT, dst, nemam_praci, MPI_COMM_WORLD);
#ifdef DEBUG
    cout << "P" << my_rank << ": POSILAM, ZE NEMAM PRACI PROCESORU " << dst << endl;
#endif
}

void posliUkonceniPrace(int dst) {
    MPI_Send(&my_rank, 1, MPI_INT, dst, konec_vypoctu, MPI_COMM_WORLD);
#ifdef DEBUG
    cout << "P" << my_rank << ": POSILA UKONCENI PRACE PROCESORU P" << dst << endl;
#endif
}

void ukonciPraci() {
#ifdef DEBUG
    cout << "P" << my_rank << ": KONCI" << endl;
#endif
    if (my_rank == 0) {
        data->vypisMaticiReseni();
        /* time measuring - stop */
        t2 = MPI_Wtime();
        printf("Celkovy cas vypoctu: %f\n", t2 - t1);
    }
    MPI_Finalize();
    exit(0);
}

void odesliReseni() {

    int message[DELKA];

    for (int i = 0; i < data->m; i++) {
        message[i] = data->M_BEST[i];
    }

    if (my_rank != 0) {
        MPI_Send(message, DELKA, MPI_INT, 0, reseni, MPI_COMM_WORLD);
    }

#ifdef DEBUG
    cout << "P" << my_rank << ": POSILAM SVOJE NEJLEPSI RESENI PROCESORU P0" << endl;
    cout << "RESENI: ";
    vypisPole(data->M_BEST, data->m);
#endif
}

void prijmiReseniOdOstatnich() {

    int message[DELKA];
    int * M_TEMP;

    M_TEMP = (int*) malloc(data->m * sizeof (int));

    //ulozim si moje aktualni pole M
    for (int i = 0; i < data->m; i++) {
        M_TEMP[i] = data->M[i];
    }

    //prijmu zpravu
    MPI_Recv(message, DELKA, MPI_INT, MPI_ANY_SOURCE, reseni, MPI_COMM_WORLD, &status);
#ifdef DEBUG
    cout << "P" << my_rank << ": PRIJIMAM RESENI OD PROCESORU P" << status.MPI_SOURCE << endl;
#endif
    //vypisPole(message, data->m);

    //ulozim si prijate reseni do sveho pole M, ktere jsem si predtim ulozil do M_TEMP
    for (int i = 0; i < data->m; i++) {
        data->M[i] = message[i];
    }

    //mam kombinaci, zkontroluju
    //    cout << "P" << my_rank << ": ";
    //    data->uloz_zpracuj_kombinaci();
    //    vypisPole(data->M_BEST, data->m);

    //obnovim si svoje aktualni reseni z M_TEMP a pokracuju dal
    for (int i = 0; i < data->m; i++) {
        data->M[i] = M_TEMP[i];
    }

    free(M_TEMP);
}

void posliToken(int color) {
    if (mamToken) {
        MPI_Send(&color, 1, MPI_INT, ((my_rank + 1) % p), posilam_token, MPI_COMM_WORLD);
        mamToken = false;
        token = WHITE;
#ifdef DEBUG
        cout << "P" << my_rank << ": POSILAM TOKEN PROCESORU P" << ((my_rank + 1) % p) << " S BARVOU " << color << endl;
#endif
    }
}

void prijmiToken() {
    MPI_Recv(&prijatyToken, 1, MPI_INT, MPI_ANY_SOURCE, posilam_token, MPI_COMM_WORLD, &status);
    mamToken = true;
#ifdef DEBUG
    cout << "P" << my_rank << ": PRIJAL TOKEN S BARVOU " << prijatyToken << " A SAM JE " << token << endl;
#endif
    if (my_rank == 0) {
        // prvni procesor dostal token
        if (prijatyToken == WHITE) {
            // token je bily - > rozesle ukonceni vypoctu vsem procesorum a sam skonci
            for (int i = 1; i < p; i++) {
                posliUkonceniPrace(i);
            }
            for (int i = 1; i < p; i++) {
                prijmiReseniOdOstatnich();
            }
            ukonciPraci();
        } else {
            // pesek je cerny -> rozeslu novy bily token
            posliToken(WHITE);
        }
    } else {
        if (token == WHITE) {
            // jsem bily, s tokenem nedelam nic
        } else {
            // jsem cerny, obarvim token na cerno //obarvit token???
            prijatyToken = BLACK;
        }
    }
}

void posliPraci(int dst) {

    //message[0]=Mindex
    //message[1]=koren
    //message[2..2+Mindex]=M;

    int message[DELKA];

    //message[0] = data->Mindex;
    message[0] = 0;
    message[1] = zasobnik.pop_dno(); //odebrali a poslali jsme cislo ze dna zasobniku, jedem dal
    for (int i = 2; i < 2 + data->Mindex; i++) {
        message[i] = data->M[i - 2];
    }
    MPI_Send(message, DELKA, MPI_INT, dst, posilam_praci, MPI_COMM_WORLD);
#ifdef DEBUG
    cout << "P" << my_rank << ": POSILAM PRACI PROCESORU " << dst << endl;
    cout << "POSILANA DATA: ";
    vypisPole(message, 10);
#endif
}

void prijmiPraci() {

    //??? push 0 pred push message[1]?

    //message[0]=Mindex
    //message[1]=koren
    //message[2..2+Mindex]=M;

    int message[DELKA];

    MPI_Recv(message, DELKA, MPI_INT, MPI_ANY_SOURCE, posilam_praci, MPI_COMM_WORLD, &status);

    data->Mindex = message[0];
    zasobnik.push(ODDELOVAC);
    zasobnik.push(message[1]);
    for (int i = 0; i <= data->Mindex; i++) {
        data->M[i] = message[i + 2];
    }

#ifdef DEBUG1
    if (my_rank == 1) {
        cout << "P1: Mindex=" << data->Mindex << " ZASOBNIK PO PRIJATE PRACI: ";
        zasobnik.vypis();
        vypisPole(data->M, data->Mindex);
    }
#endif

#ifdef DEBUG
    cout << "P" << my_rank << ": PRIJAL PRACI OD PROCESORU P" << status.MPI_SOURCE << endl;
    cout << "ZASOBNIK PO PRIJATE PRACI: ";
    zasobnik.vypis();
    cout << "Mindex=" << data->Mindex << endl;
#endif
}

void zpracujZadostOpraci() {

    int tmp;

    MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, zadost_o_praci, MPI_COMM_WORLD, &status);
#ifdef DEBUG
    cout << "P" << my_rank << ": PRIJAL ZADOST O PRACI OD PROCESORU P" << status.MPI_SOURCE << endl;
#endif
    if (mamePraci()) {
        posliPraci(status.MPI_SOURCE);
        //poslal jsem praci procesoru, ktery ma nizsi cislo nez ja, obarvim se na cerno
        if (status.MPI_SOURCE < my_rank) {
            token = BLACK;
        }
    } else {
        posliNemamPraci(status.MPI_SOURCE);
    }
}

bool zpracujNemamPraci() {
    int tmp;

    MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, nemam_praci, MPI_COMM_WORLD, &status);
    dalsiDarce = (dalsiDarce + 1) % p;
    if (dalsiDarce == my_rank) {
        dalsiDarce = (dalsiDarce + 1) % p;
    }
}

void zpracujUkonceniVypoctu() {
    int tmp;

    MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, konec_vypoctu, MPI_COMM_WORLD, &status);
    odesliReseni();

    ukonciPraci();
}

void zpracujPrichoziZpravu() {
    switch (status.MPI_TAG) {
        case zadost_o_praci:
        {
            zpracujZadostOpraci();
            break;
        }
        case posilam_praci:
        {
            prijmiPraci();
            break;
        }
        case nemam_praci:
        {
            zpracujNemamPraci();
            break;
        }
        case posilam_token:
        {
            prijmiToken();
            break;
        }
        case konec_vypoctu:
        {
            zpracujUkonceniVypoctu();
            break;
        }
        default:
        {
            cout << "P" << my_rank << ": CHYBA PRIJMU ZPRAVY OD PROCESORU P" << status.MPI_SOURCE << endl;
            break;
        }
    }
}

int cekejNaZpravuOdNuly() {
    //cekam na zpravu od procesoru P0
    MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
#ifdef DEBUG
        cout << "P" << my_rank << ": P0 MI POSILA NEJAKOU ZPRAVU, TAG=" << status.MPI_TAG << endl;
#endif
        zpracujPrichoziZpravu();
        return 1;
    }
}

void projdiStrom() {
    int a;
    bool poslalJsemZadostOpraci = false;

    while (true) {

        while ((a = zasobnik.pop()) != DNO_ZASOBNIKU) {

            //            citac_cyklu++;

            //            if (citac_cyklu % 100 == 0) {
            //zkontroluj, jestli ti neprisla zprava, kdyztak ji zpracuj
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                if (status.MPI_TAG == zadost_o_praci) {
                    zpracujPrichoziZpravu();
                }
            }
            //            citac_cyklu = 0;
            //            }

            //delej pruchod stromem
            if (a != ODDELOVAC) {
                data->M[data->Mindex] = a;
                if (data->Mindex < (data->m - 1)) {
                    data->Mindex++;
                    zasobnik_napln(a);
                } else {
                    data->uloz_zpracuj_kombinaci();
                    //vypisPole(data->M, data->m);
#ifdef DEBUG
                    //cout << "P" << my_rank << ": NASEL JSEM SVOJE NEJ RESENI: ";
                    //vypisPole(data->M_BEST, data->m);
#endif
                }
            } else {
                data->Mindex--;
            }

            poslalJsemZadostOpraci = false;
        }

        if (p == 1) {
#ifdef DEBUG
            cout << "P" << my_rank << ": JSEM SAM, VYPISU SI RESENI" << endl;
#endif
            ukonciPraci();
        }

        //pokud nula dopocita, posle bily token sousedovi
        if (my_rank == 0) {
            posliToken(WHITE);
        } else {
            posliToken(prijatyToken);
        }

        if (!poslalJsemZadostOpraci) {
            posliZadostOpraci(dalsiDarce);
            poslalJsemZadostOpraci = true;
        } else {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                if (status.MPI_TAG == nemam_praci) {
                    poslalJsemZadostOpraci = false;
                }
                zpracujPrichoziZpravu();
            }

//            if (my_rank == 1) {
//                sleep(10);
//            }

        }
    }
#ifdef DEBUG
    cout << "P" << my_rank << ": MA PRAZDNY ZASOBNIK" << endl;
#endif
}

//------------------------------------------------------------------------------

int main(int argc, char ** argv) {

    /* start up MPI */
    MPI_Init(&argc, &argv);

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* time measuring - start */
    t1 = MPI_Wtime();

    //najdu si dalsiho darce, koho se budu ptat o praci
    dalsiDarce = (my_rank + 1) % p;

    //nastavim token na bilo
    token = WHITE;
    prijatyToken = -1;

    //nastavim si, kdo ma token a kdo ne
    if (my_rank == 0) {
        mamToken = true;
    } else {
        mamToken = false;
    }

    data = new Data(atoi(argv[1]), atoi(argv[2]));
    nacteniVstupnichDat(argc, argv);

    //--------------------------------------------------------------------------

    if (my_rank == 0) {
        zasobnik_napln(0);
    } else {
        posliZadostOpraci(0);
        while (true) {
            if (cekejNaZpravuOdNuly() > 0) {
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    //HLAVNI PRACE
#ifdef DEBUG
    cout << "P" << my_rank << ": ZACINAM PRUCHOD STROMEM" << endl;
    cout << "ZASOBNIK: ";
    zasobnik.vypis();
    cout << "ZASOBNIK POZICE PO NAPLNENI: " << zasobnik.get_pozice() << endl;
#endif

    projdiStrom();

    //--------------------------------------------------------------------------

    return 0;
}