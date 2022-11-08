////////////////////////////////////////////////////////////////////////
// Projekt #2 do predmetu IPK - Client/Server
// soubor: 	client.h
//
// Autor:	Martin Knapovsky
// E-Mail:	xknapo02@stud.vutbr.cz
// Datum:	9.4.2011
//
// Popis:	Toto je klientska cast projektu, ktera odesila serveru
//			dotazy na obsah adresare, jehoz cesta je uvedena
//			jako parametr klienta.
//
// Pouziti:	./client jmenoServeru:port absolutniCestaKAdresari [-h]
//			Parametry jmenoServeru:port a absolutniCestaKAdresari jsou
//			povinne a musi byt zadany v tomto poradi.
//			Parametr -h je volitelny a slouzi k vypsani
//			napovedy programu.
//
////////////////////////////////////////////////////////////////////////

using namespace std;

#ifndef CLIENT_H
#define CLIENT_H

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

#define BUFFERSIZE 12000

//Struktura pro uchovani parametru programu
typedef struct t_param
{
	string 	server;
	int 	port;
	string	path;
} t_param;

/**Vypise napovedu klientske casti projektu
 */
void print_help();

/**Vypise chybovou hlasku
 *@param int err_code - cislo chybove hlasky
 */
void print_error(int);

/**Zpracuje parametry programu
 * @param int argc pocet parametru
 * @param char* argv[] - pole s argumenty
 * @param t_param* params - struktura pro ulozeni parametru
 */
int parse_args(int, char*, t_param*);

/**Sestroji dotaz pro server
*  @param request - string pro ulozeni dotazu
*  @param params - parametry programu
*/
void build_request(t_param*, string*);

/**Vytiskne vysledek cteni slozky na STDOUT
*  @param string *answer - odpoved ze serveru
*/
void print_result(string*);

#endif
