# Guide de Contribution - MyZone

Merci de votre intérêt pour **MyZone** ! Chaque contribution aide à faire de ce projet la référence absolue en matière d'identification et d'audit passif de périphériques réseau.

---

## Comment Contribuer ?

Vous pouvez contribuer de nombreuses façons :
1. **Ajouter ou corriger des signatures** (OUI IEEE, empreintes DHCP, signatures p0f TCP).
2. **Proposer de nouvelles règles heuristiques** dans `HostnameModule` (ex: objets connectés IoT, domotique, consoles de jeux).
3. **Améliorer les performances** du moteur d'indexation ou de recherche.
4. **Signaler des bugs** ou proposer des fonctionnalités via les *Issues* GitHub.

---

## Environnement de Développement

### Prérequis
- Compilateur compatible **C++17** (GCC 9+, Clang 10+, ou MSVC 2019+)
- **CMake** 3.14 ou supérieur

### Compilation et Tests
```bash
# Configuration et compilation
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Lancement des tests unitaires
ctest --output-on-failure
```

---

## Standards de Code C++

- Norme **C++17** stricte.
- Respect du principe de responsabilité unique (**Clean Architecture** / SOLID).
- Gestion de la mémoire via les pointeurs intelligents (`std::unique_ptr`, `std::shared_ptr`) ou structures RAII — **aucun `new` ou `delete` nu**.
- Méthodes `noexcept` et `const` partout où c'est applicable.
- Toute nouvelle fonctionnalité doit être accompagnée de **tests unitaires** dans le dossier `test/`.

---

## Ajouter une nouvelle signature

### Ajouter un modèle d'appareil dans `HostnameModule`
Les règles heuristiques basées sur les noms d'hôtes réseau se trouvent dans :
`src/modules/HostnameModule.cpp`

### Ajouter une empreinte DHCP
Les empreintes se trouvent dans le fichier texte de référence :
`data/kyd-dhcp-db.txt`

---

## Processus de Pull Request (PR)

1. Forkez le projet.
2. Créez votre branche thématique (`git checkout -b feature/nouveau-module`).
3. Écrivez vos modifications et les tests unitaires associés.
4. Vérifiez que la suite de tests passe (`ctest`).
5. Committez vos modifications avec un message explicite (`git commit -m "feat(module): add IoT SmartPlug detection"`).
6. Poussez votre branche (`git push origin feature/nouveau-module`).
7. Ouvrez une **Pull Request** sur le dépôt officiel.
