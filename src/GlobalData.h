#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

class Globals {
    public:
    static Globals* get() { static Globals* inst = new Globals(); return inst; }
        float playerX = 0.f;
        float playerY = 224.f;
        int difficulty = 3;
        int rank = 0;
        int shottype = 0;
        int timeout = 0;
        int power = 0;
        int DS1 = 0;
        int DS2 = 0;
        int DS3 = 0;
        int DS4 = 0;
        int photoCount = 0;
        int spellID = -1;
        int chapter = 0;
        int miss_count_total = 0;
        int hyper = 0;
        int graze_recent = 0;
        int goasts = 0;
        int hyperdying = 0;
        int achievement_mode = 0;
};

#endif // GLOBALDATA_H_
