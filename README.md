Pilotage d'un robot de piscine<br>
-----------------------------------<br>
Lorsqu'un robot de piscine tombe en panne, l'éléctronique de commande et de puissance en est généralement la cause. Or celle-ci est parfois impossible à remplacer voire trop couteuse.<bR>
La solution consiste à supprimer cette dernière est à la remplacer par un autre contrôleur effectant la même tâche.<br>
On utilisera ici un controleur esp8266 (ici un esp01s) ou un contrôleur esp32 pilotant une carte à deux relais placée dans
la partie non immergée du robot.<br>
Cet ensemble remplace l'électronique d'origine des robots (hors alimentation de puissance).
L'électronique de la partie immergée du robot est supprimée et est remplacée par
un pont de diodes.<br>
La partie immergée est alimentée uniquement à l'aide de deux fils
véhiculant un courant dont la polarité s'inverse pour changer le sens de déplacement du 
robot.<br>
Le pont de diode assure la rotation unidirectionnelle de la turbine.<br>
Dans la partie non immergée du robot, seule l'alimention BT est conservée. Si cette dernière fournit une tension trop élevée on ajoutera un convertisseur Buck 20A.<br>
La carte de commande communique via des messages MQTT avec un courtier MQTT public ou privé.<br>
Une application Android permet de paramétrer le cycle de nettoyage ou de commander manuellement
le robot.<br>

Système de commande:<br>
- Carte electronique et convertisseur buck 20A (si necessaire)<br>
https://fr.aliexpress.com/item/1005003493100800.html?spm=a2g0o.cart.0.0.6e4f378dLYCl5I&mp=1&pdp_npi=5%40dis%21EUR%21EUR%204.39%21EUR%204.39%21%21%21%21%21%40211b629217407258366847389e3ec1%2112000026040971361%21ct%21FR%21917560485%21%212%210&gatewayAdapt=glo2fra<br>
https://fr.aliexpress.com/item/1005005876635127.html?spm=a2g0o.order_list.order_list_main.11.4bf35e5bDy7wVE&gatewayAdapt=glo2fra<br>
- Pont de diodes<br>
https://fr.aliexpress.com/item/1005006066715454.html?spm=a2g0o.order_list.order_list_main.5.7deb5e5bFUphXd&gatewayAdapt=glo2fra<br>
<br>  
  
Application Android<br>
-------------------<br>
RobotPiscineFrontEnd<br>
https://github.com/geo17137/RobotPiscineFrontEnd
<br><br>
Plans<br>
-----<br>
Deux versions sont disponibles<br>
- Version avec moteur de traction et turbine fonctionnant sous le même tension<br>
- Version avec moteur de traction et turbine fonctionnant sous deux tensions (tension turbine > tension traction)<br>
  Cette version necessite un relai à double inverseur fonctionnant avec tension bobine = tension turbine<br>
  et un convertisseur buck<br>
  https://fr.aliexpress.com/item/1005005932230887.html?spm=a2g0o.cart.0.0.3c23378ddQLvah&mp=1&pdp_npi=5%40dis%21EUR%21EUR%205.49%21EUR%205.49%21%21%21%21%21%400b830cc417453980619753693e07af%2112000034905435980%21ct%21FR%21917560485%21%211%210&gatewayAdapt=glo2fra<br>

Voir dossier doc
