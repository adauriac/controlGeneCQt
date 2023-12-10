# Généralités
Utilise la librairie *libmodbus* obtenue avec pacman mingw-w64-x86_64-libmodbus-git. 
Pour installer un nouveau repo local:
- un répertoire contient seulement controlGene.pro main.cpp mainwindow.cpp mainwindow.h mainwindow.ui
- ouvrir QTCreator et le projet *controlGene.pro* choisir le kit "mingw64"
- en cliquant sur le fond de controlGene.pro ajouter une library extern et choisir avec le
file selector de windows le fichier libmodbus.dll.a
- ceci a modifié *controlGene.pro* **il faut corriger ce qui a été rajouté** : 
dans l'affectation de LIBS il faut remplacer libmodbus.dll par modbus.dll (car gcc rajoute le lib)
- Il faut alors choisir l'icone "Projects" à gauche puis choisr "run" (flèche verte) et 
ajouter à la variable d'environnement $Path, avec le file selector de windows, le répertoire 
qui a le même début que la lib mais en choisissant mingw64/bin 

# Avertissement
Deux modes entre lesquels on bascule en recompilant avec le flag SIMUL défini ou pas
- normal : électronique connectée
- simulation : les registres de la carte sont simulés avec le vector *m_fake*.

# Principe
1. Affichage du choix des paramètres de la liaison série et attente de la validation du bouton **connexion**
2. Si connexion réussie lecture et affichage des valeurs des registres, sinon fin du programme.
3. L'utilisateur peut renseigner le champ éditable des registres qu'il veut réaffecter. 
4. En cliquant **envoi des valeurs** les valeurs de la colonne éditable sont envoyées à la carte puis 
relues et affichées dans la colonne non éditable. Les champs vides de la colonne éditable 
sont ignorés. Les champs qui contienent des caractères qui ne représentent pas des chiffres sont mos à zero.
Si une lecture ou une écriture n'a pas marché, un popup informe l'utilisateur puis le programme
s'arrête.

# Sous le capot
Il y a un QMap qui est facilement éditable dans mainwindow.h
sous la forme d'un defined si l'on veut rajouter des registres. 

De plus par registre il y a 
- un label texte qui contient nom et adresse dans la carte du registre.
- un label pour afficher la valeur la valeur du registre lue.
- un QLineEdit pour y entrer la valeur à mettre.
- un entier qui est la dernière valeur lue.
- un entier qui est la valeur à mettre.

Toutes ces quantitées sont des vecteurs, respectivement 
- QMap\<QString, unsigned int> m_NamesAdd;
- QVector\<int> m_adds;
- QVector\<int> m_registerVals;
- QVector\<int> m_valuesToSend;
- QVector\<QLabel*> m_labelsValue;
- QVector\<QLabel*> m_labelsName;
- QVector\<QLineEdit*> m_lineEditsValue;

Le programme place les widgets aux endroits choisis (parametres de connexion et valeurs des registres)
et les rend visibles quand il faut.
