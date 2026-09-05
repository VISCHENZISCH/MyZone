import os
import urllib.request

urls = {
    "manuf": "https://www.wireshark.org/download/automated/data/manuf",
    "nmap-os-db": "https://raw.githubusercontent.com/nmap/nmap/master/nmap-os-db",
    "nmap-service-probes": "https://raw.githubusercontent.com/nmap/nmap/master/nmap-service-probes",
    "p0f.fp": "https://raw.githubusercontent.com/lcamtuf/p0f3/master/docs/p0f.fp",
    "dhcp_fingerprints.conf": "https://raw.githubusercontent.com/inverse-inc/packetfence/master/conf/dhcp_fingerprints.conf"
}

target_dir = "myzone_dbs"
os.makedirs(target_dir, exist_ok=True)

print(f"[*] Téléchargement des bases de données de fingerprinting dans le dossier '{target_dir}'...")

for name, url in urls.items():
    file_path = os.path.join(target_dir, name)
    print(f" -> Récupération de {name}...")
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'MyZone-Setup/1.0'})
        with urllib.request.urlopen(req) as response:
            with open(file_path, 'wb') as f:
                f.write(response.read())
        print(f"    [+] Succès: {name} sauvegardé.")
    except Exception as e:
        print(f"    [-] Erreur lors du téléchargement de {name}: {e}")

print("[*] Terminé ! Ces bases en texte brut sont prêtes à être intégrées et parsées.")