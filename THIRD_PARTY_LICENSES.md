# Licences des données tierces

Le **code source** de MyZone est distribué sous licence MIT (voir [LICENSE](LICENSE)).
Les **bases de signatures** du dossier `data/` proviennent de projets tiers et restent soumises à leurs propres licences, listées ci-dessous. Elles ne sont pas couvertes par la licence MIT.

| Fichier | Origine | Licence |
|---|---|---|
| `lookup.csv` | Agrégat IEEE Registration Authority + Wireshark + Nmap | Données publiques IEEE ; parties dérivées soumises aux licences Wireshark et Nmap ci-dessous |
| `manuf`, `wireshark-manuf.txt` | [Wireshark](https://gitlab.com/wireshark/wireshark) | GPL-2.0-or-later |
| `nmap-mac-prefixes.txt` | [Nmap](https://github.com/nmap/nmap) | Nmap Public Source License (NPSL) |
| `kyd-dhcp-db.txt` | [KYD](https://github.com/ptimmons/kyd), dérivé de Fingerbank | Dépôt BSD-3-Clause ; données Fingerbank sous ODbL 1.0 / DbCL 1.0 |
| `dhcp_fingerprints.conf` | [Fingerbank](https://fingerbank.org) / Inverse inc. (PacketFence) | Open Database License 1.0 (ODbL) ; contenus sous DbCL 1.0 |
| `p0f.fp` | [p0f v3](https://lcamtuf.coredump.cx/p0f3/) — Michal Zalewski | GNU LGPL 2.1 |

## Conséquences pratiques

- Redistribuer MyZone **avec** le dossier `data/` implique de respecter les licences ci-dessus (notamment GPL pour Wireshark, NPSL pour Nmap et l'obligation de partage à l'identique de l'ODbL pour Fingerbank).
- Le binaire et la bibliothèque `myzone_core` ne contiennent aucune de ces données : elles sont lues à l'exécution depuis `data/` (ou `share/myzone/data` une fois installé).
- Pour un usage commercial, vérifiez les conditions de la NPSL et de l'ODbL, ou remplacez les bases concernées par vos propres sources.
