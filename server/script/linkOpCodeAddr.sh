#!/bin/bash

BINARY_PATH="../bin/CryptoMadness_Server"
OUTPUT_FILE="opcode.s"
FUNCTIONS_FILE="functions_to_include.txt"
OPCODES_FILE="opcodes.def"

if [[ ! -f "$BINARY_PATH" ]]; then
    echo "Erreur : le binaire '$BINARY_PATH' n'existe pas."
    exit 1
fi

if [[ ! -f "$FUNCTIONS_FILE" ]]; then
    echo "Erreur : le fichier '$FUNCTIONS_FILE' n'existe pas."
    exit 1
fi

if [[ ! -f "$OPCODES_FILE" ]]; then
    echo "Erreur : le fichier '$OPCODES_FILE' n'existe pas."
    exit 1
fi

echo ".text" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

while IFS= read -r opcode_definition; do
    echo ".extern $opcode_definition" >> "$OUTPUT_FILE"
done < "$OPCODES_FILE"
echo "" >> "$OUTPUT_FILE"

while IFS= read -r opcode_extern; do
    echo "$FUNCTIONS_FILE" >> "$OUTPUT_FILE"
done < "$OUTPUT_FILE"

echo "" >> "$OUTPUT_FILE"
echo ".global _callOpCodeFunc" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo "_callOpCodeFunc:" >> "$OUTPUT_FILE"
echo "    cmp x0, #0x04" >> "$OUTPUT_FILE"
echo "    b.gt .exit" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo "    lsl x2, x0, #3" >> "$OUTPUT_FILE"
echo "    ldr x3, =function_table" >> "$OUTPUT_FILE"
echo "    ldr x4, [x3, x2]" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo "    cbz x4, .exit" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo "    blr x4" >> "$OUTPUT_FILE"
echo "    b .exit" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo ".exit:" >> "$OUTPUT_FILE"
echo "    ret" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"
echo "// Section des données pour la table des fonctions" >> "$OUTPUT_FILE"
echo ".data" >> "$OUTPUT_FILE"
echo ".align 3" >> "$OUTPUT_FILE"
echo "function_table:" >> "$OUTPUT_FILE"

function_count=0
while IFS= read -r function_name; do
    address=$(nm "$BINARY_PATH" | grep ' T ' | grep "$function_name" | awk '{print $1}')

    if [[ -n "$address" ]]; then
        echo "    .quad 0x$address" >> "$OUTPUT_FILE"
        ((function_count++))
        echo "adresse trouvée pour la fonction: $function_name"
    else
        echo "Aucune adresse trouvée pour la fonction: $function_name"
    fi
done < "$FUNCTIONS_FILE"

echo "Processed $function_count functions."
echo "Table des opcodes générée avec succès dans '$OUTPUT_FILE'"
