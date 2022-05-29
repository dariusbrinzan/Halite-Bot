[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-f059dc9a6f8d3a56e377f745f24479a46679e63a5d9fe6f495e02850cd0d8118.svg)](https://classroom.github.com/online_ide?assignment_repo_id=7438693&assignment_repo_type=AssignmentRepo)
# Proiect PA 2022

Ultima modificare: 06.03.2022

## Conţinutul repository-ului:

 - starter_packages/    - boţii pe baza cărora puteţi începe să codaţi
 - environment/         - engine-ul jocului
 - bots/                - folder cu botii pusi la dispozitie de echipa de PA

## Regulile jocului

Regulile jocului pot fi gasite [aici](https://2016.halite.io/rules_game.html)
 
## Engine

Pentru a compila engine-ul jocului:

### Linux: 
```
cd environment
make
```

### Windows:

```
cd environment
make.bat
```

## Rularea agentilor

In interiorul starter packages veti gasi un runGame.sh / runGame.bat, cu ajutorul carora puteti rula un meci (nu uitati sa copiati engine-ul jocului).
Mai multe detalii despre engine cli gasiti [aici](https://2016.halite.io/advanced_command_line.html).
La finalul fiecarui meci, engine-ul va genera un replay. Il puteti vizualiza [aici](https://2016.halite.io/local_visualizer.html). In caz ca doriti sa va faceti ceva mai automat, care sa analizeze replay-urile, structura unui replay se gaseste [aici](https://2016.halite.io/advanced_replay_file.html).
