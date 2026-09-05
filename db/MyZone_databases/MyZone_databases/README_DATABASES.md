# Bases de données téléchargées pour MyZone

Ce dossier regroupe les bases identifiées lors de la recherche sur le fingerprinting réseau, en complément de ta base OUI actuelle (`master_oui.csv` / `oui_lookup.csv`).

## 1. wireshark-manuf.txt (946 Ko, 17 411 lignes)
**Source :** mirroir GitHub officiel de Wireshark
**Usage :** lookup MAC → fabricant, alternative/complément à ta base OUI actuelle. Format simple type `ethers(4)` :
```
00:00:01	Xerox                  # XEROX CORPORATION
```
Gère aussi des masques réseau (ex: `00:50:C2:A1:10:00/36`) pour des plages plus précises qu'un simple OUI /24.

## 2. nmap-mac-prefixes.txt (1.4 Mo, 52 091 lignes)
**Source :** dépôt officiel Nmap (github.com/nmap/nmap)
**Usage :** encore une autre base OUI → fabricant, à croiser avec les deux précédentes en cas d'absence de correspondance.

## 3. nmap-os-db.txt (5.2 Mo, 116 271 lignes)
**Source :** dépôt officiel Nmap
**Usage :** fingerprinting **actif** d'OS/appareil via l'envoi de paquets TCP/UDP/ICMP spécifiques et l'analyse des réponses. Plus de 2 600 signatures, classées par type d'appareil (routeur, switch, imprimante, console...). Format texte structuré (`Fingerprint`, `Class`, `SEQ`, `OPS`, `WIN`...) — nécessite d'implémenter les probes réseau correspondantes pour en tirer parti (complexe, pour une phase avancée).

## 4. nmap-service-probes.txt (2.5 Mo, 17 167 lignes)
**Source :** dépôt officiel Nmap
**Usage :** identification de services derrière un port ouvert (ex: bannière MQTT, HTTP, telnet...) via des règles de correspondance (regex). Utile en Phase 3 pour affiner IoT (un port 1883 ouvert + bannière MQTT = objet IoT quasi certain).

## 5. kyd-dhcp-db.txt (66 Ko, 592 lignes)
**Source :** github.com/ptimmons/kyd (dérivé de l'API Fingerbank)
**Usage :** fingerprinting **DHCP** — un hash basé sur l'ordre des options demandées par le client DHCP (option 55) identifie souvent l'appareil précisément. Format TSV :
```
DHCP_hash    DHCP_FP    FingerBank_Device_name    Score
```
Nécessite de capturer les requêtes DHCP du réseau (option 55) pour calculer le hash correspondant et faire le lookup.

---

## Recommandation d'intégration dans MyZone

| Base | Complexité d'intégration | Priorité |
|---|---|---|
| wireshark-manuf.txt | Faible (même format de lookup que ta base actuelle) | Haute — à croiser dès maintenant |
| nmap-mac-prefixes.txt | Faible | Moyenne — fallback si les autres bases n'ont pas de match |
| kyd-dhcp-db.txt | Moyenne (nécessite de sniffer les requêtes DHCP) | Moyenne — bon gain de précision pour peu d'effort |
| nmap-service-probes.txt | Moyenne (nécessite scan de ports + parsing regex) | Basse — pour la Phase 3 |
| nmap-os-db.txt | Élevée (nécessite de forger des paquets bruts et interpréter les réponses) | Basse — pour une phase avancée, uniquement si le OS fingerprinting devient un besoin fort |

## Licences à respecter

- **Wireshark manuf** : GPL-2.0-or-later
- **Nmap (os-db, service-probes, mac-prefixes)** : licence Nmap (basée sur GPLv2 mais non compatible)
- **KYD / dhcp-db.txt** : BSD-3-Clause (dépôt), données dérivées de l'Open Database License de Fingerbank

À garder en tête si MyZone est distribué publiquement plus tard (cf. section licences du cahier des charges).
