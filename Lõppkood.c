//Kasutajaliidest ei saa kasutada kui arvuti, milles andmebaas asub on kinni.
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




//#pragma warning(disable : 4996)
#define MAX 50

MYSQL_RES* res;
MYSQL_ROW row;

int valimine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void kuvamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void lisamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void otsimine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void raamatu_kustutamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void autori_lisamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void zanri_lisamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void raamatule_autor(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void raamatule_zanr(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void kohaviida_lisamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void koopia_lisamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void autori_kustutamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void zanri_kustutamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void kohaviida_kustutamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);
void koopia_kustutamine(MYSQL*, MYSQL_RES*, MYSQL_ROW, int);

int main(int argc, char** argv) {
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }
    char kasutajanimi[100];
    char password[100];

    printf("Sisestage oma kasutajanimi:\n");//küsime kasutajalt kasutajanime
    if (fgets(kasutajanimi, sizeof kasutajanimi, stdin)) {
        char* newline = strchr(kasutajanimi, '\n');
        if (newline) {
            *newline = 0;

        }
    }
    printf("Sisestage oma salasõna:\n"); //küsime kasutajalt salasõna
    if (fgets(password, sizeof password, stdin)) {
        char* newline = strchr(password, '\n');
        if (newline) {
            *newline = 0;

        }
    }

    if (mysql_real_connect(con, "84.50.64.44", kasutajanimi, password, "raamatukogu", 3306, NULL, 0) == NULL) //püüame luua ühendust andmebaasiga
    {
        fprintf(stderr, "%s\n", mysql_error(con)); //kui ühenduse loomine ebaõnnestus prindi error
        printf("Sisselogimine ebaõnnestus!");
        exit(1);
    }
    int tegevus = 0;
    printf("------Raamatukogu andmebaas------\n"); //kui ühendus õnnestus prindi tekst
    valimine(con, res, row, tegevus); //liigu edasi alamprogrammi valimine


}

void lisamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char raamat_nimi[500];
    char a[600];
    char b[600];
    char *hx = "\0"; //muutuja ülakoma eemaldamiseks
	int k = 0;

    while (k == 0) { //loop mitme raamatu lisamiseks
    printf("Sisestage uue raamatu nimi:\n"); //küsib raamatu nime
    if (fgets(raamat_nimi, sizeof raamat_nimi, stdin)) { //loeb kasutaja sisestust
        char* newline = strchr(raamat_nimi, '\n'); //otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) { 
            *newline = 0; //kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(raamat_nimi);
    for (ix = 0; ix < len; ix++) //loop ülakoma kontrolliks
    {
        if (raamat_nimi[ix] == '\'')
        {
            raamat_nimi[ix] = *hx; //kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                raamat_nimi[jx] = raamat_nimi[jx + 1]; //järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }

    sprintf(b, "SELECT *FROM raamatud WHERE nimi='%s'", raamat_nimi); //lisab kokku MySQLi käsu kus valitakse tabelist kõik nimed, mis on samad nagu sisestatud vastus ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) { //saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1); //kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con); //salvestab saadud tulemused
    if(mysql_num_rows(res) != 0){ //kontrollib, kas on juba olemas vastav kirje
        printf("Raamat on juba andmebaasis olemas!\n"); //väljastab kasutajale teate
        mysql_free_result(res); //vabastab tulemused
        lisamine(con, res, row, tegevus); //alustab uuesti lisamise funktsiooniga
    }
    mysql_free_result(res);
    //kui andmebaasis pole juba soovitud kirjet olemas siis jätkab siit
    sprintf(a, "INSERT INTO raamatud (nimi) VALUES ('%s')", raamat_nimi); //lisab kokku MySQLi käsu millega sisestatakse tabelisse raamatu nimi ja kasutaja sisestatud vastuse

    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    sprintf(b, "SELECT *FROM raamatud WHERE nimi='%s'", raamat_nimi); //lisab kokku MySQLi käsu millega valitakse sisestatud nimi ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con); //salvestab saadud tulemused
    printf("Sisestatud raamatu id ja nimi: \n"); 
    while ((row = mysql_fetch_row(res)) != NULL) { //loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]); //väljastab tulemused
    }
    mysql_free_result(res); //vabastab tulemused

    int id = mysql_insert_id(con); //viimase INSERT käsu id 

    printf("Kas soovite lisada žanri?\n"); //küsib kasutajalt, kas ta soovib ka žanri lisada
    printf("-> 1 Jah\n");
    printf("-> 2 Ei\n");

    char t[10];
    int value;
    do {
        if (fgets(t, sizeof t, stdin)) { //loeb kasutaja sisestust
            char* newline = strchr(t, '\n'); //otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0; //kui reavahetus oli, siis kustutab selle
            }
        }
        value = atoi(t); //muudab vastuse numbriks
        if (value > 0 && value < 3) { //loop valimiseks
            int u = 0;
            switch (value) {
            case 1:
                while (u == 0) { //loop mitme žanri lisamiseks
                    printf("Andmebaasis olemas olevad žanrid:\n"); //näitab juba olemasolevaid žanreid
                    if (mysql_query(con, "SELECT * FROM sanrid")) {//saadab käsu žanrite valimiseks serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(2);//kui tekib viga, lahkub programmist 
                    }
                    res = mysql_use_result(con);//salvestab saadud tulemused

                    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
                    }
                    mysql_free_result(res);//vabastab tulemused
                    printf("Kas soovite lisada uue žanri või olemasoleva? \n"); //küsib kasutajalt, kas ta sooviks lisada uue või juba andmebaasis olemasoleva žanri
                    printf("-> 1 Olemasoleva\n");
                    printf("-> 2 Uue\n");
                    char d[10];
                    int valik;
                    char e[10];
                    int sanr_id;
                    char f[200];
                    char g[60];
                    char h[200];
                    char i[200];
                    do {
                        if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
                            char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                            if (newline) {
                                *newline = 0;//kui reavahetus oli, siis kustutab selle
                            }
                        }
                        valik = atoi(d);//muudab vastuse numbriks
                        if (valik > 0 && valik < 3) {
                            switch (valik) {
                            case 1: //kasutaja valis olemasoleva žanri lisamise
                                printf("Sisestage žanri id:\n"); //küsib kasutaja sisestust
                                if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                                    char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                                    if (newline) {
                                        *newline = 0;//kui reavahetus oli, siis kustutab selle

                                    }
                                }
                                sanr_id = atoi(e);//muudab vastuse numbriks
                                sprintf(f, "INSERT INTO raamatud_has_sanrid(raamatud_id, sanrid_id) VALUES ('%d', '%d')", id, sanr_id);//lisab kokku MySQLi käsu tabelisse lisamisesks ja kasutaja sisestatud vastused
                                if (mysql_query(con, f)) {//saadab käsu serverile
                                    fprintf(stderr, "%s\n", mysql_error(con));
                                    exit(1);//kui tekib viga, lahkub programmist 
                                }
                                break;//lahkub switchist
                            case 2: //kasutaja valis uue žanri lisamise
                                printf("Sisestage žanri nimi:\n");//küsib kasutaja sisestust
                                if (fgets(g, sizeof g, stdin)) {//loeb kasutaja sisestust
                                    char* newline = strchr(g, '\n'); //otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                                    if (newline) {
                                        *newline = 0;//kui reavahetus oli, siis kustutab selle

                                    }
                                    int ix, jx, len;
                                    len = strlen(g);
                                    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                                    {
                                        if (g[ix] == '\'')
                                        {
                                            g[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                                            for (jx = ix; jx < len; jx++)
                                            {
                                                g[jx] = g[jx + 1];//järgmised karakterid liigutab ettepoole
                                            }
                                            len--;//vähendab vastuse pikkust
                                            ix--;//vähendab kontrollmuutujat
                                        }
                                    }
                                    sprintf(h, "INSERT INTO sanrid( sanrinimi) VALUES ('%s')", g);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
                                    if (mysql_query(con, h)) {//saadab käsu serverile
                                        fprintf(stderr, "%s\n", mysql_error(con));
                                        exit(1);//kui tekib viga, lahkub programmist 
                                    }
                                    int id2 = mysql_insert_id(con);//viimase INSERT käsu id
                                    sprintf(i, "INSERT INTO raamatud_has_sanrid (raamatud_id, sanrid_id) VALUES ('%d', '%d')", id, id2);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                                    if (mysql_query(con, i)) {//saadab käsu serverile
                                        fprintf(stderr, "%s\n", mysql_error(con));
                                        exit(1);//kui tekib viga, lahkub programmist 
                                    }
                                }
                                break;//lahkub switchist
                            }

                        }

                    } while (valik < 0 && valik > 2);
                    char l[10];
                    printf("Kas soovite lisada veel ühe žanri?\n");//küsib kasutaja sisestust
                    printf("-> 0 Jah\n");
                    printf("-> 1 Ei\n");
                    if (fgets(l, sizeof l, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(l, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    u = atoi(l);//muudab vastuse numbriks
                }
                break;//lahkub switchist
            case 2://kasutaja ei soovinud lisada žanrit
                valimine(con, res, row, tegevus); //läheb tagasi valimise juurde
                break;//lahkub switchist
            }
        }



    } while (value < 1 && 2 < value);


    printf("Kas soovite lisada ka autori?\n"); //pakub võimaluse lisada veel ühe autori
    printf("-> 1 Jah\n");
    printf("-> 2 Ei\n");

    char c[10];
    int val;
    do {
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        val = atoi(c);//muudab vastuse numbriks
        if (val > 0 && val < 3) {
            int k = 0;
            switch (val) {
            case 1: //kasutaja valis autori lisamise
                while (k == 0) {
                    printf("Andmebaasis olemasolevad autorid:\n");//näitab andmebaasis olevaid autoreid
                    if (mysql_query(con, "SELECT * FROM autorid")) {//saadab käsu autorite valimiseks serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(2);//kui tekib viga, lahkub programmist 
                    }
                    res = mysql_use_result(con);//salvestab saadud tulemused

                    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
                    }
                    mysql_free_result(res);//vabastab tulemused
                    printf("Kas soovite lisada uue autori või olemasoleva? \n");
                    printf("-> 1 Olemasoleva\n");
                    printf("-> 2 Uue\n");
                    char d[10];
                    int valik;
                    char e[10];
                    int autor_id;
                    char f[200];
                    char g[60];
                    char h[100];
                    char i[200];
                    int id = mysql_insert_id(con);//viimase INSERT käsu id
                    do {
                        if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
                            char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                            if (newline) {
                                *newline = 0;//kui reavahetus oli, siis kustutab selle
                            }
                        }
                        valik = atoi(d);//muudab vastuse numbriks
                        if (valik > 0 && valik < 3) {
                            switch (valik) {
                            case 1: //kasutaja valis olemasoleva autori lisamise
                                printf("Sisestage autori id:\n");
                                if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                                    char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on 
                                    if (newline) {
                                        *newline = 0;//kui reavahetus oli, siis kustutab selle

                                    }
                                }
                                autor_id = atoi(e);//muudab vastuse numbriks
                                sprintf(f, "INSERT INTO raamatud_has_autorid (raamatud_id, autorid_id) VALUES ('%d', '%d')", id, autor_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                                if (mysql_query(con, f)) {//saadab käsu serverile
                                    fprintf(stderr, "%s\n", mysql_error(con));
                                    exit(1);//kui tekib viga, lahkub programmist 
                                }
                                break;//lahkub switchist
                            case 2: //kasutaja valis uue autori lisamise
                                printf("Sisestage autori nimi:\n");
                                if (fgets(g, sizeof g, stdin)) {//loeb kasutaja sisestust
                                    char* newline = strchr(g, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                                    if (newline) {
                                        *newline = 0;//kui reavahetus oli, siis kustutab selle

                                    }
                                    int ix, jx, len;
                                    len = strlen(g);
                                    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                                    {
                                        if (g[ix] == '\'')
                                        {
                                            g[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                                            for (jx = ix; jx < len; jx++)
                                            {
                                                g[jx] = g[jx + 1];//järgmised karakterid liigutab ettepoole
                                            }
                                            len--;//vähendab vastuse pikkust
                                            ix--;//vähendab kontrollmuutujat
                                        }
                                    }
                                    sprintf(h, "INSERT INTO autorid( nimi) VALUES ('%s')", g);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
                                    if (mysql_query(con, h)) {//saadab käsu serverile
                                        fprintf(stderr, "%s\n", mysql_error(con));
                                        exit(1);//kui tekib viga, lahkub programmist 
                                    }
                                    int id2 = mysql_insert_id(con);
                                    sprintf(i, "INSERT INTO raamatud_has_autorid (raamatud_id, autorid_id) VALUES ('%d', '%d')", id, id2);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                                    if (mysql_query(con, i)) {//saadab käsu serverile
                                        fprintf(stderr, "%s\n", mysql_error(con));
                                        exit(1);//kui tekib viga, lahkub programmist 
                                    }
                                }
                                break;//lahkub switchist
                            }

                        }

                    } while (valik < 0 && valik > 2);
                    char y[10];
                    printf("Kas soovite lisada veel ühe autori?\n"); //pakub kasutajale võimalust lisada veel ühe autori
                    printf("-> 0 Jah\n");
                    printf("-> 1 Ei\n");
                    if (fgets(y, sizeof y, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(y, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    k = atoi(y); //muudab vastuse numbriks
                }
                break;//lahkub switchist
            case 2: //kasutaja ei soovi lisada autorit
                valimine(con, res, row, tegevus); //tagasi valimise menüüsse
                break;//lahkub switchist
            }
        }

    } while (val < 1 && 2 < val);

    printf("Kas soovite lisada ka koopiaid?\n");//pakub võimaluse koopiate lisamiseks
    printf("-> 1 Jah\n");
    printf("-> 2 Ei\n");
    char x[10];
    int vali;
    do {
        if (fgets(x, sizeof x, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(x, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        vali = atoi(x);//muudab vastuse numbriks
        switch (vali) {
        case 1:
            int n = 0;
            while (n == 0) {
                char aasta[6];
                char triipkood[50];
                char keel[45];
                int kohaviit_id;
                char a[300];
                char d[6];

                printf("Sisestage koopia väljandmise aasta: \n");
                if (fgets(aasta, sizeof aasta, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(aasta, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                int ix, jx, len;
                len = strlen(aasta);//kontrolli jaoks vastuse pikkus
                for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                {
                    if (aasta[ix] == '\'')
                    {
                        aasta[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                        for (jx = ix; jx < len; jx++)
                        {
                            aasta[jx] = aasta[jx + 1];//järgmised karakterid liigutab ettepoole
                        }
                        len--;//vähendab vastuse pikkust
                        ix--;//vähendab kontrollmuutujat
                    }
                }
                printf("Sisestage koopia triipkood: \n");
                if (fgets(triipkood, sizeof triipkood, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(triipkood, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                len = strlen(triipkood);//kontrolli jaoks vastuse pikkus
                for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                {
                    if (triipkood[ix] == '\'')
                    {
                        triipkood[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                        for (jx = ix; jx < len; jx++)
                        {
                            triipkood[jx] = triipkood[jx + 1];//järgmised karakterid liigutab ettepoole
                        }
                        len--;//vähendab vastuse pikkust
                        ix--;//vähendab kontrollmuutujat
                    }
                }
                printf("Sisestage koopia keel: \n");
                if (fgets(keel, sizeof keel, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(keel, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                len = strlen(keel);//kontrolli jaoks vastuse pikkus
                for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                {
                    if (keel[ix] == '\'')
                    {
                        keel[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                        for (jx = ix; jx < len; jx++)
                        {
                            keel[jx] = keel[jx + 1];//järgmised karakterid liigutab ettepoole
                        }
                        len--;//vähendab vastuse pikkust
                        ix--;//vähendab kontrollmuutujat
                    }
                }

                if (mysql_query(con, "SELECT *FROM kohaviit")) {//saadab käsu kohaviitade valimiseks serverile
                    fprintf(stderr, "%s\n", mysql_error(con));
                    exit(1);//kui tekib viga, lahkub programmist 
                }
                res = mysql_use_result(con);//salvestab saadud tulemused
                printf("Raamatukogus olevate kohaviitade id-ed ja nimed: \n");
                while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                    printf("%s %s\n", row[0], row[1]);//väljastab tulemused
                }
                printf("Sisestage kohaviida id: \n");
                if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                printf("\nMitu selliste andmetega koopiat soovite lisada?\n");
                char t[10];
                if (fgets(t, sizeof t, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(t, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                int valik;
                valik = atoi(t);//muudab vastuse numbriks
                kohaviit_id = atoi(d);//muudab vastuse numbriks
                int i;
                for (i = 0; i < valik; i++) {
                    sprintf(a, "INSERT INTO koopia (aasta, triipkood, keel, raamatud_id, kohaviit_id) VALUES ('%s', '%s', '%s', '%d', '%d')", aasta, triipkood, keel, id, kohaviit_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, a)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                }
                char m[100];

                printf("Kas soovid lisada veel ühe koopia?\n"); //pakub võimaluse lisada veel ühe koopia
                printf("-> 0 Jah\n");
                printf("-> 1 Ei\n");
                if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
                    char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                    if (newline) {
                        *newline = 0;//kui reavahetus oli, siis kustutab selle
                    }
                }
                n = atoi(m);//muudab vastuse numbriks

            }
            break;//lahkub switchist
        case 2: //kasutaja ei soovi lisada koopiat
            valimine(con, res, row, tegevus); //tagasi valimise menüüsse
            break;//lahkub switchist
        }
    } while (vali < 0 && vali > 2);
	char m[100];
    printf("Kas soovite lisada veel ühe raamatu?\n");//pakub võimalust lisada veel raamatuid
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus);//läheb tagasi valimise menüüsse
}

void autori_lisamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char autor_nimi[60];
    char a[200];
    char b[200];
    char c[20];
    int k = 0;
    char *hx = "\0";
    while (k == 0) {//loop mitme autori lisamiseks

        printf("Sisestage autori nimi: \n");
        if (fgets(autor_nimi, sizeof autor_nimi, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(autor_nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        int ix, jx, len;
        len = strlen(autor_nimi);//kontrolli jaoks vastuse pikkus
        for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
        {
            if (autor_nimi[ix] == '\'')
            {
                autor_nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                for (jx = ix; jx < len; jx++)
                {
                    autor_nimi[jx] = autor_nimi[jx + 1];//järgmised karakterid liigutab ettepoole
                }
                len--;//vähendab vastuse pikkust
                ix--;//vähendab kontrollmuutujat
            }
        }
        char d[100];
        sprintf(d, "SELECT *FROM autorid WHERE nimi='%s'", autor_nimi);//lisab kokku MySQLi käsu sama nimega autori valimiseks ja kasutaja sisestatud vastuse
        if (mysql_query(con, d)) {//saadab käsu serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
       res = mysql_use_result(con);//salvestab saadud tulemused
       if(mysql_num_rows(res) != 0){ //kui andmebaasis on juba selline sissekanne olemas
           printf("Autor on juba andmebaasis olemas!\n");
            mysql_free_result(res);//vabastab tulemused
           autori_lisamine(con, res, row, tegevus);//alustab uuesti autori lisamist
        }
        mysql_free_result(res);
        sprintf(a, "INSERT INTO autorid( nimi) VALUES ('%s')", autor_nimi);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
        if (mysql_query(con, a)) {//saadab käsu serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        sprintf(b, "SELECT *FROM autorid WHERE nimi='%s'", autor_nimi);//lisab kokku MySQLi käsu sama nimega autori valimiseks ja kasutaja sisestatud vastuse
        if (mysql_query(con, b)) {//saadab käsu serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused
        printf("Sisestatud autori id ja nimi: \n");
        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }
        mysql_free_result(res);//vabastab tulemused
        
        printf("Kas soovite lisada veel ühe autori?\n");//pakub võimaluse lisada veel ühe autori
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(c);//muudab vastuse numbriks

    }


    valimine(con, res, row, tegevus);//tagasi valimise menüüsse

}

void zanri_lisamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char zanr_nimi[MAX];
    char a[100];
    char b[100];
    char c[100];
    int k = 0;
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    while (k == 0) {//loop mitme žanri lisamiseks
		printf("Andmebaasis olevad žanrid:\n");//näitab andmebaasis olemasolevaid žanreid
       if (mysql_query(con, "SELECT *FROM sanrid")) {//saadab käsu kõikide žanrite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
       }
       res = mysql_use_result(con);//salvestab saadud tulemused
       while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }


        printf("Sisestage zanri nimi: \n");
        if (fgets(zanr_nimi, sizeof zanr_nimi, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(zanr_nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
            int ix, jx, len;
            len = strlen(zanr_nimi);//kontrolli jaoks vastuse pikkus
            for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
            {
                if (zanr_nimi[ix] == '\'')
                {
                    zanr_nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                    for (jx = ix; jx < len; jx++)
                    {
                        zanr_nimi[jx] = zanr_nimi[jx + 1];//järgmised karakterid liigutab ettepoole
                    }
                    len--;//vähendab vastuse pikkust
                    ix--;//vähendab kontrollmuutujat
                }
            }

            sprintf(a, "INSERT INTO sanrid(sanrinimi) VALUES ('%s')", zanr_nimi);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
            if (mysql_query(con, a)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }

            sprintf(b, "SELECT *FROM sanrid WHERE sanrinimi='%s'", zanr_nimi);//lisab kokku MySQLi käsu sama nimega žanri valimiseks ja kasutaja sisestatud vastuse
            if (mysql_query(con, b)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }
            res = mysql_use_result(con);//salvestab saadud tulemused
            printf("Sisestatud žanri id ja nimi: \n");
            while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                printf("%s %s\n", row[0], row[1]);//väljastab tulemused
            }
        }
        mysql_free_result(res);//vabastab tulemused
        printf("Kas soovid lisada veel ühe žanri?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(c);//muudab vastuse numbriks

    }
    valimine(con, res, row, tegevus);//läheb tagasi valimise menüüsse

}

void raamatule_autor(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int autor_id;
    int raamat_id;
    char a[100];
    char b[10];
    char c[10];
    int k = 0;
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    while (k == 0) {//loop mitme autori lisamiseks
		printf("Raamatud ja nende id-d:\n");
       if (mysql_query(con, "SELECT *FROM raamatud")) {//saadab käsu kõikde raamatute valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
       }
       res = mysql_use_result(con);//salvestab saadud tulemused
       while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }

        printf("Andmebaasis olemas olevad autorid:\n");
        if (mysql_query(con, "SELECT * FROM autorid")) {//saadab käsu kõikide autorite valimiseks serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(2);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused

        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
        }
        mysql_free_result(res);//vabastab tulemused

        printf("Kas soovite lisada uue autori või olemasoleva? \n");
        printf("-> 1 Olemasoleva\n");
        printf("-> 2 Uue\n");
        int tege;
        char d[10];
        char e[100];
        char f[200];
        do {
            if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            tege = atoi(d);//muudab vastuse numbriks
            if (tege > 0 && tege < 3) {
                switch (tege) {
                    char* tegevusstr;
                case 1:
                    tegevusstr = "olemasoleva autori lisamine";
                    printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                    printf("Sisestage autori id: \n");
                    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    autor_id = atoi(b);//muudab vastuse numbriks
                    printf("Sisestage raamatu id:\n");
                    if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    raamat_id = atoi(c);//muudab vastuse numbriks
                    sprintf(a, "INSERT INTO raamatud_has_autorid (raamatud_id, autorid_id) VALUES ('%d', '%d')", raamat_id, autor_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, a)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    break;//lahkub switchist
                case 2:
                    tegevusstr = "uue autori lisamine";
                    printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                    printf("Sisestage autori id: \n");
                    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    autor_id = atoi(b);//muudab vastuse numbriks
                    printf("Sisestage autori nimi: \n");
                    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    int ix, jx, len;
                    len = strlen(e);//kontrolli jaoks vastuse pikkus
                    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                    {
                        if (e[ix] == '\'')
                        {
                            e[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                            for (jx = ix; jx < len; jx++)
                            {
                                e[jx] = e[jx + 1];//järgmised karakterid liigutab ettepoole
                            }
                            len--;//vähendab vastuse pikkust
                            ix--;//vähendab kontrollmuutujat
                        }
                    }
                    sprintf(f, "INSERT INTO autorid (id, nimi) VALUES ('%d', '%s')", autor_id, e);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, f)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    printf("Sisestage raamatu id:\n");
                    if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    raamat_id = atoi(c);//muudab vastuse numbriks
                    sprintf(a, "INSERT INTO raamatud_has_autorid (raamatud_id, autorid_id) VALUES ('%d', '%d')", raamat_id, autor_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, a)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    break;//lahkub switchist
                }

            }

        } while (tege < 1 && tege > 2);
        char m[100];

        printf("Kas soovite lisada veel ühele raamatule autori?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");

        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }



    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void raamatule_zanr(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int raamat_id;
    char a[100];
    char c[10];
    int k = 0;
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    while (k == 0) {//loop mitme žanri lisamiseks
		printf("Raamatud ja nende id-d:\n");
       if (mysql_query(con, "SELECT *FROM raamatud")) {//saadab käsu kõikide raamatute valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
       }
       res = mysql_use_result(con);//salvestab saadud tulemused
       while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }

        printf("Žanrid ja nende id-d:\n");
        if (mysql_query(con, "SELECT * FROM sanrid")) {//saadab käsu kõikide žanrite valimiseks serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(2);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused

        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
        }
        mysql_free_result(res);//vabastab tulemused

        printf("Kas soovite lisada uue või olemasoleva žanri?\n");
        printf("-> Olemasoleva\n");
        printf("-> Uue\n");
        int t;
        char d[10];
        char e[10];
        char f[200];
        char g[50];
        int zanr_id;

        do {
            if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            t = atoi(d);//muudab vastuse numbriks
            if (t > 0 && t < 3) {
                switch (t) {
                    char* tegevusstr;
                case 1:
                    tegevusstr = "olemasoleva žanri lisamine";
                    printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                    printf("Sisestage žanri id: \n");
                    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    zanr_id = atoi(e);//muudab vastuse numbriks
                    printf("Sisestage raamatu id:\n");
                    if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    raamat_id = atoi(c);//muudab vastuse numbriks
                    sprintf(a, "INSERT INTO raamatud_has_sanrid (raamatud_id, sanrid_id) VALUES ('%d', '%d')", raamat_id, zanr_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, a)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    break;//lahkub switchist
                case 2:
                    tegevusstr = "uue žanri lisamine";
                    printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                    printf("Sisestage žanri nimi: \n");
                    if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }
                    int ix, jx, len;
                    len = strlen(c);//kontrolli jaoks vastuse pikkus
                    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
                    {
                        if (c[ix] == '\'')
                        {
                            c[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                            for (jx = ix; jx < len; jx++)
                            {
                                c[jx] = c[jx + 1];//järgmised karakterid liigutab ettepoole
                            }
                            len--;//vähendab vastuse pikkust
                            ix--;//vähendab kontrollmuutujat
                        }
                    }
                    sprintf(f, "INSERT INTO sanrid (sanrinimi) VALUES ('%s')", g);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
                    if (mysql_query(con, f)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    int sanr_id = mysql_insert_id(con);//viimase INSERT käsu id
                    printf("Sisestage raamatu id:\n");
                    if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle

                        }
                    }

                    raamat_id = atoi(c);//muudab vastuse numbriks
                    sprintf(a, "INSERT INTO raamatud_has_sanrid (raamatud_id, sanrid_id) VALUES ('%d', '%d')", raamat_id, sanr_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
                    if (mysql_query(con, a)) {//saadab käsu serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(1);//kui tekib viga, lahkub programmist 
                    }
                    break;//lahkub switchist
                }

            }   
        }while (t < 1 && t > 2);

        
        char m[100];

        printf("Kas soovid lisada veel ühele raamatule žanri?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void kohaviida_lisamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {

    char kohaviit[46];
    char a[100];
    char b[100];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    int k = 0;
    while (k == 0) {//loop mitme kohaviida lisamiseks
        if (mysql_query(con, "SELECT *FROM kohaviit")) {//saadab käsu kõikide kohaviitade valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
       }
    res = mysql_use_result(con);//salvestab saadud tulemused
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab tulemused
    }

        printf("Sisestage uue kohaviida nimi: \n");
        if (fgets(kohaviit, sizeof kohaviit, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(kohaviit, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle

            }
        }
        int ix, jx, len;
        len = strlen(kohaviit);//kontrolli jaoks vastuse pikkus
        for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
        {
            if (kohaviit[ix] == '\'')
            {
                kohaviit[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                for (jx = ix; jx < len; jx++)
                {
                    kohaviit[jx] = kohaviit[jx + 1];//järgmised karakterid liigutab ettepoole
                }
                len--;//vähendab vastuse pikkust
                ix--;//vähendab kontrollmuutujat
            }
        }
        sprintf(a, "INSERT INTO kohaviit (kohaviit) VALUES ('%s')", kohaviit);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastuse
        if (mysql_query(con, a)) {//saadab käsu serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        sprintf(b, "SELECT *FROM kohaviit WHERE kohaviit='%s'", kohaviit);//lisab kokku MySQLi käsu sisestatud kohaviida valimiseks ja kasutaja sisestatud vastuse
        if (mysql_query(con, b)) {//saadab käsu serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused
        printf("Sisestatud kohaviida id ja nimi: \n");
        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }
        mysql_free_result(res);//vabastab tulemused
        char m[100];

        printf("Kas soovid lisada veel ühe kohaviida?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void koopia_lisamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char aasta[6];
    char triipkood[50];
    char keel[45];
    int raamatud_id;
    int kohaviit_id;
    char a[300];
    char c[6];
    char d[6];
    int n = 0;
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    while (n == 0) {//loop mitme koopia lisamiseks

        if (mysql_query(con, "SELECT *FROM raamatud")) {//saadab käsu kõikide raamatute valimiseks serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused
        printf("Raamatukogus olevate raamatute id-ed ja nimed: \n");
        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }
        mysql_free_result(res);//vabastab tulemused
        printf("Sisestage raamatu id, mille koopia tahate lisada: \n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }


        printf("Sisestage koopia väljandmise aasta: \n");
        if (fgets(aasta, sizeof aasta, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(aasta, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        int ix, jx, len;
        len = strlen(aasta);//kontrolli jaoks vastuse pikkus
        for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
        {
            if (aasta[ix] == '\'')
            {
                aasta[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                for (jx = ix; jx < len; jx++)
                {
                    aasta[jx] = aasta[jx + 1];//järgmised karakterid liigutab ettepoole
                }
                len--;//vähendab vastuse pikkust
                ix--;//vähendab kontrollmuutujat
            }
        }
        printf("Sisestage koopia triipkood: \n");
        if (fgets(triipkood, sizeof triipkood, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(triipkood, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        len = strlen(triipkood);//kontrolli jaoks vastuse pikkus
        for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
        {
            if (triipkood[ix] == '\'')
            {
                triipkood[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                for (jx = ix; jx < len; jx++)
                {
                    triipkood[jx] = triipkood[jx + 1];//järgmised karakterid liigutab ettepoole
                }
                len--;//vähendab vastuse pikkust
                ix--;//vähendab kontrollmuutujat
            }
        }
        printf("Sisestage koopia keel: \n");
        if (fgets(keel, sizeof keel, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(keel, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }

        len = strlen(keel);//kontrolli jaoks vastuse pikkus
        for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
        {
            if (keel[ix] == '\'')
            {
                keel[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                for (jx = ix; jx < len; jx++)
                {
                    keel[jx] = keel[jx + 1];//järgmised karakterid liigutab ettepoole
                }
                len--;//vähendab vastuse pikkust
                ix--;//vähendab kontrollmuutujat
            }
        }

        if (mysql_query(con, "SELECT *FROM kohaviit")) {//saadab käsu kõikide kohaviitade valimiseks serverile
            fprintf(stderr, "%s\n", mysql_error(con));
            exit(1);//kui tekib viga, lahkub programmist 
        }
        res = mysql_use_result(con);//salvestab saadud tulemused
        printf("Raamatukogus olevate kohaviitade id-ed ja nimed: \n");
        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
        }
        printf("Sisestage kohaviida id: \n");
        if (fgets(d, sizeof d, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        printf("\nMitu selliste andmetega koopiat soovite lisada?\n  ");
        char t[10];
        if (fgets(t, sizeof t, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(t, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        int valik;
        valik = atoi(t);//muudab vastuse numbriks
        raamatud_id = atoi(c);//muudab vastuse numbriks
        kohaviit_id = atoi(d);//muudab vastuse numbriks
        int i;
        for (i = 0; i < valik; i++) {
            sprintf(a, "INSERT INTO koopia (aasta, triipkood, keel, raamatud_id, kohaviit_id) VALUES ('%s', '%s', '%s', '%d', '%d')", aasta, triipkood, keel, raamatud_id, kohaviit_id);//lisab kokku MySQLi käsu tabelisse lisamiseks ja kasutaja sisestatud vastused
            if (mysql_query(con, a)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }
        }
        char m[100];

        printf("Kas soovid lisada veel ühe koopia?\n");//pakub võimalust veel ühe koopia lisamiseks
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        n = atoi(m);//muudab vastuse numbriks

    }



    valimine(con, res, row, tegevus); //tagasi valimise menüüsse
}

void kuvamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int id = mysql_insert_id(con); //viimase INSERT käsu id

    printf("The last inserted row id is: %d\n", id); //prindib välja eelnevalt saadud id
    if (mysql_query(con, "SELECT raamatud.nimi, autorid.nimi FROM raamatud JOIN raamatud_has_autorid ON raamatud.id = raamatud_has_autorid.raamatud_id JOIN autorid ON autorid.id = raamatud_has_autorid.autorid_id")) { //saadab käsu raamatute ja autorite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(2); //kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused

    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    valimine(con, res, row, tegevus); //tagasi valimise menüüsse

}

void autori_kuvamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {

    if (mysql_query(con, "SELECT * FROM autorid")) {//saadab käsu kõikide autorite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(2);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused

    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse

}

void zanri_kuvamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {

    if (mysql_query(con, "SELECT * FROM sanrid")) {//saadab käsu kõikide žanrite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(2);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused

    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse

}

void koopia_kuvamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus){
    printf("Kas soovite näha olemasolevaid või laenutatud koopiaid?\n");
    printf("-> 1 Olemasolevad\n");
    printf("-> 2 Laenutatud\n");
    int tegevus11;
    char a[3];
    do{
        if (fgets(a, sizeof a, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(a, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        tegevus11 = atoi(a);//muudab vastuse numbriks
        if(tegevus11 > 0 && tegevus11 < 3){//sisestatud vastus peab olema arvude 0 ja 3 vahel
            switch(tegevus11){
                case 1://kui valitakse esimene tegevus
                    if (mysql_query(con, "SELECT * FROM koopia WHERE laenutatud_id IS NULL")) {//saadab käsu olemasolevate koopiate valimiseks serverile
                        fprintf(stderr, "%s\n", mysql_error(con));//kui tekib viga, lahkub programmist 
                        exit(2);
                    }
                    res = mysql_use_result(con);//salvestab saadud tulemused

                    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
                    }
                    mysql_free_result(res);//vabastab tulemused
                    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
                    break;//lahkub switchist
                case 2://kui valitakse teine tegevus
                    if (mysql_query(con, "SELECT * FROM koopia WHERE laenutatud_id IS NOT NULL")) {//saadab käsu laenutatud koopiate valimiseks serverile
                        fprintf(stderr, "%s\n", mysql_error(con));
                        exit(2);//kui tekib viga, lahkub programmist 
                    }
                    res = mysql_use_result(con);//salvestab saadud tulemused

                    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
                        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
                    }
                    mysql_free_result(res);//vabastab tulemused
                    valimine(con, res, row, tegevus); //tagasi valimise menüüsse
                    break;//lahkub switchist
            }
        }
    }while(tegevus11 < 0 || tegevus11 > 3);

}

void otsimine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char vastus[100];
    char a[500];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
	int k = 0;
    while (k == 0) {//loop mitme raamatu otsimiseks
    printf("Sisestage raamatu nimi:\n");
    if (fgets(vastus, sizeof vastus, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(vastus, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int i, j, len;
    len = strlen(vastus);//kontrolli jaoks vastuse pikkus
    for (i = 0; i < len; i++)//loop ülakoma kontrolliks
    {
        if (vastus[i] == '\'')
        {
            vastus[i] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (j = i; j < len; j++)
            {
                vastus[j] = vastus[j + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            i--;//vähendab kontrollmuutujat
        }
    }

    sprintf(a, "SELECT raamatud.nimi, autorid.nimi FROM raamatud JOIN raamatud_has_autorid ON raamatud.id = raamatud_has_autorid.raamatud_id JOIN autorid ON autorid.id = raamatud_has_autorid.autorid_id WHERE raamatud.nimi like '%%%s%%'", vastus);//lisab kokku MySQLi käsu sarnaste nimedega raamatute koos autoitega valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    printf("Leitud vasted: \n");
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s --- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
	char c[100];
	printf("Kas soovite otsida veel ühe raamatu?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(c);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus); //tagasi valimise menüüsse
}

void autori_otsimine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus){
    char vastus[100];
    char a[500];
	int k = 0;
    while (k == 0) { //loop mitme raamatu otsimiseks
    if (mysql_query(con, "SELECT * FROM autorid")) {//saadab käsu kõikide autorite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s --- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    printf("Sisestage autori id:\n");
    if (fgets(vastus, sizeof vastus, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(vastus, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int id = atoi(vastus);//muudab vastuse numbriks
    sprintf(a, "SELECT raamatud.nimi FROM raamatud JOIN raamatud_has_autorid ON raamatud.id = raamatud_has_autorid.raamatud_id WHERE raamatud_has_autorid.autorid_id = '%d'", id);//lisab kokku MySQLi käsu autori raamatute valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    printf("Leitud vasted: \n");
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s\n", row[0]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
	char c[100];
	printf("Kas soovite otsida veel ühte raamatut?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(c);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus);

}

void zanri_otsimine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus){
    char vastus[100];
    char a[500];
	int k = 0;
    while (k == 0) { //loop mitme raamatu otsimiseks
    if (mysql_query(con, "SELECT * FROM sanrid")) {//saadab käsu kõikide žanrite valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s --- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    printf("Sisestage zanri id:\n");
    if (fgets(vastus, sizeof vastus, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(vastus, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int id = atoi(vastus);//muudab vastuse numbriks
    sprintf(a, "SELECT raamatud.nimi FROM raamatud JOIN raamatud_has_sanrid ON raamatud.id = raamatud_has_sanrid.raamatud_id WHERE raamatud_has_sanrid.sanrid_id = '%d'", id);//lisab kokku MySQLi käsu žanri raamatute valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    printf("Leitud vasted: \n");
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s\n", row[0]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
	char c[100];
	printf("Kas soovite otsida veel ühte raamatut?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(c, sizeof c, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(c, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(c);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void laenutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char vastus[100];//massiiv vastuse hoidmiseks
    char a[500]; //massiiv andmebaasiga suhtlemiseks
    char b[10]; //massiiv raamatu id hoidmiseks
    char c[500]; //massiiv andmebaasiga suhtlemiseks
    int raamat_id; //raamatu id hoidmiseks
    char *hx = "\0"; //muutuja ülakoma eemaldamiseks
	int k = 0; //muutuja k on võrdne nulliga
    while (k == 0) { //kuni k on null ole loopis

    printf("Sisestage raamatu nimi mida tahate laenutada:\n");//prindi ekraanile tekst
    if (fgets(vastus, sizeof vastus, stdin)) {//kui sisestatakse tekst, võta sisse kasutaja sisestatud tekst
        char* newline = strchr(vastus, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0; //kui oli reavahetus, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(vastus); //saame sisestatud teksti pikkuse
    for (ix = 0; ix < len; ix++) // loop mis käib kuni ix on väiksem sisestatud teksti pikkusest
    {
        if (vastus[ix] == '\'')//kontroll kas karakter on ülakoma
        {
            vastus[ix] = *hx; //karakter võrdub nüüd \0
            for (jx = ix; jx < len; jx++)//uus loop, mis vähendab vastuse pikkust
            {
                vastus[jx] = vastus[jx + 1]; //kogu tekst liigub ühe karakteri võrra tagasi
            }
            len--;//vähenda vastuse pikkust
            ix--;//vähenda ix pikkust (mõjutab kontrollloopi while)
        }
    }
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    /**Näita andmebaasist tabelist raamatud id ja nimi,tabelist autorid autori nimi. 
    Selleks ühenda raamatud, raamatud_has_autorid kasutades raamatud id-d. 
    Ühenda raamatud_has_autorid.raamatud_id autorite tabeliga. 
    Otsi uuest ühendatud tabelist raamatuid mille nimi on sisestatud vastus**/ 
    sprintf(a, "SELECT raamatud.id, raamatud.nimi, autorid.nimi FROM raamatud JOIN raamatud_has_autorid ON raamatud.id = raamatud_has_autorid.raamatud_id JOIN autorid ON autorid.id = raamatud_has_autorid.autorid_id WHERE raamatud.nimi like '%%%s%%'", vastus);
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));//kui tekib viga, lahkub programmist
        exit(1);
    }
    res = mysql_use_result(con);//salvestab saadud tulemuse res-i
    printf("Leitud vasted: \n");//prindi ekraanile
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s --- %s\n", row[0], row[1], row[2]);//prindi tulemused
    }
    if(res == NULL){//kui saadud tulemuse ridade arv andmebaasis on NULL
        printf("Vasted puuduvad\n");//prindi tekst
        laenutamine(con, res, row, tegevus);//mine tagasi laenutamise alamprogrammi algusesse
    }
    mysql_free_result(res);//vabasta varem salvestatud tulemus
    printf("Sisestage raamatu, mida soovite laenutada, id: \n"); //prindi tekst
    if (fgets(b, sizeof b, stdin)) {//kui sisestatakse tekst, võta sisse kasutaja sisestatud tekst
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus
        if (newline) {//kui oli reavahetus, siis kustutab selle
            *newline = 0;
        }
    }
    char o[600];
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    //sama andmebaasi käsk, mis enne
     sprintf(o, "SELECT raamatud.id, raamatud.nimi, autorid.nimi FROM raamatud JOIN raamatud_has_autorid ON raamatud.id = raamatud_has_autorid.raamatud_id JOIN autorid ON autorid.id = raamatud_has_autorid.autorid_id WHERE raamatud.nimi like '%%%s%%'", vastus);
    if (mysql_query(con, o)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));//kui tekib viga prindi error
        exit(1);//ja lahku programmist
    }
    res = mysql_use_result(con);//kui ei tekkinud viga siis salvesta saadud tulemus
    if(res == NULL){//kui res ei eksisteeri
        printf("Vastete seas pole sellist raamatut!\n");
        laenutamine(con, res, row, tegevus);//alusta alamprogrammi laenutamine uuesti
    }
    mysql_free_result(res);//vabasta varem salvestatud tulemus
    raamat_id = atoi(b);//atoi muudab stringi b sisu integeriks ning raamat_id hoiab seda
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    /*Näita koopia id, raamatu nimi, koopia aasta ja keel, selleks ühenda kaks tabelit raamatud ja koopia
    Ühendamiseks kasuta raamatu id-d. Raamatu id on kasutaja sisestatud suurus*/
    sprintf(c, "SELECT koopia.idkoopia, raamatud.nimi, koopia.aasta, koopia.keel FROM raamatud JOIN koopia ON raamatud.id = koopia.raamatud_id WHERE raamatud.id = '%d'", raamat_id);
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));//prindi error kui tekkis viga
        exit(1);//lahku programmist
    }
    res = mysql_use_result(con);//salvesta saadud tulemus
    while ((row = mysql_fetch_row(res)) != NULL) {//kuni saadud tulemusel on ridu
        printf("%s %s %s %s \n", row[0], row[1], row[2], row[3]);//prindi read
    }
    mysql_free_result(res);//vabasta tulemus
    char n[600];
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    //sama käsk mis eelmine
    sprintf(n, "SELECT koopia.idkoopia, raamatud.nimi, koopia.aasta, koopia.keel FROM raamatud JOIN koopia ON raamatud.id = koopia.raamatud_id WHERE raamatud.id = '%d'", raamat_id);
    if (mysql_query(con, n)) {//kui tekib error
        fprintf(stderr, "%s\n", mysql_error(con));//prindi error sõnum
        exit(1);//välju programmist
    }
    res = mysql_use_result(con);//kui polnud errorit salvesta saadud tulemus
    if(res == NULL){//kui tulemus puudub
        printf("Vastete seas pole sellist koopiat!\n");
        laenutamine(con, res, row, tegevus);//alusta alamprogrammi laenutamine uuesti
    }
    mysql_free_result(res);//vabasta tulemus
    char d[10];
    int koopia_id;
    printf("Sisestage ülaltoodud koopiate seast meelepärane id: \n");//prindi tekst
    if (fgets(d, sizeof d, stdin)) {//kui sisestatakse tekst
        char* newline = strchr(d, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus. Kui on salvestab newline'i
        if (newline) {//kui newline eksisteerib
            *newline = 0;//kui oli reavahetus, siis kustutab selle
        }
    }
    koopia_id = atoi(d);//atoid muudab stringi d väärtuse integeriks ning koopia-_d hoiab seda
    printf("Kasutusel olevad id numbrid:\n");
    if (mysql_query(con, "SELECT * FROM laenutatud")) {//suhtle andmebaasiga, otsi tabelist laenutatud
        fprintf(stderr, "%s\n", mysql_error(con));//kui ebaõnnestus prindi error
        exit(1);//välju programmist
    }
    res = mysql_use_result(con);//salvesta saadud tulemus
    while ((row = mysql_fetch_row(res)) != NULL) {//kuni tulemusel on ridu
        printf("%s \n", row[0]);//prindi need
    }
    mysql_free_result(res);//vabasta tulemus
    char e[10];
    int laenutatud_id;
    printf("Sisestage vaba laenutuse id:\n");//prindi
    if (fgets(e, sizeof e, stdin)) {//kui sisestatakse tekst
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus salvesta newline'i
        if (newline) {//kui newline eksisteerib
            *newline = 0;//kui oli reavahetus, siis kustutab selle
        }
    }
    laenutatud_id = atoi(e);//atoi muudab stringi e integeriks ning laenutatud_id hoiab seda
    char laenid[100];
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    //näita tabelist laenutatud kõik mille id võrdub laenutatud_id-ga
    sprintf(laenid, "SELECT * FROM laenutatud WHERE id='%d'", laenutatud_id);
    if (mysql_query(con, laenid)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));//kui oli error prindi põhjus
        exit(1);//ja lahku 
    }
    res = mysql_use_result(con);//salvesta tulemus
    if(mysql_num_rows(res) != 0){//kui tulemus on olemas
        printf("Laenutuse id on juba kasutusel!\n");//anna tekst
        mysql_free_result(res);//vabasta tulemus
        laenutamine(con, res, row, tegevus);//alusta alamprogrammi uuesti
    }
    mysql_free_result(res);//vabasta tulemus


    char f[10];
    int user_id;
    printf("Sisestage laenutaja id:\n");//küsi kasutajalt infot
    if (fgets(f, sizeof f, stdin)) {//kui sistatakse tekst salvesta see f massiivi
        char* newline = strchr(f, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus
        if (newline) {//kui on reavahetus
            *newline = 0;//kustuta see
        }
    }
    user_id = atoi(f);//atoi muudab stringi f integeriks ning seda hoiab user_id
    char g[10];
    int worker_id;
    printf("Sisestage töötaja id:\n");//küsi kasutajalt infot
    if (fgets(g, sizeof g, stdin)) {//kui sistatakse tekst salvesta see g massiivi
        char* newline = strchr(g, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus
        if (newline) {//kui on reavahetus
            *newline = 0;//kustuta see
        }
    }
    worker_id = atoi(g);//atoi muudab stringi g integeriks ning seda hoiab worker_id
    char kuupaev[20];
    printf("Sisestage kuupäev formaadis AASTA-KUU-PÄEV (nt. 2022-08-12):\n");
   //küsi kasutajalt infot
    if (fgets(kuupaev, sizeof kuupaev, stdin)) {//kui sistatakse tekst salvesta see kuupaev massiivi
        char* newline = strchr(kuupaev, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus
        if (newline) {//kui on reavahetus
            *newline = 0;//kustuta see
        }
    }
    len = strlen(g);//saame teada stringi g pikkuse
    for (ix = 0; ix < len; ix++)//loop kuni ix on väiksem stringi g pikkusest
    {
        if (g[ix] == '\'')//kontroll kas karakter on ülakoma
        {
            g[ix] = *hx; //karakter võrdub nüüd \0
            for (jx = ix; jx < len; jx++)////järgmised karakterid liigutab ettepoole
            {
                g[jx] = g[jx + 1];
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    char h[500];
    char i[200];
    //lisab kokku MySQLi käsu ja kasutaja sisestatud vastuse
    /*sisesta tabelisse laenutatud kasutaja sisestatud väärtused*/
    sprintf(h, "INSERT INTO laenutatud (id, kuupaev, users_idusers, worker_idworker) VALUES ('%d', '%s', '%d', '%d')", laenutatud_id, kuupaev, user_id, worker_id);
    /*lisa koopia juurde laenutatud_id kuna ta on nüüd välja laenutatud*/
    sprintf(i, "UPDATE koopia SET laenutatud_id = '%d' WHERE idkoopia = '%d'", laenutatud_id, koopia_id);
    if (mysql_query(con, h)) {//kui serveriga suhtlemisel tekib viga
        fprintf(stderr, "%s\n", mysql_error(con));//prindi veateade
        exit(1);//lahku
    }
    if (mysql_query(con, i)) {//kui serveriga suhtlemisel tekib viga
        fprintf(stderr, "%s\n", mysql_error(con));//prindi veateade
        exit(1);//lahku
    }
	char m[100];//Uus massiiv m
    printf("Kas soovite laenutada veel ühe raamatu?\n");//prindi ekraanile
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//võta sisse kasutaja sisestus
            char* newline = strchr(m, '\n');//pointer newline on esimene reavahetus sisestutud vastuses
            if (newline) {//kontroll kas sisestuses on reavahetust ehk newline
                *newline = 0;//eemalda reavahetus
            }
			        }
        k = atoi(m); //atoi muudab stringi m integeriks ning k hoiab nüüd seda väärtust

    }


    valimine(con, res, row, tegevus);//naase alamprogrammi valimine
}

void tagastamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char e[100];
    char a[100];
    char b[100];
    int laenutatud_id;
	int k = 0;
    while (k == 0) {//loop mitme raamatu tagastamiseks
    printf("Sisestage laenutuse id:\n");
    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    laenutatud_id = atoi(e);//muudab vastuse numbriks
    time_t t; //muutuja aja jaoks
    t = time(NULL); //muutuja väärtuseks hetke aeg 
    struct tm tm = *localtime(&t); //salvestab aja structi
    printf("Tänane kuupäev:\n%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);//prindib välja saadud kuupäeva
    sprintf(a, "SELECT kuupaev FROM laenutatud WHERE id = '%d'", laenutatud_id);//lisab kokku MySQLi käsu sisestatud id-ga raamatu tagastuskuupäeva valimiseks ja kasutaja sisestatud vastuse

    printf("\nTagastus kuupäev:\n");
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s \n", row[0]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    sprintf(b, "UPDATE koopia SET laenutatud_id =null WHERE laenutatud_id = '%d'", laenutatud_id);//lisab kokku MySQLi käsu laenutatud_id muutmiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }


    sprintf(a, "DELETE FROM laenutatud WHERE id ='%d'", laenutatud_id);//lisab kokku MySQLi käsu koopia laenutatud raamatute seast kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }


    printf("Raamat on tagastatud\n");
	char m[100];
    printf("Kas soovite tagastada veel ühe raamatu?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }


    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void nime_muutmine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char b[400];
    char e[100];
    char nimi[300];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Sisestage raamatu id, mille nime soovite muuta:\n");
    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    printf("Sisestage raamatu nimi, mille nime soovite muuta:\n");
    if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(nimi);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (nimi[ix] == '\'')
        {
            nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    int idnimi;
    idnimi = atoi(e);//muudab vastuse numbriks
    sprintf(b, "UPDATE raamatud SET nimi='%s' WHERE id='%d'", nimi, idnimi);//lisab kokku MySQLi käsu raamatu andmete muutmiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nNimi on muudetud\n");
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void autori_muutmine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char b[400];
    char e[100];
    char nimi[300];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Sisestage autori id, kelle nime soovite muuta:\n");
    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    printf("Sisestage autori nimi, kelle nime soovite muuta:\n");
    if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(nimi);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (nimi[ix] == '\'')
        {
            nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    int idnimi;
    idnimi = atoi(e);//muudab vastuse numbriks
    sprintf(b, "UPDATE autorid SET nimi='%s' WHERE id='%d'", nimi, idnimi);//lisab kokku MySQLi käsu autori andmete uuendamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nNimi on muudetud\n");
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void zanri_muutmine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char b[400];
    char e[100];
    char nimi[300];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Sisestage žanri id, mille nime soovite muuta:\n");
    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    printf("Sisestage žanri nimi, mille nime soovite muuta:\n");
    if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(nimi);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (nimi[ix] == '\'')
        {
            nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    int idnimi;
    idnimi = atoi(e);//muudab vastuse numbriks
    sprintf(b, "UPDATE sanrid SET sanrinimi='%s' WHERE id='%d'", nimi, idnimi);//lisab kokku MySQLi käsu žanri andmete muutmiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub 
    }
    printf("\nNimi on muudetud\n");
    valimine(con, res, row, tegevus);//tagasi valimise menüüsse
}

void uus_kasutaja(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char name[300];
    char ad[400];
    char num[100];
    char l[900];
    char b[800];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Sisestage kasutaja nimi\n");
    if (fgets(name, sizeof name, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(name, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(name);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (name[ix] == '\'')
        {
            name[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                name[jx] = name[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    printf("Sisestage kasutaja aadress\n");
    if (fgets(ad, sizeof ad, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(ad, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    len = strlen(ad);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (ad[ix] == '\'')
        {
            ad[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                ad[jx] = ad[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    printf("Sisestage kasutaja telefoni number\n");
    if (fgets(num, sizeof num, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(num, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    len = strlen(num);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (num[ix] == '\'')
        {
            num[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                num[jx] = num[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    sprintf(l, "INSERT INTO users (nimi, aadress, number) VALUES ('%s', '%s', '%s')", name, ad, num);//lisab kokku MySQLi käsu tabelisse andmete lisamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, l)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nKasutaja on lisatud\n");//tagasiside kasutajale
    sprintf(b, "SELECT *FROM users WHERE number='%s'", num);//lisab kokku MySQLi käsu sisestatud kasutaja valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    printf("Sisestatud kasutaja andmed: \n"); //näitab lisatud andmeid
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s  %s  %s\n", row[0], row[1], row[2], row[3]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemused
    valimine(con, res, row, tegevus); //tagasi peamenüüsse
}

void uus_worker(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char name[300];
    char l[400];
    char a[400];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Sisestage uue töötaja nimi\n");
    if (fgets(name, sizeof name, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(name, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(name);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (name[ix] == '\'')
        {
            name[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                name[jx] = name[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    sprintf(l, "INSERT INTO worker (workername) VALUES ('%s')", name);//lisab kokku MySQLi käsu tabelisse nime lisamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, l)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nTöötaja on lisatud\n"); //tagasiside kasutajale
    sprintf(a, "SELECT *FROM worker WHERE workername='%s'", name);//lisab kokku MySQLi käsu lisatud töötaja valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud andmed
    printf("Sisestatud töötaja andmed: \n"); //näitab sisesatud töötaja andmeid
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s \n", row[0], row[1]);//näitab tulemuseid
    }
    mysql_free_result(res);//vabastab tulemuse muutuja
    valimine(con, res, row, tegevus);//tagasi peamenüüsse

}

void worker_muutmine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char e[200];
    char nimi[200];
    char a[200];
    char b[300];
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    sprintf(a, "SELECT *FROM worker"); //lisab kokku MySQLi käsu kõikide töötajate valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab saadud andmed
    printf("Kõik töötajad: \n"); 
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s \n", row[0], row[1]);//väljastab tulemused
    }
    printf("Sisestage töötaja id, kelle nime soovite muuta:\n");
    if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    printf("Sisestage kasutaja uus nimi:\n");
    if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int ix, jx, len;
    len = strlen(nimi);//kontrolli jaoks vastuse pikkus
    for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
    {
        if (nimi[ix] == '\'')
        {
            nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
            for (jx = ix; jx < len; jx++)
            {
                nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
            }
            len--;//vähendab vastuse pikkust
            ix--;//vähendab kontrollmuutujat
        }
    }
    int idnimi;
    idnimi = atoi(e);//muudab vastuse numbriks
    sprintf(b, "UPDATE worker SET workername='%s' WHERE idworker='%d'", nimi, idnimi);//lisab kokku MySQLi käsu töötajate andmete muutmiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, b)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nNimi on muudetud\n"); //tagasiside kasutajale
    valimine(con, res, row, tegevus); //tagasi peamenüüsse

}

void kasutaja_muutmine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char b[400];
    char e[100];
    char a[10];
    char nimi[300];
    int tegevus2;
    char *hx = "\0";//muutuja ülakoma eemaldamiseks
    printf("Mida soovite muuta? (Valige tegevusele vastav number)\n");
    printf("-> 1 Kasutaja nime\n");
    printf("-> 2 Kasutaja aadressi\n");
    printf("-> 3 Kasutaja telefoni numbrit\n");

    do {//loop tegevuse valimise jaoks, kordab seni kaua kui kasutaja valib ühe ette antud tegevustest
        if (fgets(a, sizeof a, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(a, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        tegevus2 = atoi(a); //muudab kasutaja poolt sisestatud vastuse numbriks
        switch (tegevus2) {
        case 1://kui kasutaja valib nime muutmise
            printf("Sisestage kasutaja id, kelle nime soovite muuta:\n");
            if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            printf("Sisestage kasutaja uus nimi:\n");
            if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            int ix, jx, len;
            len = strlen(nimi);//kontrolli jaoks vastuse pikkus
            for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
            {
                if (nimi[ix] == '\'')
                {
                    nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                    for (jx = ix; jx < len; jx++)
                    {
                        nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
                    }
                    len--;//vähendab vastuse pikkust
                    ix--;//vähendab kontrollmuutujat
                }
            }
            int idnimi;
            idnimi = atoi(e);//muudab vastuse numbriks
            sprintf(b, "UPDATE users SET nimi='%s' WHERE idusers='%d'", nimi, idnimi);//lisab kokku MySQLi käsu tabelis andmete muutmiseks ja kasutaja sisestatud vastuse
            if (mysql_query(con, b)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }
            printf("\nNimi on muudetud\n");
            break;//lahkub switchist
        case 2:
            printf("Sisestage kasutaja id, kelle aadressit soovite muuta:\n");
            if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            printf("Sisestage kasutaja uus aadress:\n");
            if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }

            len = strlen(nimi);//kontrolli jaoks vastuse pikkus
            for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
            {
                if (nimi[ix] == '\'')
                {
                    nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                    for (jx = ix; jx < len; jx++)
                    {
                        nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
                    }
                    len--;//vähendab vastuse pikkust
                    ix--;//vähendab kontrollmuutujat
                }
            }
            int idnimi2;
            idnimi2 = atoi(e);//muudab vastuse numbriks
            sprintf(b, "UPDATE users SET aadress='%s' WHERE idusers='%d'", nimi, idnimi2);//lisab kokku MySQLi käsu tabelis andmete muutmiseks ja kasutaja sisestatud vastuse
            if (mysql_query(con, b)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }
            printf("\nAadress on muudetud\n");
            break;//lahkub switchist
        case 3:
            printf("Sisestage kasutaja id, kelle telefoni numbrit soovite muuta:\n");
            if (fgets(e, sizeof e, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(e, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            printf("Sisestage kasutaja uus number:\n");
            if (fgets(nimi, sizeof nimi, stdin)) {//loeb kasutaja sisestust
                char* newline = strchr(nimi, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                if (newline) {
                    *newline = 0;//kui reavahetus oli, siis kustutab selle
                }
            }
            len = strlen(nimi);//kontrolli jaoks vastuse pikkus
            for (ix = 0; ix < len; ix++)//loop ülakoma kontrolliks
            {
                if (nimi[ix] == '\'')
                {
                    nimi[ix] = *hx;//kui ülakoma on, siis kustutab selle karakteri
                    for (jx = ix; jx < len; jx++)
                    {
                        nimi[jx] = nimi[jx + 1];//järgmised karakterid liigutab ettepoole
                    }
                    len--;//vähendab vastuse pikkust
                    ix--;//vähendab kontrollmuutujat
                }
            }
            int idnimi3;
            idnimi3 = atoi(e);//muudab vastuse numbriks
            sprintf(b, "UPDATE users SET number='%s' WHERE idusers='%d'", nimi, idnimi3);//lisab kokku MySQLi käsu tabelis andmete muutmiseks ja kasutaja sisestatud vastuse
            if (mysql_query(con, b)) {//saadab käsu serverile
                fprintf(stderr, "%s\n", mysql_error(con));
                exit(1);//kui tekib viga, lahkub programmist 
            }
            printf("\nNumber on muudetud\n");
            break;//lahkub switchist

        }
    } while (tegevus2 > 0 && tegevus2 < 4);//sisestatud number peab olema 1-3

    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}

void worker_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char nim[300];
    char l[300];
    if (mysql_query(con, "SELECT *FROM worker")) {//saadab käsu kõikide töötajate valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab valitud tulemused
    printf("Kõik töötajad: \n");
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s \n", row[0], row[1]);//väljastab tulemused
    }
    printf("Sisestage töötaja id, kelle andmeid soovite kustutada\n");
    if (fgets(nim, sizeof nim, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nim, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int id;
    id = atoi(nim);//muudab vastuse numbriks
    sprintf(l, "DELETE FROM worker WHERE idworker = '%d'", id);//lisab kokku MySQLi käsu sisestatud id-ga töötaja kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, l)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nTöötaja on kustutatud\n");//tagasiside kasutajale
    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}

void kasutaja_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char nim[300];
    char l[300];
    printf("Sisestage kasutaja id, kelle andmeid soovite kustutada\n");
    if (fgets(nim, sizeof nim, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(nim, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    int id;
    id = atoi(nim);//muudab vastuse numbriks
    sprintf(l, "DELETE FROM users WHERE idusers = '%d'", id);//lisab kokku MySQLi käsu sisestatud id-ga kasutaja kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, l)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    printf("\nKasutaja on kustutatud\n");//tagasiside kasutajale
    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}

void kasutajate_kuvamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    if (mysql_query(con, "SELECT * FROM users")) {//saadab käsu kõikide kasutajate valimiseks serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(2);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab valitud andmed

    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s ----- %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemuse muutuja
    valimine(con, res, row, tegevus);//tagasi peamenüüsse

}

int valimine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    char b[20];
    char a[20];
    char f[200];
    int tegevus4;
    printf("\nValige tegevus, mida soovite ning sisestage sellele vastav number: \n");
    printf("-> 1 Kuvamine\n");
    printf("-> 2 Lisamine\n");
    printf("-> 3 Raamatute otsimine nime järgi\n");
    printf("-> 4 Raamatute otsimine autori või zanri järgi\n");
    printf("-> 5 Raamatu\n");
    printf("-> 6 Raamatu tagastamine\n");
    printf("-> 7 Kustutamine\n");
    printf("-> 8 Muuda olemasolevat sissekannet\n");
    printf("-> 9 Lahku programmist\n");
    do {//tsükkel tegevuse valimiseks nii kaua kui kasutaja sisestab 
        if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        tegevus = atoi(b);//muudab vastuse numbriks
        if (tegevus > 0 && tegevus < 10)//sisestatud vastus peab olema 1-9
        {
            switch (tegevus) {
                char* tegevusstr;//muutuja valitud tegevuse kuvamiseks
            case 1:
                tegevusstr = "Kuvamine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                printf("Mida soovite näha? (Valige tegevusele vastav number)\n");
                printf("-> 1 Raamatuid\n");
                printf("-> 2 Autoreid\n");
                printf("-> 3 Žanreid\n");
                printf("-> 4 Kasutajaid\n");
                printf("-> 5 Koopiaid\n");
                int tegevus10;

                do {//tsükkel seni kuni kasutaja sisestab aksepteeritava vastuse
                    if (fgets(a, sizeof a, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(a, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    tegevus10 = atoi(a);//muudab sisestuse numbriks
                    switch (tegevus10) {
                    case 1:
                        kuvamine(con, res, row, tegevus);//raamatute kuvamise funktsioon
                        break;//lahkub switchist
                    case 2:
                        autori_kuvamine(con, res, row, tegevus);//autorite kuvamise funktsioon
                        break;//lahkub switchist
                    case 3:
                        zanri_kuvamine(con, res, row, tegevus);//žanrite kuvamise funktsioon
                        break;//lahkub switchist

                    case 4:
                        kasutajate_kuvamine(con, res, row, tegevus);//kasutajate kuvamise funktsioon
                        break;//lahkub switchist
                    case 5:
                        koopia_kuvamine(con, res, row, tegevus);//koopiate kuvamise funktsioon
                        break;//lahkub switchist
                    }
                } while (tegevus10 > 0 && tegevus10 < 6);//valitud tegevus peab olema 1-5
                break;//lahkub switchist

            case 2:
                tegevusstr = "lisamine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                int tegevus2;
                printf("Mida soovite lisada? (Valige tegevusele vastav number)\n");
                printf("-> 1 Raamat\n");
                printf("-> 2 Autor\n");
                printf("-> 3 Zanr\n");
                printf("-> 4 Raamatule autor\n");
                printf("-> 5 Raamatule zanr\n");
                printf("-> 6 Kohaviit\n");
                printf("-> 7 Koopia\n");
                printf("-> 8 Uus kasutaja\n");
                printf("-> 9 Uus töötaja\n");
                printf("-> 10 Lahkumine\n");
                do {//loop seni kaua kui kasutaja sisestab vastuse vahemikus 1-10
                    if (fgets(a, sizeof a, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(a, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    tegevus2 = atoi(a);//muudab sisestuse numbriks
                    switch (tegevus2) {
                    case 1:
                        printf("Andmebaasis olevad raamatud\n");
                        if (mysql_query(con, "SELECT *FROM raamatud")) {//saadab serverile käsu valida andmebaasist kõik raamatute andmed
                            fprintf(stderr, "%s\n", mysql_error(con));
                            exit(1);//kui tekib viga, lahkub programmist 
                        }
                        res = mysql_use_result(con);//salvestab saadud tulemused
                        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kui vastuses on ridu
                            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
                        }
                        mysql_free_result(res);//vabastab tulemuse muutuja
                        lisamine(con, res, row, tegevus);//tagasi lisamise menüüsse
                        break;//lahkub switchist
                    case 2:
		                printf("Autorid ja nende id-d:\n");
                        if (mysql_query(con, "SELECT *FROM autorid")) {//saadab serverile käsu valida andmebaasist kõik autorite andmed
                            fprintf(stderr, "%s\n", mysql_error(con));
                            exit(1);//kui tekib viga, lahkub programmist 
                        }
                        res = mysql_use_result(con);//salvestab saadud tulemused
                        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kui vastuses on ridu
                            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
                        }
                        autori_lisamine(con, res, row, tegevus);//autori lisamise funktsioon
                        break;//lahkub switchist
                    case 3:
                        zanri_lisamine(con, res, row, tegevus);//žanri lisamise funktsioon
                        break;//lahkub switchist
                    case 4:
                        raamatule_autor(con, res, row, tegevus);//raamatule autori lisamise funktsioon
                        break;//lahkub switchist
                    case 5:
                        raamatule_zanr(con, res, row, tegevus);//raamatule žanri lisamise funktsioon
                        break;//lahkub switchist
                    case 6:
                        kohaviida_lisamine(con, res, row, tegevus);//kohaviida lisamise funktsioon
                        break;//lahkub switchist
                    case 7:
                        koopia_lisamine(con, res, row, tegevus);//koopia lisamise funktsioon
                        break;//lahkub switchist
                    case 8:
                        uus_kasutaja(con, res, row, tegevus);//uue kasutaja lisamise funktsioon
                        break;//lahkub switchist
                    case 9:
                        uus_worker(con, res, row, tegevus);//uue töötaja lisamise funktsioon
                        break;//lahkub switchist

                    case 10:
                        printf("Valisite tegevuse lahkumine.\n");//kui valitakse lahkumine
                        valimine(con, res, row, tegevus);//tagasi peamenüüsse
                        break;//lahkub switchist
                    }
                } while (tegevus2 > 0 && tegevus2 < 11);//sisestus peab olema vahemikus 1-10

                break;//lahkub switchist

            case 3:
                tegevusstr = "raamatute otsimine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                otsimine(con, res, row, tegevus);//raamatute otsimise funktsioon
                break;//lahkub switchist
            case 4:
                tegevusstr = "otsimine autori või žanri järgi";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                printf("Kas soovite otsida autori või žanri järgi?\n");
                printf("-> 1 Autori\n");
                printf("-> 2 Žanri\n");
                char vas[10];
                int tegevus00;
                 do {
                    if (fgets(vas, sizeof vas, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(vas, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    tegevus00 = atoi(vas);//muudab sisestatud vastuse numbriks
                    switch (tegevus00) {
                    case 1:
                        autori_otsimine(con, res, row, tegevus);//autori järgi raamatute otsimise funktsioon
                        break;//lahkub switchist
                    case 2:
                        zanri_otsimine(con, res, row, tegevus);//žanri järgi raamatute otsimise funktsioon
                        break;//lahkub switchist
                    }
                } while (tegevus00 > 0 && tegevus00 < 3);//sisestatud väärtus peab olema vahemikus 1-2
                break;//lahkub switchist
            case 5:
                tegevusstr = "raamatu laenutamine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                laenutamine(con, res, row, tegevus);//raamatute laenutamise funktsioon
                break;//lahkub switchist
            case 6:
                tegevusstr = "raamatu tagastamine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                tagastamine(con, res, row, tegevus);//raamatute tagastamise funktsioon
                break;//lahkub switchist
            case 7:
                tegevusstr = "kustutamine";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                char i[10];
                int tegevus3;
                printf("Mida soovite kustutada? (Valige tegevusele vastav number)\n");
                printf("-> 1 Raamat\n");
                printf("-> 2 Autor\n");
                printf("-> 3 Žanr\n");
                printf("-> 4 Kohaviit\n");
                printf("-> 5 Koopia\n");
                printf("-> 6 Kasutaja andmed\n");
                printf("-> 7 Töötaja andmed\n");
                printf("-> 8 Lahkumine\n");
                do {//tsükkel seni kaua kui kasutaja sisestab oodatud vastuse
                    if (fgets(i, sizeof i, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(i, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    tegevus3 = atoi(i);//muudab sisestatud vastuse numbriks
                    switch (tegevus3) {
                    case 1:
                        raamatu_kustutamine(con, res, row, tegevus);//raamatute kustutamise funktsioon
                        break;//lahkub switchist
                    case 2:
                        autori_kustutamine(con, res, row, tegevus);//autori kustutamise funktsioon
                        break;//lahkub switchist
                    case 3:
                        if (mysql_query(con, "SELECT *FROM sanrid")) {//saadab serverile käsu valida andmebaasist kõik žanrite andmed
                            fprintf(stderr, "%s\n", mysql_error(con));
                            exit(1);//kui tekib viga, lahkub programmist 
                        }
                        res = mysql_use_result(con);//salvestab saadud tulemused
                        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kui vastuses on ridu
                            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
                        }
                        mysql_free_result(res);//vabastab tulemuse muutuja
                        zanri_kustutamine(con, res, row, tegevus);//žanri kustutamise funktsioon
                        break;//lahkub switchist
                    case 4:
                        if (mysql_query(con, "SELECT *FROM kohaviit")) {//saadab serverile käsu andmebaasist kohaviitade andmete valimiseks
                            fprintf(stderr, "%s\n", mysql_error(con));
                            exit(1);//kui tekib viga, lahkub programmist 
                        }
                        res = mysql_use_result(con);//salvestab saadud tulemused
                        while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kui vastuses on ridu
                            printf("%s %s\n", row[0], row[1]);//väljastab tulemused
                        }
                        mysql_free_result(res);//vabastab tulemuse muutuja
                        kohaviida_kustutamine(con, res, row, tegevus);//kohaviida kustutamise funktsioon
                        break;//lahkub switchist
                    case 5:
                        koopia_kustutamine(con, res, row, tegevus);//koopia kustutamise funktsioon
                        break;//lahkub switchist
                    case 6:
                        kasutaja_kustutamine(con, res, row, tegevus);//kasutaja kustutamise funktsioon
                        break;//lahkub switchist
                    case 7:
                        worker_kustutamine(con, res, row, tegevus);//töötaja kustutamise funktsioon
                        break;//lahkub switchist
                    case 8:
                        printf("Valisite tegevuse lahkumine\n");//kui kasutaja valib lahkumise
                        valimine(con, res, row, tegevus);//tagasi peamenüüsse
                        break;//lahkub switchist
                    }
                } while (tegevus3 > 0 && tegevus3 < 9);//sisestatud tegevus peab olema vahemikus 1-8

                break;//lahkub switchist
            case 8:
                tegevusstr = "Muuda sissekannet";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                printf("Mida soovite muuta? (Valige tegevusele vastav number)\n");
                printf("-> 1 Raamatu nime\n");
                printf("-> 2 Autori nime\n");
                printf("-> 3 Žanri nime\n");
                printf("-> 4 Kasutaja informatsiooni\n");
                printf("-> 5 Töötaja informatsiooni\n");
                do {//tsükkel seni kaua kui kasutaja sisestab oodatud vastuse
                    if (fgets(a, sizeof a, stdin)) {//loeb kasutaja sisestust
                        char* newline = strchr(a, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
                        if (newline) {
                            *newline = 0;//kui reavahetus oli, siis kustutab selle
                        }
                    }
                    tegevus4 = atoi(a);//muudab sisestatud vastuse numbriks
                    switch (tegevus4) {
                    case 1:
                        nime_muutmine(con, res, row, tegevus);//raamatu nime muutmise funktsioon
                        break;//lahkub switchist
                    case 2:
                        autori_muutmine(con, res, row, tegevus);//autori muutmise funktsioon
                        break;//lahkub switchist
                    case 3:
                        zanri_muutmine(con, res, row, tegevus);//žanri muutmise funktsioon
                        break;//lahkub switchist
                    case 4:
                        kasutaja_muutmine(con, res, row, tegevus);//kasutaja andmete muutmise funktsioon
                        break;//lahkub switchist
                    case 5:
                        worker_muutmine(con, res, row, tegevus);//töötaja andmete muutmise funktsioon
                        break;//lahkub switchist

                    }
                } while (tegevus > 0 && tegevus < 6);//sisestatud vastus peab olema vahemikus 1-5

                break;//lahkub switchist
            case 9://kui kasutaja valib programmist lahkumise
                tegevusstr = "lahkumine programmist";
                printf("\n----- Valisite tegevuse %s -----\n", tegevusstr);
                mysql_close(con);//ühendus andmebaasi serveriga katkestatakse
                exit(0);//programm lõpetab töö
                break;//lahkub switchist

            }
        }
    } while (tegevus > 0 && tegevus < 9);//tegevus ei tohi olla väiksem kui 1 ja suurem kui 8
        printf("Te ei valinud tegevust! \n");
        valimine(con, res, row, tegevus);//tagasi peamenüüsse

    return 0;//tagastab nulli
}

void raamatu_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int raamat_id;
    char a[100];
    char b[10];
    char c[100];
    char d[100];
    char e[100];
	int k = 0;
    while (k == 0) {//tsükkel mitme raamatu kustutamiseks
    if (mysql_query(con, "SELECT *FROM raamatud")) {//saadab serverile käsu kõikide raamatute valimiseks
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con);//salvestab valitud andmed
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab tulemused
    }
    mysql_free_result(res);//vabastab tulemuse muutuja

    printf("Sisestage raamatu id, mida soovite kustutada: \n");
    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    raamat_id = atoi(b);
    sprintf(c, "DELETE FROM raamatud_has_autorid WHERE raamatud_id = '%d'", raamat_id);//lisab kokku MySQLi käsu andmete kustutamiseks ja kasutaja sisestatud id
    sprintf(d, "DELETE FROM raamatud_has_sanrid WHERE raamatud_id = '%d'", raamat_id);//lisab kokku MySQLi käsu andmete kustutamiseks ja kasutaja sisestatud id
    sprintf(e, "DELETE FROM koopia WHERE raamatud_id = '%d'", raamat_id);//lisab kokku MySQLi käsu raamatu koopiate kustutamiseks ja kasutaja sisestatud id
    sprintf(a, "DELETE FROM raamatud WHERE id = '%d'", raamat_id);//lisab kokku MySQLi käsu raamatu kustutamiseks ja kasutaja sisestatud id
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, d)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, e)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
	char m[100];
    printf("Kas soovite kustutada veel ühe raamatu?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus); //tagasi peamenüüsse
}

void autori_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int autor_id;
    char a[100];
    char b[10];
    char c[100];
	int k = 0;
    while (k == 0) {//loop mitme autori kustutamiseks
    if (mysql_query(con, "SELECT *FROM autorid")) {//saadab serverile käsu kõikide autorite valimiseks
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con); //salvestab tulemused
    while ((row = mysql_fetch_row(res)) != NULL) {//loop seni kaua kuni andmebaasist saadud vastel on ridu
        printf("%s %s\n", row[0], row[1]);//väljastab valitud tulemused
    }
    mysql_free_result(res); //vabastab tulemuse muutuja
    printf("Sisestage autori id, mida soovite kustutada: \n");
    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    autor_id = atoi(b); //muudab sisestatud id numbriks
    sprintf(a, "DELETE FROM autorid WHERE id = '%d'", autor_id);
    sprintf(c, "DELETE FROM raamatud_has_autorid WHERE autorid_id = '%d'", autor_id);
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
	char m[100];
    printf("Kas soovite kustutada veel ühe autori?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab sisestatud vastuse numbriks

    }

    valimine(con, res, row, tegevus);//tagasi peamenüüsse

}

void zanri_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int zanr_id;
    char a[100];
    char c[100];
    char b[10];
    char f[100];
	int k = 0;
    while (k == 0) { //loop mitme žanri kustutamiseks
    printf("Sisestage zanri id: \n");
    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    zanr_id = atoi(b); //muudab sisestatud id numbriks
    sprintf(f, "SELECT * FROM sanrid WHERE id = '%d'", zanr_id); //lisab kokku MySQLi käsu sisestatud id-ga žanri valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, f)) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }
    res = mysql_use_result(con);//salvestab saadud tulemused
    if(res == NULL){
        printf("Žanri pole andmebaasis!\n");
        zanri_kustutamine(con, res, row, tegevus);
    }
    mysql_free_result(res);//vabastab tulemuse muutuja
    sprintf(a, "DELETE FROM sanrid WHERE id = '%d'", zanr_id);//lisab kokku MySQLi käsu sisestatud id-ga žanri kustutamiseks ja kasutaja sisestatud vastuse
    sprintf(c, "DELETE FROM raamatud_has_sanrid WHERE sanrid_id = '%d'", zanr_id);//lisab kokku MySQLi käsu kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
	char m[100];
    printf("Kas soovite kustutada veel ühe žanri?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m); //muudab vastuse numbriks

    }


    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}

void kohaviida_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int kohaviit_id;
    char a[100];
    char b[10];
    char c[100];
	int k = 0;
    while (k == 0) {//loop mitme kohaviida kustutamiseks
    printf("Sisestage kohaviida id, mille soovite kustutada: \n");
    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    kohaviit_id = atoi(b); //muudab sisestatud id numbriks
    char d[100];
    sprintf(d, "SELECT * FROM kohaviit WHERE id = '%d'", kohaviit_id);//lisab kokku MySQLi käsu sisestatud id-ga kohaviida valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, d)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con); //salvestab tulemused
    if(res == NULL){ //kontrollib kas saadi mingi vastus
        printf("Kohaviita pole andmebaasis!\n");//tagasiside
        kohaviida_kustutamine(con, res, row, tegevus);//alusatb uuesti kustutamisega
    }
    mysql_free_result(res);//vabastab tulemuse muutuja
    sprintf(a, "DELETE FROM kohaviit WHERE id = '%d'", kohaviit_id);//lisab kokku MySQLi käsu kohaviida kustutamiseks ja kasutaja sisestatud vastuse
    sprintf(c, "DELETE FROM koopia WHERE kohaviit_id = '%d'", kohaviit_id); //lisab kokku MySQLi käsu sellise kohaviidaga koopiate kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
	char m[100];
    printf("Kas soovite kustutada veel ühe kohaviida?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m); //muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}

void koopia_kustutamine(MYSQL* con, MYSQL_RES* res, MYSQL_ROW row, int tegevus) {
    int koopia_id;
    char a[100];
    char b[10];
	int k = 0;
    while (k == 0) {//loop mitme koopia kustutamiseks
    printf("Sisestage koopia id: \n");
    if (fgets(b, sizeof b, stdin)) {//loeb kasutaja sisestust
        char* newline = strchr(b, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
        if (newline) {
            *newline = 0;//kui reavahetus oli, siis kustutab selle
        }
    }
    koopia_id = atoi(b);//muudab sisestatud id numbriks
    char c[100];
    sprintf(c, "SELECT * FROM koopia WHERE idkoopia = '%d'", koopia_id);//lisab kokku MySQLi käsu sisestatud id-ga koopia valimiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, c)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
    res = mysql_use_result(con); //salvestab saadud tulemused
    if(res == NULL){//kontrollib kas ikka saadi mingi tulemus
        printf("Koopiat pole andmebaasis!\n");//tagasiside
        koopia_kustutamine(con, res, row, tegevus);//alustatakse koopia kustutamisega uuesti
    }
    mysql_free_result(res);//vabastatakse tulemuse muutuja
    sprintf(a, "DELETE FROM koopia WHERE idkoopia = '%d'", koopia_id);//lisab kokku MySQLi käsu sisestatud id-ga koopia kustutamiseks ja kasutaja sisestatud vastuse
    if (mysql_query(con, a)) {//saadab käsu serverile
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);//kui tekib viga, lahkub programmist 
    }
	char m[100];
    printf("Kas soovite kustutada veel ühe koopia?\n");
        printf("-> 0 Jah\n");
        printf("-> 1 Ei\n");
        if (fgets(m, sizeof m, stdin)) {//loeb kasutaja sisestust
            char* newline = strchr(m, '\n');//otsib sisestuse läbi ning vaatab, kas selles on reavahetus 
            if (newline) {
                *newline = 0;//kui reavahetus oli, siis kustutab selle
            }
        }
        k = atoi(m);//muudab vastuse numbriks

    }

    valimine(con, res, row, tegevus);//tagasi peamenüüsse
}
