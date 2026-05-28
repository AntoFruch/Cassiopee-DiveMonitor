# Projet Cassiopee DiveMonitor (2026)

* Membres du Projet : BENOIS Loup, FRUCHET Antonin
* Coordinateur : TAILLANDIER-LOIZE Thierry

## Presentation du projet

DiveMonitor est une application mobile de suivi de plongees developpee avec Qt/QML et C++. L'objectif du projet est de permettre a un plongeur d'importer les donnees de son ordinateur de plongee, de les enregistrer localement, puis de les consulter dans une interface simple avec liste, details et visualisation graphique.

Le projet s'appuie sur `libdivecomputer` pour communiquer avec les ordinateurs de plongee compatibles et parser les donnees recuperees. Les plongees importees sont stockees dans une base SQLite locale afin de pouvoir etre consultees hors connexion. L'application gere notamment les metadonnees de plongee, les points temporels profondeur/temperature, ainsi que les fingerprints permettant d'eviter de reimporter des plongees deja presentes.

## Fonctionnalites principales

* Selection d'un ordinateur de plongee compatible.
* Connexion a un appareil via les modes de transport exposes par `libdivecomputer`.
* Import asynchrone des plongees pour ne pas bloquer l'interface.
* Stockage local des plongees dans une base SQLite.
* Liste des plongees sauvegardees.
* Page de details avec duree, profondeur maximale, profondeur moyenne, temperatures et pression atmospherique.
* Graphique de plongee permettant de visualiser l'evolution des donnees pendant la plongee.

## Repartition du travail

* Interface utilisateur : BENOIS Loup
* Backend, import des donnees, integration `libdivecomputer` et base de donnees : FRUCHET Antonin

### Planning Projet :
![Gantt Projet](./gantt/ganttProjet.png)

### Planning par personne :
![Gantt /pers](./gantt/ganttParPersonne.png)

## Architecture

Le projet est organise autour de deux parties principales :

* `diveMonitorApp/qml/` : interface graphique de l'application, pages, composants et navigation.
* `diveMonitorApp/src/` : logique C++ exposee a QML, import depuis les ordinateurs de plongee, modeles Qt et persistance locale.

Les classes backend principales sont :

* `DiveComputerWrapper` : wrapper autour de `libdivecomputer`, responsable de la detection des appareils, de la connexion, de l'import et du parsing des plongees.
* `DiveDatabase` : couche de persistance SQLite pour enregistrer les plongees, leurs points de mesure et les fingerprints.
* `DiveListModel` et `SampleModel` : modeles Qt utilises par l'interface QML pour afficher les plongees et leurs donnees.
