# Progetto di Sistemi Operativi

A cura di Lorenzo Franceschelli e Gabriele Monti

![cinema](https://github.com/Lorenx03/Sistema-di-prenotazione-posti/assets/48184667/be6e5aa3-5400-497f-b970-6e26e4c9c9e4)

---

# Sistema di prenotazione posti

Benvenuti nel sistema di prenotazione posti per una sala cinematografica. 

Questo progetto offre una soluzione moderna ed efficiente per gestire le prenotazioni di biglietti, garantendo un'esperienza intuitiva e completa sia per gli utenti che per gli amministratori.

## Caratteristiche Principali

### 1. Visualizzazione della Programmazione delle Sale

Gli utenti possono accedere alla lista aggiornata dei film in programmazione, completa delle seguenti informazioni:

- Titolo del film

- Durata

- Genere

- Cast

- Trama

- Orari di proiezione

La programmazione viene gestita dinamicamente tramite file CSV, garantendo flessibilità e aggiornamenti in tempo reale.

### 2. Prenotazione di Biglietti

Il sistema consente agli utenti di:

- Prenotare fino a un massimo di quattro biglietti per transazione.

- Consultare una mappa visiva dei posti disponibili nella sala cinematografica.

- Selezionare manualmente i posti desiderati.

Alla conferma della prenotazione, il sistema genera un codice univoco che serve per la gestione futura.

### 3. Cancellazione di Prenotazioni

Gli utenti possono annullare una prenotazione esistente inserendo il codice univoco generato in fase di conferma. Questa funzionalità permette una gestione flessibile e semplificata delle prenotazioni.

### 4. Gestione Efficiente delle Risorse

Il sistema garantisce una chiusura ordinata:

- Liberazione della memoria dinamica allocata al termine dell'esecuzione.

- Salvataggio automatico dei dati, assicurando che le informazioni delle prenotazioni siano sempre sincronizzate.

## Struttura del Sistema

### Server

Un server centrale gestisce:

- La mappa dei posti della sala cinematografica, dove ogni posto è identificato da:

- Numero di fila

- Numero di poltrona

- Stato (prenotato o disponibile)

Le richieste dei client, elaborandole in modo concorrente.

### Client

I client permettono agli utenti di:

- Visualizzare la mappa dei posti disponibili.

- Inviare una richiesta di prenotazione specificando il numero di fila e di poltrona.

- Ricevere conferma della prenotazione e un codice univoco.

- Disdire una prenotazione utilizzando il codice fornito.

### Tecnologie Utilizzate

- Linguaggio di Programmazione: C.

- Gestione dei Dati: File CSV per la programmazione e per le prenotazioni.

- Comunicazione Client-Server: Socket TCP/IP.

---
# Come utilizzare il sistema

## Prerequisiti

Assicurarsi che nel sistema siano installati:

- `make`

- Un compilatore C (come `clang` o `gcc`)

## Istruzioni per la Configurazione

Scaricare il file `.zip` fornito e procedere con l'estrazione dei suoi contenuti in una cartella a piacere.

Accedere alla directory estratta

Utilizzare la vostra `shell` preferita e spostarsi nella directory in cui è stato estratto il file. All'interno della cartella si trova un file `Makefile`.

## Compilare il programma

Per compilare ed eseguire il programma, digitare il comando:

`make`

Dopo aver eseguito il comando, il `Makefile` creerà due oggetti eseguibili:

- `SERVER`: rappresenta la macchina server

- `CLIENT`: rappresenta la macchina client

### Avviare i programmi

Eseguire il server con il comando:

`./server -p <port> -t <numThreads>`

- `<port>`: Porta su cui il server deve ascoltare.

- `<numThreads>`: Numero di thread da utilizzare per gestire le richieste.

#### Client

Eseguire il client con il comando:

`./client -a <ip> -p <port>`

- `<ip>`: Indirizzo IP del server a cui connettersi.

- `<port>`: Porta su cui il server sta ascoltando.

## Esecuzione simultanea di client e server

Se si desidera gestire l’esecuzione di client e server in maniera compatta e simultanea è possibile utilizzare il comando: 
`./rundev.sh` 

Questo sarà possibile a condizione che il software `tmux` sia installato nel sistema, esso consente di visualizzare e gestire più sessioni all'interno di una singola finestra della shell. 

Il file `rundev.sh` è uno script Bash che automatizza la compilazione e l'esecuzione di un progetto di prenotazione di cinema, utilizzando tmux per gestire le sessioni del server e del client. 

## Installazione di tmux

### Su Linux

Per installare `tmux` su Linux, puoi installare tmux utilizzando il gestore di pacchetti della tua distribuzione:
- Ubuntu/Debian: `sudo apt install tmux`
- Arch Linux: `sudo pacman -S tmux`
 
### Su macOS

Su macOS, puoi utilizzare Homebrew:  `brew install tmux`

Con questa procedura l'applicazione sarà pronta per l'uso. 

##  Supporto

Per domande o problemi si consiglia di contattare il responsabile del progetto.


