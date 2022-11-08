////////////////////////////////////////////////////////////////////////
// Projekt #2 do predmetu IPK - Client/Server
// soubor: 	server.h
//
// Autor:	Martin Knapovsky
// E-Mail:	xknapo02@stud.vutbr.cz
// Datum:	9.4.2011
//
// Popis:	Toto je serverova cast, ktera zpracovava dotazy klientu
//          konkurentnim zpusobem. Dotazy od klientu obsahuji pozadavek
//          na vypis daneho adresare, ktery server resp. jeho proces
//          precte a zasle klientovi odpoved, jejiz typ je urcen
//          uspesnosti/neuspesnosti cteni slozky.
//
// Pouziti:	./server -p cisloPortu [-h]
//          Parametry -p a cislo portu jsou povinne a udavaji cislo portu,
//          na kterem ma server naslouchat dotazum klientu.
//          Parametr [-h] pak vytiskne napovedu na STDOUT.
//
////////////////////////////////////////////////////////////////////////

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <signal.h>
#include <dirent.h>
#include <errno.h>

#define END    -1
#define SUCCESS 0
#define E_ARG   1
#define E_CON   2
#define NODIR   3
#define NOTDIR  4
#define NOPRIV  5
#define E_COMM  6
#define E_RES   7
#define E_SOC   8
#define HELP    9
#define E_SOCN 10
#define E_ACC  11
#define E_FORK 12
#define EP_CSOC 13
#define E_CSOC 14
#define E_GPATH 15
#define E_GREQ  16
#define E_ATC   17
#define E_RDD   18
#define E_QUE   19
#define E_CSS   20
#define E_RDRQ  21
#define E_RLEN  22

#define BUFFERSIZE 12000

using namespace std;

/**Vypise napovedu serverove casti projektu
 */
void print_help();

/**Vypise chybovou hlasku
 *@param int err_code - cislo chybove hlasky
 */
void print_error(int);

/**Ziska parametry programu a vrati cislo portu,
*  pokud jej mezi paramery nalezne.
*  @param int argc - pocet parametru programu
*  @param char *argv[] - pole s parametry programu
*/
int get_params(int, char*);

/**Ziska pozadavek od klienta
*  @param int client_socket - socket, z ktereho pozadavek ziskavat
*  @param string *request - retezec pro ulozeni pozadavku
*/
int get_request(int, string*);

/**Ziska absolutni cestu k adresari, jehoz obsah si vyzadal klient
*  @param string *request - retezec obsahuji pozadavek od klienta
*  @param string *path - odkaz na retezec pro ulozeni cesty
*/
int get_path(string*, string*);

/**Precte slozku vyzadanou klientem a sestroji odpoved, ktera se klientovi zasle
*  @param string *path - ukazatel na retezec obsahujici cestu k adresari
*  @param string *answer - ukazatel na retezec pro ulozeni odpovedi
*  @param string *err - ukazatel na retezec pro ulozeni chybove hlasky
*/
int build_answer(string*, string*, string*);

#endif // SERVER_H_INCLUDED
