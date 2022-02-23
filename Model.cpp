#include "Arduino.h"
#include "eeprom.h"
#include "LinkedList.h"

#include "Constants.h"

class Model {
    public:
            
        static uint16_t* getBoard(int player) {
            return boards[player];
        }

        static void setBoard(int player, uint16_t* board) {
            for (int i=0; i<BOARD_SIZE; i++) {
                boards[player][i] = board[i];
                writePlayerBoard(player, board);
            }
        }

        static int addPlayer(String name) {
            uint16_t emptyBoard[BOARD_SIZE] = {};
            players.add(name);
            boards.add(emptyBoard);
            writeData();
            return players.size() - 1;
        }

        static void removePlayer(int player) {
            players.remove(player);
            boards.remove(player);
            writeData();
        }

        static void loadData() {
            for (int i=0; i<10; i++) {
                String name = readPlayerName(i);
                if (name == "                    ") break;
                players.add(i, name);

                uint16_t* board = readPlayerBoard(i);
                boards.add(i, board);
            }
        }

    private:
        static LinkedList<String> players;
        static LinkedList<uint16_t*> boards;

        static String readPlayerName(int player) {
            if (player < 0 || player >= 10) {
                return "";
            }

            String name;
            for (int i=0; i<20; i++) {        
                const int offset = player * 28;
                name += (char)EEPROM.read(offset + i);
            }

            return name;
        }

        static uint16_t* readPlayerBoard(int player) {
            static uint16_t emptyBoard [4];
            if (player < 0 || player >= 10) {
                return emptyBoard;
            }

            static uint16_t board [4];
            for (int part=0; part<4; part++) {
                const int offset = player * 28 + 20;
                uint8_t firstOctet = (uint8_t)EEPROM.read(offset + 2 * part);
                uint8_t secondOctet = (uint8_t)EEPROM.read(offset + 2 * part + 1);
                board[part] = firstOctet * 256 + secondOctet;
            }

            return board;
        }

        static void writePlayerName(int player) {
            if (player >= 0 & player < 10) {
                String name = players.get(player);

                const int offset = player * 28;
                for (int i=0; i<20 & i<name.length(); i++) {
                    EEPROM.put(offset + i, (uint8_t)name[i]);
                }
            }
        }

        static void writePlayerBoard(int player, uint16_t* board) {
            if (player >= 0 & player < 10) {
                String name = players.get(player);

                for (int part=0; part<4; part++) {
                    const int offset = player * 28 + 20;
                    uint8_t firstOctet = board[part] / 256;
                    uint8_t secondOctet = board[part] % 256;
                    EEPROM.put(offset + 2 * part, firstOctet);
                    EEPROM.put(offset + 2 * part + 1, secondOctet);
                }
            }
        }

        static void resetMemory() {
            for (int i=0; i<EEPROM.length(); i++) {
                EEPROM.write(i, 0);
            }
        }

        static void writeData() {
            for (int i=0; i<players.size(); i++) {
                writePlayerName(i);
                writePlayerBoard(i, boards[i]);
            }
        }
};
