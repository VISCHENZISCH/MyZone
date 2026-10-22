# Changelog

Toutes les évolutions notables de MyZone sont listées ici. Le format suit [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/) et le projet respecte le [versionnage sémantique](https://semver.org/lang/fr/).

## [1.2.0] — 2026-10-22

Version de fiabilité : aucune nouvelle source d'identification, mais une base saine pour la suite.

###  Changements de format (sorties machine)

- JSON : `category` contient désormais un **identifiant stable en anglais** (`Router`, `Mobile`…) ; le libellé traduit passe dans le nouveau champ `categoryLabel`.
- JSON : `confidenceLevel` vaut `high` / `medium` / `low` ; le libellé traduit (« Élevé », « Moyen »…) passe dans `confidenceLabel`.
- CSV : les colonnes `category` et `confidence_level` utilisent ces mêmes identifiants stables.

### Ajouté

- Bibliothèque statique `myzone_core`, séparée du CLI, installable et utilisable via `find_package(MyZone)` puis `MyZone::myzone_core`.
- Règles `cmake --install` : binaire, bibliothèque, en-têtes (`<myzone/...>`), bases dans `share/myzone/data`, licences.
- Recherche des bases via la variable d'environnement `MYZONE_DATA` et dans `<prefix>/share/myzone/data`.
- Option CMake `MYZONE_SANITIZE` (AddressSanitizer + UBSan) et job CI dédié, avec un test d'installation.
- Tests de bout en bout du CLI dans `ctest` (version, codes de sortie, clés JSON stables).
- `THIRD_PARTY_LICENSES.md` : licences des bases de signatures embarquées.

### Modifié

- CLI stricte : une option inconnue, une valeur manquante, une MAC invalide, `--json` combiné avec `--csv`, ou `--json`/`--csv` sans identifiant provoquent un message sur `stderr` et le **code de sortie 2**. Auparavant, ces cas étaient ignorés silencieusement (`--mac zz --json` affichait `{}` avec le code 0).
- Version unique définie dans `project(VERSION)` et injectée dans le code via `Version.hpp` (la bannière affichait 1.0, `--version` 1.1).
- `data/` est copié à chaque build au lieu d'une seule fois à la configuration.
- Avertissements compatibles MSVC (`/W4 /utf-8`).
- `update_signatures.py` : écriture atomique, User-Agent aligné sur la version, code de sortie non nul en cas d'échec.
- README : badge CI réel, exemple JSON et règles de score conformes au comportement réel, performances mesurées.

### Corrigé

- Le test `DeviceProfile_csvExport` échouait sur `main` (le CSV exportait la catégorie traduite).
- Échappement CSV des retours chariot (`\r`).

### Retiré

- `data/nmap-os-db.txt` et `data/nmap-service-probes.txt` (7,9 Mo) : bases de fingerprinting **actif**, utilisées par aucun module et contraires à l'approche passive.

## [1.1.0]

- Détection IEEE 802 (LAA, multicast, broadcast), score de confiance, exports `--json` / `--csv`, modules p0f et FingerBank, CI multi-plateforme.

## [1.0.0]

- Première version : identification par OUI, empreinte DHCP et nom d'hôte, interface console interactive.
