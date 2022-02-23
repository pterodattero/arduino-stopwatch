#pragma once

class Model {
    public:
        static void loadData();

        static uint16_t* getBoard(int player);
        static void setBoard(int player, uint16_t* board);

        static String getPlayerName(int player);
        static int getPlayersSize();
        static int addPlayer(String name);
        static void removePlayer(int player);
};
