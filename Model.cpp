#include "eeprom.h"
#include "LinkedList.h"

class Model {
    public:
        Model() {
            loadData();
        }
        static LinkedList<String> players;
        static LinkedList<uint16_t*> boards;
    private:
        String readPlayerName(int player) {
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

        uint16_t* readPlayerBoard(int player) {
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

        void writePlayerName(int player) {
            if (player >= 0 & player < 10) {
                String name = players.get(player);

                const int offset = player * 28;
                for (int i=0; i<20 & i<name.length(); i++) {
                    EEPROM.put(offset + i, (uint8_t)name[i]);
                }
            }
        }

        void writePlayerBoard(int player, uint16_t* board) {
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

        void loadData() {
            for (int i=0; i<10; i++) {
                String name = readPlayerName(i);
                if (name == "                    ") break;
                players.add(i, name);

                uint16_t* board = readPlayerBoard(i);
                boards.add(i, board);
            }
        }

        void writeData() {
            for (int i=0; i<players.size(); i++) {
                writePlayerName(i);
                writePlayerBoard(i, boards[i]);
            }
        }
};
