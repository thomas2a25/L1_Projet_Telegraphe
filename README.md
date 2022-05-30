# Télégraphe sans fil

Ce projet est réalisé dans le cadre de l'UE Communication Sans Fil de la première année de Licence Sciences et Technologies.

Dans le cours « communication sans fil », la première chose qui nous est venue à l’esprit, est la conception d’un appareil permettant de communiquer à distance. Initialement, nous voulions réaliser un « talkie-walkie », mais les exigences techniques et matériel de cet outil ne nous auraient pas permis de terminer dans les temps.

Ayant revu nos ambitions à la baisse, nous avons pris la décision de fabriquer un Télégraphe sans fil.

# Description du projet

Le télégraphe sans fil (TSF), apparu à la fin du 19ème siècle, est l’outil pionnier des communications sans fil.
Il permet d’envoyer des messages à distance, codés en Morse, via des ondes radios. Le TSF a été largement utilisé jusqu’à la moitié du 20ème siècle. Sa simplicité de conception et d’utilisation en ont fait l’un des acteurs majeurs des communications durant les deux Guerres Mondiales.

Étant novices en télécommunications, et n’ayant jamais réalisé d’appareils électriques, le TSF cochait toutes les cases d’un projet qui serait à la fois passionnant à réaliser, et accessible dans sa conception.

Ce projet va donc nous permettre de fabriquer notre propre TSF, et d’envoyer des messages codés en Morse, puis retranscrits en caractères alphanumériques par notre code Arduino et inversement. Finalement, nous souhaitons concevoir un télégraphe permettant la communication sans fil complète entre deux cartes RFThings.

# Liste des composants et logiciels

-	2 Cartes RFThings
-	2 buzzers
-	Fils de connexion
-	Logiciel Arduino

# Etapes de réalisation

Le projet va se faire en 2 étapes :

1ère étape : Encodage/décodage ASCII <-> Morse avec configuration buzzers et leds associés

Pour réaliser l'encodage/décodage ASCII <-> Morse, nous allons utiliser les deux boutons de la carte Arduino. Le bouton gauche est programmé pour représenter un point et le bouton droit est programmé pour représenter une barre.

Chaque pression de bouton est associée à un bip sonore du buzzer et aussi à l'allumage des leds horizontales en bas de la carte. Les leds vertes sont associées au bouton gauche (=point) et les leds rouges sont associées au bouton droit (=barre).

On considère 1 unité = 300ms. Nous avons donc défini les intervalles de temps suivants : 

  - Un point = 1 unité
  - Une barre = 3 unités (900ms)
  - Temps entre deux caractères (point et barre) = 1 unité
  - Temps entre deux lettres = 3 unités (900ms)
  - Temps entre deux mots = 7 unités (2100ms)
  - 
![Alphabet morse](https://user-images.githubusercontent.com/103073162/170978607-f6b5ffa1-59f9-4226-8ad4-e3102871feed.jpeg)

2ème étape : Codage pour l'envoi de messages

Pour réaliser l'envoi de messages codés/décodés, nous avons utilisé la technologie LoRa Physical, qui pour nous était la meilleure façon d'obtenir une réelle communication entre deux cartes situées à distance. On estime la portée max à environ 15km. Nous nous sommes inspirés du github de Fabien Ferrero https://github.com/FabienFerrero/UCA_Education_Board/tree/master/Code/LoRa_PHY.

# Fonctionnement

La communication peut se faire :

- soit en Morse vers ASCII (décodage Morse-ASCII)

- soit en ASCII vers Morse : dans ce cas-là, les deux cartes vont réagir par des bips sonores et LEDS (encodage ASCII-Morse)

La réception des messages se fait à la fois sur le moniteur série et grâce aux bips et LEDS, selon si nous souhaitons décoder du Morse comme à l'ancienne, ou se faciliter la tâche avec la traduction apparaissant directement.

Nous avons utilisé deux cartes RFThings qui fonctionnent en émetteur/récepteur à tour de rôle.

# Conclusion

Nous avons la fierté d’avoir mené à terme notre projet en créant un véritable système de communication sans fil en Morse qui rend hommage au télégraphe. 
Nous avons réussi à traduire l'alphabet Morse en ASCII, et à faire apparaître le message avec sa traduction à l’écran de notre ordinateur. Nous avons également réussi à envoyer les messages à une autre machine en utilisant la technologie LoRa Physical.
Notre télégraphe nous permet aussi d'effectuer l’opération inverse, c’est-à-dire, écrire un message en ASCII sur le clavier de l’ordinateur émetteur, qui sera encodé en morse par Arduino, puis envoyé sur une autre machine réceptrice. 
Nous pouvons ainsi nous mettre dans la peau de réels opérateurs télégraphes ! Et qui sait, peut-être aurons-nous un jour besoin de communiquer en Morse, si les tensions actuelles menaçant la paix dans le monde venaient à s’aggraver.
