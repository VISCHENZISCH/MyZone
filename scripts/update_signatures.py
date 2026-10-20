#!/usr/bin/env python3
"""
MyZone Signatures Updater
Télécharge et actualise automatiquement les bases de données locales (IEEE OUI, Wireshark manuf, Nmap).
"""

import os
import sys
import urllib.request

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE_DIR, "data")

SOURCES = {
    "Wireshark Manuf": {
        "url": "https://gitlab.com/wireshark/wireshark/-/raw/master/manuf",
        "destination": os.path.join(DATA_DIR, "manuf"),
    },
    "Nmap MAC Prefixes": {
        "url": "https://raw.githubusercontent.com/nmap/nmap/master/nmap-mac-prefixes",
        "destination": os.path.join(DATA_DIR, "nmap-mac-prefixes.txt"),
    },
}

def update():
    print("=" * 65)
    print("  MyZone - Mise à jour automatique des signatures locales")
    print("=" * 65)
    
    if not os.path.exists(DATA_DIR):
        print(f"[!] Erreur : Le dossier {DATA_DIR} n'existe pas.")
        sys.exit(1)

    for name, info in SOURCES.items():
        print(f"[*] Téléchargement de {name}...")
        try:
            req = urllib.request.Request(
                info["url"],
                headers={"User-Agent": "MyZone-Updater/1.1"}
            )
            with urllib.request.urlopen(req, timeout=30) as response:
                content = response.read()
                with open(info["destination"], "wb") as f:
                    f.write(content)
            print(f"[+] Succès : {os.path.basename(info['destination'])} mis à jour ({len(content):,} octets)")
        except Exception as e:
            print(f"[-] Échec lors de la mise à jour de {name} : {e}")

    print("\n[+] Processus terminé. Vos bases MyZone sont prêtes.")

if __name__ == "__main__":
    update()
