# Bases de données de MyZone

Ce dossier est la source unique de données de l'application. Toutes les bases ci-dessous sont indexées en mémoire au démarrage. Les licences de chaque base sont détaillées dans [THIRD_PARTY_LICENSES.md](../THIRD_PARTY_LICENSES.md).

| Fichier | Rôle | Module |
|---|---|---|
| `lookup.csv` | Fabricant **et catégorie** par préfixe OUI (prioritaire) | `OUI` |
| `manuf` | Fabricant OUI, base Wireshark récente (fallback) | `OUI` |
| `wireshark-manuf.txt` | Fabricant OUI, base Wireshark historique (fallback) | `OUI` |
| `nmap-mac-prefixes.txt` | Fabricant OUI, base Nmap (fallback) | `OUI` |
| `kyd-dhcp-db.txt` | Appareil par hash MD5 d'empreinte DHCP (option 55) | `DHCP` |
| `dhcp_fingerprints.conf` | OS par liste ordonnée d'options DHCP | `FingerBank` |
| `p0f.fp` | OS par signature TCP SYN (section `[tcp:request]`) | `p0f TCP` |

Les bases OUI partagent de nombreuses entrées. MyZone conserve une seule fiche par préfixe MAC, dans l'ordre de chargement : `lookup.csv` (qui porte la catégorie d'appareil), puis Wireshark, puis Nmap. Les préfixes MA-L (/24), MA-M (/28) et MA-S (/36) sont pris en charge.

## Formats

- **manuf / wireshark-manuf.txt** — TSV `préfixe[/masque] <tab> nom court <tab> nom complet`.
- **nmap-mac-prefixes.txt** — `PRÉFIXE Nom du fabricant`.
- **kyd-dhcp-db.txt** — TSV `DHCP_hash  DHCP_FP  FingerBank_Device_name  Score`.
- **dhcp_fingerprints.conf** — sections `[os N]` avec `description=` et un bloc `fingerprints=<<EOT … EOT`.
- **p0f.fp** — format p0f v3 ; seules les signatures `[tcp:request]` sont indexées.

## Mise à jour

```bash
python3 scripts/update_signatures.py
```

Le script actualise `manuf` et `nmap-mac-prefixes.txt` depuis leurs dépôts officiels.

## Bases retirées en v1.2

`nmap-os-db.txt` et `nmap-service-probes.txt` ont été retirées : elles servent au fingerprinting **actif** (envoi de sondes), incompatible avec l'approche passive de MyZone, et n'étaient utilisées par aucun module.
