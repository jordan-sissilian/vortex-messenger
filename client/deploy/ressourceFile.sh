#!/bin/bash

# Répertoire où se trouvent vos ressources
ASSETS_DIR="Ressources"
# Nom du fichier .qrc à générer
RESSOURCES_DIR="Ressources"
QRC_FILE="$RESSOURCES_DIR/resources.qrc"

# Vérifiez si le répertoire des ressources existe
if [ ! -d "$ASSETS_DIR" ]; then
    echo "Erreur : Le répertoire $ASSETS_DIR n'existe pas."
    exit 1
fi

# Commencer à écrire le fichier .qrc
echo "<RCC>" > "$QRC_FILE"
echo "    <qresource prefix=\"/\">" >> "$QRC_FILE"

# Boucle à travers chaque fichier dans le répertoire des ressources
for asset in "$ASSETS_DIR"/*; do
    if [ -f "$asset" ]; then
        # Extraire le nom du fichier sans le chemin
        filename=$(basename "$asset")
        echo "        <file>$filename</file>" >> "$QRC_FILE"
    fi
done

# Terminer le fichier .qrc
echo "    </qresource>" >> "$QRC_FILE"
echo "</RCC>" >> "$QRC_FILE"

# Confirmation de création
echo "Fichier $QRC_FILE créé avec succès dans le répertoire $RESSOURCES_DIR."
