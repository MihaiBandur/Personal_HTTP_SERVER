# Server HTTP Custom în C cu Module Dinamice

Acest proiect este un server web HTTP scris de la zero în C pentru Linux. O caracteristică specială a acestui server este că nu servește fișiere statice, ci generează conținut dinamic încărcând module (biblioteci partajate `.so`) la runtime în funcție de cererile clientului.

## Cerințe de sistem
- **Sistem de operare:** Linux (Proiectul folosește intensiv sistemul de fișiere `/proc` și apelul de sistem `sendfile` specific Linux).
- **Compilator:** `gcc` (sau `clang`)
- **Utilitare:** `make`

## Cum se compilează
Deschide un terminal în folderul proiectului și rulează simplu:

make

Această comandă va compila executabilul principal `server` și cele patru module dinamice (`diskfree.so`, `issue.so`, `processes.so`, `time.so`).

Pentru a șterge fișierele compilate și a curăța directorul, poți rula:
make clean

##  Cum se pornește serverul
După compilare, pornește serverul specificând portul dorit (ex: 8080) și activând modul verbose (`-v`) pentru a vedea logurile în terminal:

./server -p 8080 -v

**Alte argumente utile:**
- `-p <port>` : Setează portul pe care ascultă serverul. Dacă nu este specificat, va alege unul aleator.
- `-a <adresa>` : Leagă serverul de o anumită adresă IP (default: toate adresele locale).
- `-m <director>` : Specifică directorul de unde să încarce modulele `.so` (default: directorul curent al executabilului).
- `-v` : Afișează mesaje detaliate (verbose) la conectarea clienților.
- `-h` : Afișează meniul de ajutor.

##  Cum se testează (Module disponibile)
Deschide un browser web sau folosește `curl` și accesează următoarele rute:

1. **Ora curentă:** http://localhost:8080/time
   *(Pagina face refresh automat la fiecare 5 secunde)*

2. **Spațiul liber pe disc:** http://localhost:8080/diskfree
   *(Rulează comanda `df -h` în spate și afișează rezultatul)*

3. **Informații despre sistem:** http://localhost:8080/issue
   *(Afișează conținutul fișierului `/etc/issue`)*

4. **Procesele care rulează:** http://localhost:8080/processes
   *(Citește `/proc` și afișează un tabel detaliat cu PID, Nume, User și Memorie)*

## Structura proiectului
- `server.c` / `server.h` - Logica principală de rețelistică (sockets, acceptare conexiuni HTTP).
- `main.c` - Parsarea argumentelor din linia de comandă.
- `module.c` - Funcțiile de încărcare/descărcare dinamică a fișierelor `.so` folosind `dlopen`/`dlsym`.
- `common.c` - Funcții utilitare (alocare memorie, tratare erori, determinarea căii).
- `*.c` (time, diskfree, issue, processes) - Codul sursă pentru fiecare modul în parte.
