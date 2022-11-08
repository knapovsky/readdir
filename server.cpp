////////////////////////////////////////////////////////////////////////
// Projekt #2 do predmetu IPK - Client/Server
// soubor: 	server.cpp
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

#include "server.h"
#include <dirent.h>

using namespace std;

/**Vypise napovedu serverove casti projektu
*/
void print_help()
{
    cout << "----------------------------------------------------------------" << endl
	     << "Serverova cast projektu Client/Server BSD Sockets IPK" << endl
		 << "Pouziti: ./server -p cisloPortu [-h]" << endl << endl
		 << "Parametry -p a cisloPortu jsou povinne" << endl
		 <<	"a musi byt zadany v tomto poradi." << endl
		 <<	"Parametr -h je volitelny a slouzi k vypsani" << endl
		 <<	"napovedy programu." << endl
		 << "----------------------------------------------------------------" << endl;

	return;
}

/**Vypise chybovou hlasku
 *@param int err_code - cislo chybove hlasky
 */
void print_error(int err_code)
{
	switch ( err_code )
	{
		case E_ARG: cerr << "ERROR: Wrong parameter!" << endl; break;
		case E_RES: cerr << "ERROR: Wrong destination address!" << endl; break;
		case E_CON:	cerr << "ERROR: Cannot connect to server!" << endl; break;
		case NODIR: cerr << "ERROR: Directory does not exist!" << endl; break;
		case NOTDIR: cerr << "ERROR: Entered path is not a directory!" << endl; break;
		case NOPRIV: cerr << "ERROR: You are not privileged to enter directory!" << endl; break;
		case E_COMM: cerr << "ERROR: Communication error!" << endl; break;
		case E_SOC: cerr << "ERROR: Cannot create socket!" << endl; break;
		case E_SOCN: cerr << "ERROR: Cannot name socket!" << endl; break;
		case E_QUE: cerr << "ERROR: Cannot create listening queue!" << endl; break;
		case E_ACC: cerr << "ERROR: Cannot accept client request!" << endl; break;
		case E_FORK: cerr << "ERROR: Cannot create new process!" << endl; break;
		case EP_CSOC: cerr << "ERROR: Cannot close client socket in process!" << endl; break;
        case E_CSS: cerr << "ERROR: Cannot close client socket in parent!" << endl; break;
        case E_GPATH: cerr << "ERROR: Cannot get path to directory from request!" << endl; break;
        case E_GREQ: cerr << "ERROR: Cannot get request from client!" << endl; break;
        case E_ATC: cerr << "ERROR: Cannot send answer to client!" << endl; break;
        case E_RDRQ: cerr << "ERROR: Cannot read request from client!" << endl; break;
        case E_RLEN: cerr << "ERROR: Request too long, or bad request!" << endl; break;
	}
}

/**Ziska parametry programu a vrati cislo portu,
*  pokud jej mezi paramery nalezne.
*  @param int argc - pocet parametru programu
*  @param char *argv[] - pole s parametry programu
*/
int get_params(int argc, char *argv[])
{
    string temp;
    if(argc == 1) { print_help(); return SUCCESS; }
    else if(argc > 3) { print_error(E_ARG); return E_ARG; }
    else {
        for( int i = 1; i < argc; i++ )
        {
            temp = argv[i];
            if(temp == "-h") { print_help(); return HELP; }
            else if(temp == "-p" && i < argc - 1 ) { return atoi(argv[i+1]); }
            else { print_error(E_ARG); return E_ARG; }
        }
    }

    return E_ARG;
}

/**Ziska pozadavek od klienta
*  @param int client_socket - socket, z ktereho pozadavek ziskavat
*  @param string *request - retezec pro ulozeni pozadavku
*/
int get_request(int client_socket, string *request)
{
    char buffer;
    int size, total_size;
    int end_candidate = 0;
    request->clear();
    int i = 0;

    while(total_size < BUFFERSIZE)
    {
        i++;
        if((size = read(client_socket, &buffer, 1)) < 0)
        {
            print_error( E_RDRQ );
            return E_RDRQ;
        }
        total_size += size;
        request->append(&buffer);
        if(buffer == '\n')
        {
            if( end_candidate == 1 )
            {
                return SUCCESS;
            }
            else
            {
                end_candidate = 1;
            }
        }
        else end_candidate = 0;
    }

    //tady vratit, ze zprava byla moc dlouha
    //print_error( E_RLEN );
    return E_RLEN;
}

