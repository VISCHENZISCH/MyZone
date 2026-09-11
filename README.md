<div align="center">

<img src="./myzone-logo.svg" width="90" height="90" alt="MyZone logo" />

# MyZone

**Identification locale d'appareils réseau par adresse MAC et empreinte DHCP**

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![Interface](https://img.shields.io/badge/interface-CLI%20interactive-00B8D9?style=for-the-badge)](#interface)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey?style=for-the-badge&logo=linux&logoColor=white)](#)

</div>

---

MyZone est un outil C++ local : il associe le préfixe d'une adresse MAC (OUI) à un fabricant, affiche la catégorie connue de l'appareil et peut consulter une empreinte DHCP issue de FingerBank/KYD. Il ne scanne pas le réseau et n'envoie aucune donnée.

## Fonctionnalités

- Recherche interactive d'un fabricant à partir d'une MAC, avec saisie `AA:BB:CC:DD:EE:FF`, `AA-BB-CC-DD-EE-FF` ou compacte.
- Catégorisation lorsque l'information est disponible : routeur, smartphone, switch, caméra, IoT, etc.
- Chargement sans doublon des index OUI : le référentiel MyZone prioritaire est complété par Wireshark et Nmap uniquement pour les préfixes absents.
- Recherche d'empreintes DHCP par hash MD5 dans la base KYD / FingerBank.
- Tableau de bord coloré : recherche MAC, recherche DHCP, démonstration, aide et catalogue des sources.

## Lancer le projet

### Avec Code::Blocks

1. Ouvrir `MyZone.cbp`.
2. Choisir **Build → Build and run** (`F9`).
3. L'application localise automatiquement le dossier `data/`, y compris lorsqu'elle est lancée depuis `bin/Debug`.

### Avec GCC

```bash
g++ -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Iinclude \
  main.cpp src/Database.cpp src/MacAddress.cpp src/DeviceCategory.cpp \
  -o MyZone
./MyZone
```

Pour désactiver les couleurs ANSI, définissez `NO_COLOR` :

```bash
NO_COLOR=1 ./MyZone
```

## Interface

Au démarrage, l'écran d'accueil affiche le nombre de préfixes MAC uniques et d'empreintes DHCP indexées. Le tableau de bord propose :

| Option | Action |
|---:|---|
| 1 | Rechercher un appareil par adresse MAC |
| 2 | Rechercher une empreinte DHCP connue |
| 3 | Voir les données chargées et les signatures disponibles |
| 4 | Lancer une démonstration avec des MAC d'exemple |
| 5 | Afficher l'aide et les limites d'identification |
| 0 | Quitter |

Une MAC permet d'identifier un bloc attribué à un fabricant ; elle ne garantit pas, à elle seule, le modèle ou le type exact de l'appareil.

## Sources de données

Les fichiers sont centralisés dans `data/`. Les fichiers utilisés directement par l'interface sont indexés au lancement ; les signatures destinées à un futur moteur de scan restent disponibles et sont signalées dans le catalogue.

| Fichier | Utilisation dans MyZone |
|---|---|
| `lookup.csv` | Référentiel OUI principal et catégories d'appareil |
| `manuf` | Complément Wireshark récent pour les fabricants OUI |
| `wireshark-manuf.txt` | Complément Wireshark historique |
| `nmap-mac-prefixes.txt` | Complément Nmap pour les fabricants OUI |
| `kyd-dhcp-db.txt` | Lookup interactif d'empreintes DHCP |
| `dhcp_fingerprints.conf` | Règles DHCP disponibles pour une future capture réseau |
| `nmap-os-db.txt` | Signatures d'OS pour fingerprinting actif futur |
| `nmap-service-probes.txt` | Signatures de services pour scan futur |
| `p0f.fp` | Signatures TCP/IP pour fingerprinting passif futur |

Les attributions et licences des bases téléchargées sont détaillées dans [data/README_DATABASES.md](data/README_DATABASES.md).

## Structure

```text
MyZone/
├── data/                  # Toutes les bases locales
├── include/               # Interfaces C++
├── src/                   # Chargement des bases et modèles métier
├── main.cpp               # Tableau de bord interactif
├── MyZone.cbp             # Projet Code::Blocks (C++17)
└── myzone-logo.svg
```

## Limites et prochaine étape

Cette version est volontairement locale et consultative. Nmap, p0f et FingerBank fournissent les signatures nécessaires pour une évolution vers la découverte d'appareils, l'analyse de trafic DHCP et le fingerprinting actif/passif. Ajouter ce module impliquera de demander explicitement les permissions réseau nécessaires.
