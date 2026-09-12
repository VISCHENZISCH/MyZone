<div align="center">

<img src="./myzone-logo.svg" width="90" height="90" alt="MyZone logo" />

# MyZone

**Identification locale d'appareils réseau par adresse MAC, DHCP, TCP et nom d'hôte**

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.14+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org)
[![Interface](https://img.shields.io/badge/interface-CLI%20hacker%20%7C%20JSON-00B8D9?style=for-the-badge)](#interface)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=for-the-badge&logo=linux&logoColor=white)](#)

</div>

---

**MyZone** est un outil d'audit et d'identification réseau locale écrit en C++17. Il combine plusieurs sources et signatures (IEEE OUI, Wireshark, Nmap, KYD/FingerBank, p0f) pour identifier un constructeur, une catégorie d'équipement, un modèle ou un système d'exploitation à partir d'une adresse MAC, d'un nom d'hôte, d'une empreinte DHCP ou d'une signature passive TCP SYN.

L'outil fonctionne **100 % en local et passivement** : il ne scanne pas le réseau et n'émet aucun paquet.

```text
  __  __       ____                  
 |  \/  |_   _|__  /___  _ __   ___   v1.0
 | |\/| | | | | / // _ \| '_ \ / _ \ 
 | |  | | |_| |/ /| (_) | | | |  __/  local network discovery & device auditor
 |_|  |_|\__, /____\___/|_| |_|\___|  https://github.com/VISCHENZISCH/MyZone
         |___/                       
=============================================================================
```

## Fonctionnalités

- **Identification multi-modules** :
  - `OUI` : Fabricant et catégorie par préfixe MAC (`AA:BB:CC:DD:EE:FF`, `AA-BB-CC-DD-EE-FF` ou compact).
  - `DHCP` : Modèle d'appareil et OS par empreinte MD5 (base KYD / FingerBank).
  - `Hostname` : Détection heuristique d'OS et de gammes (Windows, Android, iPhone, iPad, MacBook, etc.).
  - `p0f TCP` : Détection passive du système d'exploitation par signature SYN TCP/IP.
  - `FingerBank` : Identification via la séquence des options DHCP demandées.
- **Indexation unifiée sans doublon** : référentiel prioritaire MyZone complété par Wireshark et Nmap pour les préfixes absents.
- **Mode direct en ligne de commande (CLI)** avec sortie JSON scriptable (`--json`), pipable directement dans `jq`.
- **Suite de tests unitaires intégrée** avec prise en charge native de `ctest`.

## Lancer le projet

### Avec CMake (Recommandé sur Linux, Windows & macOS)

```bash
# Configuration et compilation
mkdir -p build && cd build
cmake ..
cmake --build .

# Exécution des tests unitaires
ctest --output-on-failure

# Lancer l'application
./MyZone
```

### Avec GCC / Clang

```bash
g++ -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -Iinclude \
  main.cpp src/*.cpp src/modules/*.cpp \
  -o MyZone

./MyZone
```

Pour désactiver les couleurs ANSI dans un terminal standard :
```bash
NO_COLOR=1 ./MyZone
```

### Avec Code::Blocks

1. Ouvrir le fichier projet `MyZone.cbp`.
2. Choisir **Build → Build and run** (`F9`).
3. L'application résout automatiquement le dossier `data/`, y compris depuis `bin/Debug` ou `bin/Release`.

## Utilisation en Ligne de Commande (CLI)

En plus de son interface interactive, MyZone peut être invoqué directement pour des scripts d'automatisation ou des pipelines :

```bash
# Identifier une adresse MAC
./MyZone --mac B8:27:EB:12:34:56

# Combiner MAC et nom d'hôte
./MyZone --mac AC:DE:48:00:11:22 --hostname "iPhone-de-Tom"

# Sortie JSON pure pour traitement avec jq
./MyZone --mac B8:27:EB:12:34:56 --json | jq .

# Rechercher une empreinte DHCP
./MyZone --dhcp "0123456789abcdef0123456789abcdef"

# Afficher l'aide
./MyZone --help
```

### Options disponibles

| Option | Argument | Description |
|---|---|---|
| `--mac` | `<address>` | Adresse MAC de l'appareil (avec ou sans séparateurs) |
| `--hostname` | `<name>` | Nom d'hôte réseau (mDNS, NetBIOS, DHCP) |
| `--dhcp` | `<hash>` | Hash MD5 d'empreinte DHCP (32 caractères hexadécimaux) |
| `--tcp` | `<sig>` | Signature passive TCP SYN (format p0f) |
| `--dhcp-options`| `<list>` | Liste d'options DHCP (ex: `1,15,3,6,44`) |
| `--json` | *(aucun)* | Sortie JSON pure sur `stdout` sans bannière |
| `--help`, `-h` | *(aucun)* | Affichage immédiat de l'aide |
| `--version`, `-v`| *(aucun)* | Affichage de la version |

## Interface Interactive

Au démarrage interactif, MyZone présente un tableau de bord clair :

```text
[+] Options disponibles :

  [1] Identifier un appareil (MAC, DHCP, hostname, TCP)
  [2] Rechercher une empreinte DHCP connue
  [3] Voir le catalogue des sources et signatures
  [4] Lancer une demonstration avec des cibles d'exemple
  [5] Afficher l'aide et les limites d'identification
  [0] Quitter

[?] Choix : 
```

| Option | Action |
|---:|---|
| `[1]` | Identification d'un appareil par saisie libre des indices disponibles |
| `[2]` | Consultation de la base des empreintes DHCP (FingerBank/KYD) |
| `[3]` | Affichage du catalogue des bases locales (état, entrées, statut) |
| `[4]` | Démonstration sur 10 cibles d'exemple sous forme de tableau |
| `[5]` | Consultation de l'aide et des limites d'audit passif |
| `[0]` | Quitter la session |

## Sources de données

Toutes les bases locales sont centralisées dans le dossier `data/` :

| Fichier | Statut | Description |
|---|---|---|
| `lookup.csv` | **Indexé** | Référentiel OUI principal et catégorisation d'appareils |
| `manuf` | **Indexé** | Complément Wireshark récent pour fabricants OUI |
| `wireshark-manuf.txt` | **Indexé** | Complément Wireshark historique |
| `nmap-mac-prefixes.txt`| **Indexé** | Complément Nmap pour fabricants OUI |
| `kyd-dhcp-db.txt` | **Indexé** | Base d'empreintes DHCP KYD / FingerBank |
| `p0f.fp` | **Indexé** | Signatures passives TCP SYN p0f v3 |
| `dhcp_fingerprints.conf`| **Indexé** | Règles FingerBank basées sur la suite d'options DHCP |
| `nmap-os-db.txt` | *Disponible* | Signatures OS pour futur fingerprinting actif |
| `nmap-service-probes.txt`| *Disponible* | Probes et signatures de services pour scan futur |

Les licences et attributions des bases sont récapitulées dans [data/README_DATABASES.md](data/README_DATABASES.md).

## Structure du Projet

```text
MyZone/
├── CMakeLists.txt         # Configuration de build et intégration ctest
├── MyZone.cbp             # Projet Code::Blocks (C++17)
├── README.md              # Documentation du projet
├── myzone-logo.svg        # Logo vectoriel
├── data/                  # Référentiels et signatures locales
├── include/               # En-têtes C++
│   ├── Database.hpp
│   ├── DeviceCategory.hpp
│   ├── DeviceProfile.hpp
│   ├── IdentificationEngine.hpp
│   ├── IdentificationModule.hpp
│   ├── MacAddress.hpp
│   ├── UI.hpp
│   └── modules/           # En-têtes des modules d'identification
├── src/                   # Implémentation C++
│   ├── Database.cpp
│   ├── DeviceCategory.cpp
│   ├── DeviceProfile.cpp
│   ├── IdentificationEngine.cpp
│   ├── MacAddress.cpp
│   ├── UI.cpp             # Rendu console hacker wifite2
│   └── modules/           # Logique des modules (OUI, DHCP, Hostname, p0f, FingerBank)
└── test/                  # Suite de tests unitaires (132+ assertions)
```

## Limites et Évolution

- **Adresses MAC randomisées** : les appareils récents (iOS, Android, Windows 11) activent par défaut le masquage d'adresse MAC en Wi-Fi privé. Dans ce cas, le préfixe n'est pas attribué à un constructeur public.
- **Caractère passif** : MyZone n'émet aucun paquet et respecte scrupuleusement la confidentialité de l'environnement d'exécution.
- **Prochaine étape** : une future version pourra intégrer l'écoute passive de paquets ARP/DHCP via `libpcap` ou la lecture consultative du cache ARP local (`/proc/net/arp`) avec les autorisations système adéquates.