/**Ziska absolutni cestu k adresari, jehoz obsah si vyzadal klient
*  @param string *request - retezec obsahuji pozadavek od klienta
*  @param string *path - odkaz na retezec pro ulozeni cesty
*/
int get_path(string *request, string *path)
{
    size_t offset;
    offset = request->find(':');
    if(offset!=string::npos)
    {
      size_t newline;
      newline = request->find('\n', offset);
      if(offset!=string::npos)
      {
        path->assign(*request, offset + 1, newline - offset - 2);
        return SUCCESS;
      }
      else return E_GPATH;
    }
    else return E_GPATH;
}

/**Precte slozku vyzadanou klientem a sestroji odpoved, ktera se klientovi zasle
*  @param string *path - ukazatel na retezec obsahujici cestu k adresari
*  @param string *answer - ukazatel na retezec pro ulozeni odpovedi
*  @param string *err - ukazatel na retezec pro ulozeni chybove hlasky
*/
int build_answer(string *path, string *answer, string *err)
{
    answer->clear();
    DIR *Dir;
    struct dirent *DirEntry;
    //toto tu je kvuli eve..nevim proc ale kazdy lichy znak
    //v path->c_str() mel hodnotu 1 a cesta ke slozce tedy 
    //nebyla spravna
    const char *path_name = path->c_str();
    char re_path[BUFFERSIZE];
    char c = 1;
    int i = 0;
    int j = 0;
    while(c != '\0')
    {
      c = path_name[i];
      i++;
      if( c != 1 )
      {
        re_path[j] = c;
        j++;
      }
    }
    if(( Dir = opendir(re_path) ) == NULL )
    {
        err->append(strerror(errno));
        //tohle by slo udelat jednoduseji
        switch( errno )
        {
            case EACCES: answer->append("EACCES\nERROR:"); return EACCES; break;
            case EBADF:  answer->append("EBADF\nERROR:"); return EBADF; break;
            case EMFILE: answer->append("EMFILE\nERROR:"); return EMFILE; break;
            case ENFILE: answer->append("ENFILE\nERROR:"); return ENFILE; break;
            case ENOENT: answer->append("ENOENT\nERROR:"); return ENOENT; break;
            case ENOMEM: answer->append("ENOMEM\nERROR:"); return ENOMEM; break;
            case ENOTDIR: answer->append("ENOTDIR\nERROR:"); return ENOTDIR; break;
        }
    }
    //slozka otevrena v poradku
    else
    {
        unsigned char type;
        string name;

        answer->append("OK\n\n");
        while((DirEntry = readdir(Dir)) != NULL)
        {
            type = DirEntry->d_type;
            name = DirEntry->d_name;

            if(type == DT_DIR || type == DT_REG || type == DT_LNK)
            {
                if(name != "." && name != "..")
                {
                   answer->append(DirEntry->d_name);
                   answer->append("\n");
                }
            }

        }

        answer->append("\n");
    }
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    int port;
    int server_socket;
    sockaddr_in socket_name;
    sockaddr_in client_info;
    socklen_t addr_length;
    int pid;

    if(( port = get_params(argc, argv) ) == HELP) { return SUCCESS; }
    //spatne zadany port----------------------------------------------------------------
    else if( port == 0 ) { print_error(E_ARG); return E_ARG; }
    else if( port == E_ARG ) { return E_ARG; }
    //else cout << "Port: " << port << endl;
    //vytvorime socket------------------------------------------------------------------
    if(( server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        print_error(E_SOC);
        return E_SOC;
    }
    //naplnime strukturu sockaddr_in----------------------------------------------------
    //rodina protokolu------------------------------------------------------------------
    socket_name.sin_family = AF_INET;
    //cislo port pro naslouchani--------------------------------------------------------
    socket_name.sin_port = htons(port);
    //interface-------------------------------------------------------------------------
    socket_name.sin_addr.s_addr = INADDR_ANY;
    //pojmenovani socketu---------------------------------------------------------------
    if ( bind( server_socket, (sockaddr *)&socket_name, sizeof(socket_name) ) == -1)
    {
        print_error( E_SOCN );
        return E_SOCN;
    }

    //fronta pro 10 pozadavku-----------------------------------------------------------
    if (listen(server_socket, 10) == -1)
    {
        print_error( E_QUE );
        return E_QUE;
    }
    
    //ukonceni bezecich potomku, pokud se ukonci server
    signal(SIGCHLD, SIG_IGN);

    //prijem pozadavku od klienta-------------------------------------------------------
    do
    {
        addr_length = sizeof(client_info);
        int client;

        //vyber pozadavku na spojeni z fronty-------------------------------------------
        if((client = accept(server_socket, (sockaddr*)&client_info, &addr_length)) < 0)
        {
            print_error( E_ACC );
            return E_ACC;
        }
        //vytvoreni noveho procesu------------------------------------------------------
        if((pid = fork()) < 0)
        {
            print_error( E_FORK );
            return E_FORK;
            kill(0, SIGTERM);
        }
        else if( pid == 0 )
        {
            string path;    //retezec pro ulozeni absolutni cesty adresare v soub. systemu
            string request;
            string answer;
            string error;
            int rdd;

            //ziskani requestu od klienta
            if((rdd = get_request(client, &request)) != SUCCESS )
            {
                //moc dlouhy request, ktery nelze precist cely...je potreba zvetsit
                //BUFFERSIZE
                if(rdd == E_RLEN)
                {
                  answer.append("REQERR\nERROR:Request too long, or bad request!\n\n");
                  if( write( client, answer.c_str(), answer.length() ) < 0 )
                  {
                    if(close(client) < 0)
                    { 
                      print_error(EP_CSOC);
                      exit( EP_CSOC );
                    }   
                    print_error( E_ATC );
                    exit( E_ATC );
                  }
                }
                else{ 
                  answer.append("CANTGETANSWER\nERROR:Cannot get a request!\n\n"); 
                  if( write( client, answer.c_str(), answer.length() ) <0  )
                  {
                    if(close(client) < 0)
                    {
                      print_error(EP_CSOC);
                      exit( EP_CSOC );
                    }
                    print_error( E_ATC );
                    exit( E_ATC );
                  }
                }
                if(close(client) < 0)
                {
                  print_error(EP_CSOC);
                  exit( EP_CSOC );
                }
                print_error( rdd );
                exit( rdd );
            }
            //ziskani cesty z pozadavku klienta
            if(( rdd = get_path(&request, &path)) != SUCCESS )
            {
                if(close(client) < 0)
                {
                  print_error(EP_CSOC);
                  exit( EP_CSOC );
                }
                print_error( E_GPATH );
                exit( E_GPATH );
            }
            //precteni obsahu adresare daneho cestou zaslonou klientem
            //neco jako readdir a sestaveni odpovedi pro klienta
            if((rdd = build_answer(&path, &answer, &error)) != SUCCESS)
            {
                answer.append(error);
                answer.append("\n\n");
                //cout << "zasilani odpovedi: " << answer << endl;
                if( write( client, answer.c_str(), answer.length() ) < 0 )
                {
                    if(close(client) < 0)
                    {
                      print_error(EP_CSOC);
                      exit( EP_CSOC );
                    }
                    print_error( E_ATC );
                    exit( E_ATC );
                }
                //cerr << "ERROR: " << error << "\n";
                if(close(client) < 0)
                {
                  print_error(EP_CSOC);
                  exit( EP_CSOC );
                }
                exit( rdd );
            }
            //zaslani odpovedi klientovi
            if( write( client, answer.c_str(), answer.length() ) < 0 )
            {
                print_error( E_ATC );
                if(close(client) < 0)
                {
                  print_error(EP_CSOC);
                  exit( EP_CSOC );
                }
                exit( E_ATC );
            }
            //zpracovani pozadavku
            if(close(client) < 0)
            {
                print_error(EP_CSOC);
                exit( EP_CSOC );
            }
            //pozadavek vyrizen, ukonceni procesu
            exit(SUCCESS);
        }

        //uzavreni socketu v rodici
        if(close(client) < 0)
        {
            print_error( E_CSOC );
            return E_CSOC;
        }
    } while(1);


    if(close(server_socket) < 0)
    {
        print_error( E_CSS );
        return E_CSS;
    }

    return SUCCESS;
}
