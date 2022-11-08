////////////////////////////////////////////////////////////////////////
  // Projekt #2 do predmetu IPK - Client/Server
  // soubor: 	client.cpp
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

#include "client.h"

  using namespace std;

  /**Vypise napovedu klientske casti projektu
   */
  void print_help()
  {
      cout << "----------------------------------------------------------------" << endl
         << "Klientska cast projektu Client/Server BSD Sockets IPK" << endl
       << "Pouziti: ./client jmenoServeru:port absolutniCestaKAdresari [-h]" << endl << endl
       << "Parametry jmenoServeru:port a absolutniCestaKAdresari jsou" << endl
       <<	"povinne a musi byt zadany v tomto poradi." << endl
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
    switch ( err_code ){
      case E_ARG: cerr << "ERROR: Wrong parameter!" << endl; break;
      case E_RES: cerr << "ERROR: Wrong destination address!" << endl; break;
      case E_CON:	cerr << "ERROR: Cannot connect to server!" << endl; break;
      case NODIR: cerr << "ERROR: Directory does not exist!" << endl; break;
      case NOTDIR: cerr << "ERROR: Entered path is not a directory!" << endl; break;
      case NOPRIV: cerr << "ERROR: You are not privileged to enter directory!" << endl; break;
      case E_COMM: cerr << "ERROR: Communication error!" << endl; break;
      case E_SOC: cerr << "ERROR: Cannot create socket!" << endl; break;
    }
  }

  /**Zpracuje parametry programu
   * @param int argc pocet parametru
   * @param char* argv[] - pole s argumenty
   * @param t_param* params - struktura pro ulozeni parametru
   */
  int parse_args(int argc, char *argv[], t_param *params)
  {
    bool address = false;
    bool path = false;
    string temp;
    string temp2;
    size_t offset = 0;

    if(argc == 1)
    {
      print_help();
      return END;
    }
    else if(argc > 4)
    {
      print_error( E_ARG );
      return END;
    }
    else
    {
      //prochazeni polem parametru
      for(int i = 1; i < argc; i++)
      {
        temp = argv[i];
        //zadano -h - tisk napovedy
        if(temp == "-h"){ print_help(); return END; }
        //zadany parametr je cestou
        else if((temp.c_str())[0] == '/') { params->path = temp; path = true; }
        else
        {
          offset = temp.find(':');
          if(offset!=string::npos)
          {
            params->server.assign(temp, 0, offset);
            temp2.assign(temp, offset + 1, temp.length() - offset);
            params->port = atoi(temp2.c_str());
            address = true;
          }
          else { print_error( E_ARG ); return END; }
        }

      }
    }
    if(address == true && path == true && params->port != 0) return SUCCESS;
    else {
      print_error( E_ARG );
      return END;
    }
  }

  /**Sestroji dotaz pro server
  *  @param request - string pro ulozeni dotazu
  *  @param params - parametry programu
  */
  void build_request(t_param *params, string* request)
  {
      request->append("READDIR\n");
      request->append("DIRECTORY:");
      request->append(params->path);
      request->append("\n\n");

      return;
  }

  /**Vytiskne vysledek cteni slozky na STDOUT
  *  @param string *answer - odpoved ze serveru
  */
  void print_result(string *answer)
  {
      size_t offset = 0;
      string temp;

      offset = answer->find('\n');
      if(offset!=string::npos)
      {
          temp.assign(*answer, 0, offset);
      }
      if(temp == "OK")
      {
          temp.clear();
          temp.assign(*answer, offset + 2, answer->length() - offset - 3);
      }
      else
      {
          temp.clear();
          temp.assign(*answer, offset + 1, answer->length() - offset - 2);
      }

      cout << temp;
  }

  int main(int argc, char *argv[])
  {
      hostent *host;
      string request, answer;
    t_param params;
    char buffer[BUFFERSIZE];
    int client_socket;
    sockaddr_in server_socket;
    int size, total_size = 0;
    //int answer_code;

    //nacteni parametru programu do struktury t_param--------------------
    if( parse_args(argc, argv, &params) < 0)
    {
        return E_ARG;
    }

      //vytvoreni dotazu podle parametru programu--------------------------
      build_request(&params, &request);
      //resolving----------------------------------------------------------
      if((host = gethostbyname(params.server.c_str())) == NULL)
    {
      print_error(E_RES);
      return E_RES;
    }
    //------------------------------------------------------------------
    //Vytvoreni socketu-------------------------------------------------
    if((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
      print_error( E_SOC );
      return 	E_SOC;
    }
    //------------------------------------------------------------------
    //Naplneni socaddr_in-----------------------------------------------
    server_socket.sin_family = AF_INET;		//rodina protokolu
    server_socket.sin_port = htons(params.port);	//cislo portu, ke kteremu se pripojime
    ////nastaveni IP adresy, ke ktere se pripojime
    memcpy(&(server_socket.sin_addr), host->h_addr, host->h_length);
    /*------------------------------------------------------------------
     * Pripojeni socketu
     */
    if(connect(client_socket, (sockaddr *)&server_socket, sizeof(server_socket)) == -1)
    {
      print_error( E_CON );
      return E_CON;
    }
    //------------------------------------------------------------------
    /*------------------------------------------------------------------
     * Odesilani dat
     */
      //cout << "posilam request: \n" << request << endl;
    if((size = send(client_socket, request.c_str(), request.size() + 1, 0)) == -1)
    {
      print_error( E_COMM );
		return E_COMM;
	}
	//cout << "Data odeslana" << endl;
	//------------------------------------------------------------------
	/*------------------------------------------------------------------
	 * Prijimani dat a ukladani do bufferu
	 */
	while(((size = recv(client_socket, buffer, BUFFERSIZE - 1, 0)) != -1) && (size != 0))
	{
		buffer[size] = '\0';
		total_size += size;
		answer += buffer;
	}
	//------------------------------------------------------------------
	close(client_socket);

    print_result(&answer);

	return SUCCESS;
}
