Pilotage d'un robot de piscine<br>
------------------------------<br>
Ce logiciel est destiné à remplacer le logiciel d'origine
de la plupart de robots de piscine.<br>
Il est implémenté sur une carte esp01s pilotant une carte à deux relais placée dans
la partie non immergée du robot.<br>
Cet ensemble remplace l'électronique d'origine des robots (hors alimentation de puissance).
L'électronique de la partie immergée du robot est supprimée et est remplacée par
un pont de diodes. La partie immergée est alimentée uniquement à l'aide de deux fils
véhiculant un courant dont la polarité s'inverse pour changer le sens de déplacement du 
robot.<br>
Le pont de diode assure la rotation unidirectionnelle de la turbine.<br>
Dans la partie non immergée du robot, seule l'alimention BT est conservée.<br>
La carte de commande communique via des messages MQTT avec un courtier MQTT public ou privé.<br>
Une application Android permet de paramétrer le cycle de nettoyage ou de commander manuellement
le robot.<br>

Système de commande:<br>
- Carte electronique <br>
https://fr.aliexpress.com/item/1005003493100800.html?spm=a2g0o.cart.0.0.6e4f378dLYCl5I&mp=1&pdp_npi=5%40dis%21EUR%21EUR%204.39%21EUR%204.39%21%21%21%21%21%40211b629217407258366847389e3ec1%2112000026040971361%21ct%21FR%21917560485%21%212%210&gatewayAdapt=glo2fra<br>
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
Voir dossier doc
