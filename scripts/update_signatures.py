#!/usr/bin/env python3
"""
MyZone Signatures Updater
Télécharge et actualise automatiquement les bases de données locales (IEEE OUI, Wireshark manuf, Nmap).
"""

import os
import re
import sys
import tempfile
import urllib.request

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(BASE_DIR, "data")


def project_version():
    """Lit la version depuis CMakeLists.txt (source unique de vérité)."""
    try:
        with open(os.path.join(BASE_DIR, "CMakeLists.txt"), encoding="utf-8") as f:
            match = re.search(r"project\(MyZone VERSION ([0-9.]+)", f.read())
            return match.group(1) if match else "dev"
    except OSError:
        return "dev"


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

    failures = 0
    for name, info in SOURCES.items():
        print(f"[*] Téléchargement de {name}...")
        try:
            req = urllib.request.Request(
                info["url"],
                headers={"User-Agent": f"MyZone-Updater/{project_version()}"}
            )
            with urllib.request.urlopen(req, timeout=30) as response:
                content = response.read()
            if not content.strip():
                raise ValueError("réponse vide")
            # Écriture atomique : un téléchargement interrompu ne corrompt pas la base existante
            fd, tmp_path = tempfile.mkstemp(dir=DATA_DIR, prefix=".update-")
            with os.fdopen(fd, "wb") as f:
                f.write(content)
            os.replace(tmp_path, info["destination"])
            print(f"[+] Succès : {os.path.basename(info['destination'])} mis à jour ({len(content):,} octets)")
        except Exception as e:
            failures += 1
            print(f"[-] Échec lors de la mise à jour de {name} : {e}")

    if failures:
        print(f"\n[-] {failures} source(s) n'ont pas pu être mises à jour.")
        sys.exit(1)
    print("\n[+] Processus terminé. Vos bases MyZone sont prêtes.")

if __name__ == "__main__":
    update()
