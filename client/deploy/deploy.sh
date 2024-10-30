#!/bin/bash

# Couleurs pour les messages d'information
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
RESET="\033[0m"

# Configuration
APP_NAME="CryptoMadness_Client"
BUNDLE_NAME="Vortex Messenger.app"  # Nom du bundle .app
DMG_NAME="Vortex Messenger Installer.dmg"  # Nom du dmg
BIN_DIR="bin"
ASSETS_DIR="Ressources"
DEPLOY_DIR="deploy"

CLIENT_EXEC="./${BIN_DIR}/${APP_NAME}"

# Vérification de l'existence de l'exécutable
if [ ! -f "$CLIENT_EXEC" ]; then
    echo -e "${RED}Erreur : L'exécutable $CLIENT_EXEC n'existe pas. Assurez-vous de l'avoir compilé.${RESET}"
    exit 1
fi

# Création du bundle de l'application
echo -e "${BLUE}Création du bundle ${BUNDLE_NAME}...${RESET}"
mkdir -p "$DEPLOY_DIR/$BUNDLE_NAME/Contents/MacOS"
mkdir -p "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Resources"
mkdir -p "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks"
mkdir -p "$DEPLOY_DIR/$BUNDLE_NAME/Contents/PlugIns"

# Copie de l'exécutable dans le bundle
echo -e "${BLUE}Copie de l'exécutable dans le bundle...${RESET}"
cp "$CLIENT_EXEC" "$DEPLOY_DIR/$BUNDLE_NAME/Contents/MacOS/"

# Vérifiez si l'exécutable a été copié avec succès
if [ -f "$DEPLOY_DIR/$BUNDLE_NAME/Contents/MacOS/$APP_NAME" ]; then
    echo -e "${GREEN}Exécutable copié avec succès.${RESET}"
else
    echo -e "${RED}Erreur : L'exécutable n'a pas été copié dans le bundle.${RESET}"
    exit 1
fi

# Liste des fichiers de ressources à copier
ASSET_FILES=(
    "attach.svg"
    "avatar_fox.svg"
    "avatar_ghost.svg"
    "avatar_heart.svg"
    "avatar_incognito.svg"
    "avatar_soccerball.svg"
    "bell.svg"
    "block.svg"
    "chat-x.svg"
    "chat.svg"
    "check.svg"
    "compose.svg"
    "deleteImg.png"
    "file.svg"
    "info.svg"
    "leave.svg"
    "lock.svg"
    "logo.png"
    "menu.svg"
    "messagestatus-delivered.svg"
    "messagestatus-read.svg"
    "messagestatus-sending.svg"
    "messagestatus-sent.svg"
    "more.svg"
    "open-compact.svg"
    "photo-square.svg"
    "plus.svg"
    "recent.svg"
    "redo.svg"
    "refresh.svg"
    "save.svg"
    "sendMsgBtn.png"
    "settings.svg"
    "sync.svg"
    "tune.svg"
)

# Copie des ressources dans le bundle
echo -e "${BLUE}Copie des ressources dans le bundle...${RESET}"
for asset in "${ASSET_FILES[@]}"; do
    cp "$ASSETS_DIR/$asset" "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Resources/"
done

# Création du fichier Info.plist
cat <<EOL > "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>Vortex Messenger</string>  <!-- Nom de l'application -->
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleIdentifier</key>
    <string>com.jordansissilian.vortexmessenger</string>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIconFile</key>
    <string>icon.icns</string>
</dict>
</plist>
EOL

# Création de l'icône de l'application
ICON_PATH="$ASSETS_DIR/logo.png"
ICON_DEST="$DEPLOY_DIR/$BUNDLE_NAME/Contents/Resources/icon.icns"

if [ -f "$ICON_PATH" ]; then
    echo -e "${BLUE}Création de l'icône de l'application...${RESET}"
    mkdir -p "tempicon.iconset"
    sips -z 16 16 "$ICON_PATH" --out "tempicon.iconset/icon_16x16.png"
    sips -z 32 32 "$ICON_PATH" --out "tempicon.iconset/icon_32x32.png"
    sips -z 128 128 "$ICON_PATH" --out "tempicon.iconset/icon_128x128.png"
    sips -z 256 256 "$ICON_PATH" --out "tempicon.iconset/icon_256x256.png"
    sips -z 512 512 "$ICON_PATH" --out "tempicon.iconset/icon_512x512.png"
    cp "$ICON_PATH" "tempicon.iconset/icon_512x512@2x.png"
    
    iconutil -c icns "tempicon.iconset"
    mv "tempicon.icns" "$ICON_DEST"
    rm -rf "tempicon.iconset"
