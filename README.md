# Software Cinema

### Progetto di Sistemi Operativi

A cura di Lorenzo Franceschelli e Gabriele Monti

![cinema](https://github.com/Lorenx03/Sistema-di-prenotazione-posti/assets/48184667/be6e5aa3-5400-497f-b970-6e26e4c9c9e4)

---

## Sistema di prenotazione posti

Questo progetto consiste nella realizzazione di un sistema di prenotazione posti per una sala cinematografica. Un processo su una macchina server gestisce una mappa dei posti della sala, dove ciascun posto è caratterizzato da un numero di fila, un numero di poltrona e un FLAG che indica se il posto è già stato prenotato o meno.

Il server accetta e processa le richieste di prenotazione dei posti da uno o più client, che possono risiedere su macchine diverse. Un client deve fornire all'utente le seguenti funzionalità:

1. Visualizzare la mappa dei posti per individuare quelli ancora disponibili.
2. Inviare al server l'elenco dei posti che si intende prenotare (identificati tramite numero di fila e numero di poltrona).
3. Attendere la conferma di prenotazione e un codice di prenotazione dal server.
4. Disdire una prenotazione utilizzando il codice ricevuto.

Lo studente è tenuto a realizzare sia il client che il server. Il server deve essere in grado di gestire le richieste dei client in modo concorrente.

---

### Come compilare ed eseguire i programmi

Compilazione ed Esecuzione

Prerequisiti

Assicurarsi che nel sistema siano installati:

make

Un compilatore C (come clang o gcc)

Compilazione

Per compilare il progetto, eseguire il comando:

make

Questo produrrà due eseguibili nella root del progetto:

client

server

Esecuzione

- Per il client:

./client -a <ip> -p <port>

Dove si specifica l'indirizzo IP del server e la porta a cui connettersi.

- Per il server:

./server -p <port> -t <numThreads>

Dove si specifica la porta su cui il server deve lavorare e il numero di thread da utilizzare.

Per facilitare lo sviluppo, se si dispone di tmux, è possibile utilizzare lo script bash `rundev.sh` per eseguire client e server affiancati sulla stessa macchina in una sessione tmux.
