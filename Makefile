RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
RESET = \033[0m

.PHONY: all build clean clear server client client-local client-distant docker-server stop-docker-server

BIN_DIR = bin
BUILD_DIR = build
SERVER_DIR = server
CLIENT_DIR = client
KEY_DIR = key

RCC = /Users/jordan/Qt/6.7.2/macos/libexec/rcc

all: build

build: initalise
	@echo "$(BLUE)Create Key...$(RESET)"
	@xxd -r -p key/key > key/key.bin
	@as -arch arm64 -mmacosx-version-min=10.14 -o key/key_tmp.o key/key.s
	@objcopy --strip-unneeded key/key_tmp.o key/key.o

	@echo "$(BLUE)Move opcode.s in src/opcode ...$(RESET)"
	@cp $(SERVER_DIR)/script/opcode.s $(SERVER_DIR)/src/opcode/opcode.s

	@echo "$(BLUE)Creating directories...$(RESET)"
	@mkdir -p $(SERVER_DIR)/$(BIN_DIR) $(SERVER_DIR)/$(BIN_DIR)/objects $(CLIENT_DIR)/$(BIN_DIR)
	@mkdir -p $(SERVER_DIR)/$(BUILD_DIR) $(CLIENT_DIR)/$(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

	@echo "$(BLUE)Running cmake for server...$(RESET)"
	@cd $(SERVER_DIR)/$(BUILD_DIR) && cmake -DINCLUDE_OPCODE=OFF ..
	@echo "$(BLUE)Building server...$(RESET)"
	@cd $(SERVER_DIR)/$(BUILD_DIR) && $(MAKE)

	@echo "$(BLUE)Running cmake for client...$(RESET)"
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && cmake ..
	@echo "$(BLUE)Building client...$(RESET)"
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && $(MAKE)

server:
	@echo "$(BLUE)Building server...$(RESET)"
	@cd $(SERVER_DIR)/$(BUILD_DIR) && $(MAKE)
	@echo "$(GREEN)Creating server symbolic link and Start Server...$(RESET)"
	@ln -sf ../$(SERVER_DIR)/$(BIN_DIR)/CryptoMadness_Server $(BIN_DIR)/CryptoMadness_Server

client-local:
	@echo "$(BLUE)Building client in local mode...$(RESET)"
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && cmake -DLOCAL_SERVER=ON ..
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && $(MAKE)
	@echo "$(GREEN)Creating client symbolic link and Start Client...$(RESET)"
	@ln -sf ../$(CLIENT_DIR)/$(BIN_DIR)/CryptoMadness_Client $(BIN_DIR)/CryptoMadness_Client

client-distant:
	@echo "$(BLUE)Building client in distant mode...$(RESET)"
	@echo "$(YELLOW)create resources file...$(RESET)"
	@cd ./$(CLIENT_DIR) && ./deploy/ressourceFile.sh
	@cd ./$(CLIENT_DIR) && $(RCC) -o ./src/resources.cpp ./Ressources/resources.qrc
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && cmake -DLOCAL_SERVER=OFF ..
	@cd $(CLIENT_DIR)/$(BUILD_DIR) && $(MAKE)
	@echo "$(GREEN)Creating client symbolic link and Start Client...$(RESET)"
	@ln -sf ../$(CLIENT_DIR)/$(BIN_DIR)/CryptoMadness_Client $(BIN_DIR)/CryptoMadness_Client
	@echo "$(YELLOW)remove resources file...$(RESET)"
	@rm ./$(CLIENT_DIR)/src/resources.cpp ./$(CLIENT_DIR)/Ressources/resources.qrc

clear:
	@echo "$(YELLOW)Clearing...$(RESET)"
	@rm -rf $(SERVER_DIR)/$(BIN_DIR)/* $(CLIENT_DIR)/$(BIN_DIR)/*
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(CLIENT_DIR)/deploy/*.app $(CLIENT_DIR)/deploy/*.dmg $(CLIENT_DIR)/deploy/*.qrc
	@echo "$(RED)Cleaning key$(RESET)"
	@rm $(KEY_DIR)/key_tmp.o $(KEY_DIR)/key.o $(KEY_DIR)/key.bin
	@echo "$(RED)Cleaning opcode$(RESET)"
	@rm server/src/opcode/opcode.o

clean: delete
	@echo "$(RED)Cleaning build and bin directories...$(RESET)"
	@rm -rf $(SERVER_DIR)/$(BUILD_DIR) $(CLIENT_DIR)/$(BUILD_DIR)
	@rm -rf $(SERVER_DIR)/$(BIN_DIR) $(CLIENT_DIR)/$(BIN_DIR)
	@rm -rf $(BIN_DIR)
	@echo "$(RED)Cleaning deploy$(RESET)"
	@rm -rf $(CLIENT_DIR)/deploy/*.app  $(CLIENT_DIR)/deploy/*.dmg $(CLIENT_DIR)/deploy/*.qrc
	@echo "$(RED)Cleaning key$(RESET)"
	@rm $(KEY_DIR)/key_tmp.o $(KEY_DIR)/key.o $(KEY_DIR)/key.bin
	@echo "$(RED)Cleaning opcode$(RESET)"
	@rm server/src/opcode/opcode.o

initalise:
	@echo "$(BLUE)Create Server Dir$(RESET)"
	@mkdir -p /Users/Shared/ServerCryptoMadnessVortex/server/{queue,rooms,users_key,users_key_serv}
	@echo "$(BLUE)Create Client Dir$(RESET)"
	@mkdir -p /Users/Shared/ClientCryptoMadnessVortex/dir_user/{config,contact,conv/mp,conv/room,key,queue}

delete:
	@echo "$(YELLOW)Delete Server Dir$(RESET)"
	@rm -rf /Users/Shared/ServerCryptoMadnessVortex
	@echo "$(YELLOW)Delete Client Dir$(RESET)"
	@rm -rf /Users/Shared/ClientCryptoMadnessVortex

reset: delete initalise

deploy-client: client-distant
	@echo "$(BLUE)Deploying client...$(RESET)"
	@cd ./client && ./deploy/deploy.sh

docker-server: stop-docker-server
	@echo "$(BLUE)Building Docker image for server...$(RESET)"
	@docker build -t cryptomadness-server ./server
	@echo "$(BLUE)Running server in Docker container...$(RESET)"
	@docker run -d -p 9999:9999 --name cryptomadness-server cryptomadness-server

stop-docker-server:
	@echo "$(YELLOW)Stopping and removing Docker container...$(RESET)"
	@docker stop cryptomadness-server || true
	@docker rm cryptomadness-server || true