# Cahier des charges

Ici le cahier des charges pour la réalisation du projet.

Celui-ci pourra être agrémenté au fil du projet si des spécifications supplémentaires semblent devenir nécessaires.

## Contraintes globales

Il s'agit d'une application mobile, en ```C++/Qt```.

## Interface

L'application doit avoir une interface simple d'utilisation, et agréable à utiliser. 

Plusieurs vues sont à mettre en oeuvre : 
* Vue liste des plongée enregistrées
* Vue détail d'une plongée
* _Bonus : Vue paramètres de l'application_

Eventuellement une barre de navigation pour naviguer entre les différentes vues 

### Liste des plongées

C'est une vue simple des plongées enregistrées, il faut que celles-ci soient listées avec une prévisualisation de certaines informations (nom, date, durée, profondeur max, température moyenne, etc)

Chaque bouton représentant une plongée doit pouvoir mener à la vue détail de la plongée associée.

### Détail d'une plongée

C'est la vue d'intérêt de l'application. Elle doit présenter les statistiques détaillées de la plongée selectionnée.

Un graphe ergonomique doit être présent, celui-ci doit montrer l'évolution de plusieurs données en fonction du temps, avec possibilité de zoom, déplacement sur le graphe, curseur pour visualiser les données à un instant T.

Données à afficher dans la vue :
* Nom de la plongée
* _Bonus_ : Description de la plongée_
* Graphe :
    * Profondeur en fonction du temps
    * Profondeur à instant t avec le curseur
    * Température en fonction du temps
    * Température à instant t avec le curseur
    * Pression en fonction du temps
    * Vitesse de remontée/descente en fonction du temps
    * Vitesse de remontée/descente à instant t avec le curseur

### Importer des plongée

Cette vue permettra d'ajouter les nouvelles plongées que l'appareil a enregistré à la liste dejà existante.

Elle comprendra : 
* Trois menus déroulants (_Bonus_: champ textuel à choix réduit comme pour la selection d'une commune par exemple)
    * Menu 1 : Vendeurs (_ex:_ Mares)
    * Menu 2 : Modèle (_ex_: Quad Air)
    * Menu 2 : Mode de communication (_ex_: Serial, Bluetooth, Bluetooth Low Energy...)
* Un bouton pour initier la connection à l'appareil
* Un indicateur du statut de cette connexion
* un bouton "importer" qui importe les nouvelles plongées. Celui-ci ne peut être utilisé que si la connexion est bien faite.

## Back-end

Le back-end doit gérer la récupération et le traitement des données.

Utilisation de ```libdivecomputer``` qui fera office de boîte noire entre l'ordinateur de plongée et les données. La librairie retournera toutes les données.

### Récupération des données

Avec un programme simple, on doit récupérer toutes les plongées enregistrées dans l'appareil, ainsi que les données associées. 

L'utilisation de l'outil est décrite ici : [libdivecomputer.md](/documentation/libdivecomputer.md)

### Wrapper

Le wrapper devra faire interface entre les appels à libDC et l'interface. **L'interface ne doit à aucun moment interagir directement avec libDC**.



Il fournira les méthodes nécessaires pour :
* indiquer quels sont les appareils supportés par l'application 
* établir la connexion à l'appareil de plongée.
* télécharger efficacement les plongées.
* préparer au maximum les données pour faciliter le travail de l'interface.