else
    echo -e "${YELLOW}Aucune icône trouvée à l'emplacement spécifié : $ICON_PATH. L'icône par défaut ne sera pas ajoutée.${RESET}"
fi

# Copie des bibliothèques nécessaires
echo -e "${BLUE}Copie des bibliothèques nécessaires...${RESET}"
cp /opt/homebrew/lib/libcrypto.3.dylib "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks"
cp /opt/homebrew/lib/libssl.3.dylib "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks"
cp /opt/homebrew/lib/libcryptopp.dylib "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks"

# Vérifiez si les bibliothèques ont été copiées avec succès
if [ -f "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks/libcrypto.3.dylib" ] && [ -f "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks/libssl.3.dylib" ] && [ -f "$DEPLOY_DIR/$BUNDLE_NAME/Contents/Frameworks/libcryptopp.dylib" ]; then
    echo -e "${GREEN}Bibliothèques copiées avec succès.${RESET}"
else
    echo -e "${RED}Erreur : Une ou plusieurs bibliothèques n'ont pas été copiées.${RESET}"
    exit 1
fi

# Création de qt.conf
cat <<EOL > "$DEPLOY_DIR/$BUNDLE_NAME/Contents/qt.conf"
[Paths]
Plugins = PlugIns
EOL

# Copie des plugins Qt
echo -e "${BLUE}Copie des plugins Qt...${RESET}"
PLUGINS_DIR="$DEPLOY_DIR/$BUNDLE_NAME/Contents/PlugIns"
mkdir -p "$PLUGINS_DIR"
cp -R /Users/jordan/Qt/6.7.2/macos/plugins/styles "$PLUGINS_DIR/"
cp -R /Users/jordan/Qt/6.7.2/macos/plugins/imageformats "$PLUGINS_DIR/"

# Déploiement des bibliothèques Qt
echo -e "${BLUE}Déploiement des bibliothèques Qt...${RESET}"
MACDEPLOYQT_PATH="/Users/jordan/Qt/6.7.2/macos/bin/macdeployqt"

if [ -f "$MACDEPLOYQT_PATH" ]; then
    # Utiliser le bundle complet pour le déploiement
    "$MACDEPLOYQT_PATH" "$DEPLOY_DIR/$BUNDLE_NAME" -no-strip
else
    echo -e "${RED}Erreur : macdeployqt n'est pas installé. Veuillez installer Qt pour utiliser cette fonctionnalité.${RESET}"
    exit 1
fi

# Signature de l'application
echo -e "${BLUE}Signature de l'application...${RESET}"
codesign --deep --force --verify --verbose --sign "Apple Development: sissilian.jordan@icloud.com (78NS46KWAA)" "$DEPLOY_DIR/$BUNDLE_NAME"

# Vérification de la signature de l'application
if codesign -vvv --deep --strict "$DEPLOY_DIR/$BUNDLE_NAME"; then
    echo -e "${GREEN}Application signée avec succès.${RESET}"
else
    echo -e "${RED}Erreur lors de la signature de l'application.${RESET}"
    exit 1
fi

echo -e "${BLUE}Création du DMG...${RESET}"
hdiutil create -volname 'Vortex Messenger' -srcfolder "$DEPLOY_DIR/$BUNDLE_NAME" -format UDZO -o "$DEPLOY_DIR/$DMG_NAME"

# Signature du DMG
echo -e "${BLUE}Signature du DMG...${RESET}"
DMG_PATH="$DEPLOY_DIR/$DMG_NAME"

codesign --force --verify --verbose --deep --sign "Apple Development: sissilian.jordan@icloud.com (78NS46KWAA)" "$DMG_PATH"

chmod -R 755 "$DMG_PATH"

# Vérification de la signature du DMG
if codesign -vvv --strict "$DMG_PATH"; then
    echo -e "${GREEN}DMG signé avec succès.${RESET}"
else
    echo -e "${RED}Erreur lors de la signature du DMG.${RESET}"
    exit 1
fi

# Fin du déploiement
echo -e "${GREEN}Déploiement terminé ! Votre application est située dans le dossier : ${DEPLOY_DIR}/${BUNDLE_NAME}${RESET}"
