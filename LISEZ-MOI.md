# Télégraphe sans fil

Ce projet est réalisé dans le cadre de l'UE Communication Sans Fil de la première année de Licence Sciences et Technologies.

Dans le cours « communication sans fil », la première chose qui nous est venue à l’esprit, est la conception d’un appareil permettant de communiquer à distance. Initialement, nous voulions réaliser un « talkie-walkie », mais les exigences techniques et matériel de cet outil ne nous auraient pas permis de terminer dans les temps.

Ayant revu nos ambitions à la baisse, nous avons pris la décision de fabriquer un Télégraphe sans fil.

# Description du projet

Le Télégraphe sans fil, apparu à la fin du 19ème siècle, est l’outil pionnier des communications sans fil. Il permet d’envoyer des messages à distance, codés en Morse, via des ondes radios. Le TSF (télégraphe sans fil) a été largement utilisé jusqu’à la moitié du 20ème siècle. Sa simplicité de conception et d’utilisation en ont fait l’un des acteurs majeurs des communications durant les deux Guerres Mondiales.

Étant novices en télécommunications, et n’ayant jamais réalisé d’appareils électriques, le TSF cochait toutes les cases d’un projet qui serait à la fois passionnant à réaliser, et accessible dans sa conception.

Ce projet va donc nous permettre de fabriquer notre propre TSF, et d’envoyer des messages codés en Morse, puis retranscrits en caractères alphanumériques par notre code Arduino et inversement.

# Liste des composants et logiciels

-	Carte Arduino
-	Buzzer 
-	The Things Network (TTN)
-	Node Red
-	Arduino

# Etapes de réalisation

Le projet va se faire en 2 étapes :

-	1ère étape : Arduino récepteur

Coder en local c’est-à-dire que le message taper en morse sur la carte sera réceptionner avec Arduino puis décoder en caractères alphanumériques. 

-	2ème étape : Arduino émetteur

Coder pour que Arduino puisse renvoyer le message décodé. 
Il va falloir connecter Arduino à TTN, ce qui va permettre de recueillir les données à partir de la carte et de les renvoyer sur un autre machine grâce à Node Red .
Node Red permet de réceptionner ce qui est envoyé par TTN et permettra finalement d’envoyer le message par mail par exemple. 

# Conclusion

À ce jour, nous avons réussi à traduire un code Morse en caractères alphanumériques, et à faire apparaître le message à l’écran de notre ordinateur. 
Prochainement, nous devrons déployer un système permettant d’envoyer les messages décodés, à une autre machine, en utilisant TTN et Node Red.
Une fois cette étape réalisée, nous testerons la possibilité d’effectuer l’opération inverse, c’est-à-dire, écrire un message en caractères alphanumériques sur le clavier de l’ordinateur émetteur, qui sera encodé en morse par Arduino, puis envoyé sur une autre machine réceptrice, où le message codé sera pris en charge par l’audio de la machine.
Nous pourrons ainsi nous mettre dans la peau de réels opérateurs télégraphes ! Et qui sait, peut-être auront nous un jour besoin de communiquer en Morse, si les tensions actuelles menaçant la paix dans le monde venaient à s’aggraver…
