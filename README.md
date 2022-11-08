# ReadDIR

Popis protokolu READDIR
Toto je popis protokolu READDIR, který byl navržen speciálně pro projekt #2 do předmětu „Počítačové komunikace a sítě“.
Stručný popis projektu #2
Implementace programu client/server pomocí BSD socketů. Server konkurentně zpracovává požadaveky klienta na čtení složky v souborovém systému a odesílá mu odpovídající odpověď.
Protokol READDIR
Protokol byl inspirován klasickým textovým protokolem HTTP, o jehož možnostech jsme měli možnost se dozvědět v projektu #1. Klient používá jediný typ dotazu, jehož syntaxe je následovná:

```
READDIR\n
DIRECTORY:/path/to/directory\n\n
```

READDIR je typ požadavku, /path/to/directory určuje cestu ke složce, kterou chceme vypsat a \n je znak nového řádku. Pokud je na straně serveru složka dostupná, pak server vrací klientovy odpověď typu

```
OK\n\n
file1\n
file2\n
file3\n\n
```

Klient tuto odpověď zpracuje a vypíše soubory na standartní výstup. Při pokusu o přečtení obsahu složky však může dojít k několika situacím, o kterých musí být klient informován. Odpověd serveru je pak ve tvaru

```
CHYBOVYKOD\n
ERROR:Chybova hlaska\n\n
```

Chybové hlášky jsou řetězce, které server získal pomocí hodnoty errno vrácenou funkcí readdir() pro čtení složky souborového systému. Následují typy chybových kódů a jejich popis, který je převzat z manuálové stránky man 3 opendir.
Dokument

```
EACCES
EMFILE
ENFILE
ENOENT
ENOMEM
ENOTDIR
```

- Permission denied.
- Too many file descriptors in use by process.
- Too many files are currently open in the system. - Directory does not exist, or name is an empty string.
- Insufficient memory to complete the operation.
- name is not a directory.
  Maximální velikost zprávy je nastavena na 1200 bytů. Pokud klient překročí požadavkem tuto velikost, nebo zašle špatný typ požadavku, kterému server nerozumí, pak je mu doručena odpověď s chybovým kódem REQERR a stejnou syntaxí jako v předchozím případě.
