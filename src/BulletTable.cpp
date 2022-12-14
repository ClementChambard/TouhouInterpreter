#include "BulletTable.h"

#define NONE 0, 0, 0, 0
#define NONEm -1, -1, -1, 0

TableBulletSprite_t BULLET_TYPE_DEFINITIONS[] = {
{35,
 {{  0, 323, 336, 161},
  {  1, 324, 337, 164},
  {  2, 324, 338, 167},
  {  3, 325, 339, 170},
  {  4, 325, 340, 173},
  {  5, 326, 341, 176},
  {  6, 326, 342, 179},
  {  7, 327, 343, 182},
  {  8, 327, 344, 185},
  {  9, 328, 345, 188},
  { 10, 328, 346, 191},
  { 11, 328, 347, 194},
  { 12, 329, 348, 197},
  { 13, 329, 349, 200},
  { 14, 329, 350, 203},
  { 15, 330, 351, 206}},
 2.4f, 5, 6, 6, 0},

{36,
 {{  0, 323, 336, 161},
  {  1, 324, 337, 164},
  {  2, 324, 338, 167},
  {  3, 325, 339, 170},
  {  4, 325, 340, 173},
  {  5, 326, 341, 176},
  {  6, 326, 342, 179},
  {  7, 327, 343, 182},
  {  8, 327, 344, 185},
  {  9, 328, 345, 188},
  { 10, 328, 346, 191},
  { 11, 328, 347, 194},
  { 12, 329, 348, 197},
  { 13, 329, 349, 200},
  { 14, 329, 350, 203},
  { 15, 330, 351, 206}},
 2.4f, 5, 6, 6, 0},

{37,
 {{ 16, 323, 336, 209},
  { 17, 324, 337, 212},
  { 18, 324, 338, 215},
  { 19, 325, 339, 218},
  { 20, 325, 340, 221},
  { 21, 326, 341, 224},
  { 22, 326, 342, 227},
  { 23, 327, 343, 230},
  { 24, 327, 344, 233},
  { 25, 328, 345, 236},
  { 26, 328, 346, 239},
  { 27, 328, 347, 242},
  { 28, 329, 348, 245},
  { 29, 329, 349, 248},
  { 30, 329, 350, 251},
  { 31, 330, 351, 254}},
 2.4f, 5, 6, 6, 0},

{38,
 {{ 48, 323, 336, 209},
  { 49, 324, 337, 212},
  { 50, 324, 338, 215},
  { 51, 325, 339, 218},
  { 52, 325, 340, 221},
  { 53, 326, 341, 224},
  { 54, 326, 342, 227},
  { 55, 327, 343, 230},
  { 56, 327, 344, 233},
  { 57, 328, 345, 236},
  { 58, 328, 346, 239},
  { 59, 328, 347, 242},
  { 60, 329, 348, 245},
  { 61, 329, 349, 248},
  { 62, 329, 350, 251},
  { 63, 330, 351, 254}},
 2.0f, 5, 6, 6, 0},

{39,
 {{ 64, 323, 336, 209},
  { 65, 324, 337, 212},
  { 66, 324, 338, 215},
  { 67, 325, 339, 218},
  { 68, 325, 340, 221},
  { 69, 326, 341, 224},
  { 70, 326, 342, 227},
  { 71, 327, 343, 230},
  { 72, 327, 344, 233},
  { 73, 328, 345, 236},
  { 74, 328, 346, 239},
  { 75, 328, 347, 242},
  { 76, 329, 348, 245},
  { 77, 329, 349, 248},
  { 78, 329, 350, 251},
  { 79, 330, 351, 254}},
 4.0f, 3, 6, 6, 0},

{40,
 {{ 80, 323, 336, 209},
  { 81, 324, 337, 212},
  { 82, 324, 338, 215},
  { 83, 325, 339, 218},
  { 84, 325, 340, 221},
  { 85, 326, 341, 224},
  { 86, 326, 342, 227},
  { 87, 327, 343, 230},
  { 88, 327, 344, 233},
  { 89, 328, 345, 236},
  { 90, 328, 346, 239},
  { 91, 328, 347, 242},
  { 92, 329, 348, 245},
  { 93, 329, 349, 248},
  { 94, 329, 350, 251},
  { 95, 330, 351, 254}},
 4.0f, 3, 6, 6, 0},

{41,
 {{ 96, 323, 336, 209},
  { 97, 324, 337, 212},
  { 98, 324, 338, 215},
  { 99, 325, 339, 218},
  {100, 325, 340, 221},
  {101, 326, 341, 224},
  {102, 326, 342, 227},
  {103, 327, 343, 230},
  {104, 327, 344, 233},
  {105, 328, 345, 236},
  {106, 328, 346, 239},
  {107, 328, 347, 242},
  {108, 329, 348, 245},
  {109, 329, 349, 248},
  {110, 329, 350, 251},
  {111, 330, 351, 254}},
 4.0f, 3, 6, 6, 0},

{42,
 {{112, 323, 336, 209},
  {113, 324, 337, 212},
  {114, 324, 338, 215},
  {115, 325, 339, 218},
  {116, 325, 340, 221},
  {117, 326, 341, 224},
  {118, 326, 342, 227},
  {119, 327, 343, 230},
  {120, 327, 344, 233},
  {121, 328, 345, 236},
  {122, 328, 346, 239},
  {123, 328, 347, 242},
  {124, 329, 348, 245},
  {125, 329, 349, 248},
  {126, 329, 350, 251},
  {127, 330, 351, 254}},
 4.0f, 3, 6, 6, 0},

{43,
 {{128, 323, 336, 209},
  {129, 324, 337, 212},
  {130, 324, 338, 215},
  {131, 325, 339, 218},
  {132, 325, 340, 221},
  {133, 326, 341, 224},
  {134, 326, 342, 227},
  {135, 327, 343, 230},
  {136, 327, 344, 233},
  {137, 328, 345, 236},
  {138, 328, 346, 239},
  {139, 328, 347, 242},
  {140, 329, 348, 245},
  {141, 329, 349, 248},
  {142, 329, 350, 251},
  {143, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{44,
 {{160, 323, 336, 209},
  {161, 324, 337, 212},
  {162, 324, 338, 215},
  {163, 325, 339, 218},
  {164, 325, 340, 221},
  {165, 326, 341, 224},
  {166, 326, 342, 227},
  {167, 327, 343, 230},
  {168, 327, 344, 233},
  {169, 328, 345, 236},
  {170, 328, 346, 239},
  {171, 328, 347, 242},
  {172, 329, 348, 245},
  {173, 329, 349, 248},
  {174, 329, 350, 251},
  {175, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{45,
 {{176, 323, 336, 209},
  {177, 324, 337, 212},
  {178, 324, 338, 215},
  {179, 325, 339, 218},
  {180, 325, 340, 221},
  {181, 326, 341, 224},
  {182, 326, 342, 227},
  {183, 327, 343, 230},
  {184, 327, 344, 233},
  {185, 328, 345, 236},
  {186, 328, 346, 239},
  {187, 328, 347, 242},
  {188, 329, 348, 245},
  {189, 329, 349, 248},
  {190, 329, 350, 251},
  {191, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{46,
 {{208, 323, 336, 209},
  {209, 324, 337, 212},
  {210, 324, 338, 215},
  {211, 325, 339, 218},
  {212, 325, 340, 221},
  {213, 326, 341, 224},
  {214, 326, 342, 227},
  {215, 327, 343, 230},
  {216, 327, 344, 233},
  {217, 328, 345, 236},
  {218, 328, 346, 239},
  {219, 328, 347, 242},
  {220, 329, 348, 245},
  {221, 329, 349, 248},
  {222, 329, 350, 251},
  {223, 330, 351, 254}},
 2.8f, 3, 6, 6, 0},

{47,
 {{224, 323, 336, 209},
  {225, 324, 337, 212},
  {226, 324, 338, 215},
  {227, 325, 339, 218},
  {228, 325, 340, 221},
  {229, 326, 341, 224},
  {230, 326, 342, 227},
  {231, 327, 343, 230},
  {232, 327, 344, 233},
  {233, 328, 345, 236},
  {234, 328, 346, 239},
  {235, 328, 347, 242},
  {236, 329, 348, 245},
  {237, 329, 349, 248},
  {238, 329, 350, 251},
  {239, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{48,
 {{240, 323, 336, 209},
  {241, 324, 337, 212},
  {242, 324, 338, 215},
  {243, 325, 339, 218},
  {244, 325, 340, 221},
  {245, 326, 341, 224},
  {246, 326, 342, 227},
  {247, 327, 343, 230},
  {248, 327, 344, 233},
  {249, 328, 345, 236},
  {250, 328, 346, 239},
  {251, 328, 347, 242},
  {252, 329, 348, 245},
  {253, 329, 349, 248},
  {254, 329, 350, 251},
  {255, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{49,
 {{256, 323, 336, 209},
  {257, 324, 337, 212},
  {258, 324, 338, 215},
  {259, 325, 339, 218},
  {260, 325, 340, 221},
  {261, 326, 341, 224},
  {262, 326, 342, 227},
  {263, 327, 343, 230},
  {264, 327, 344, 233},
  {265, 328, 345, 236},
  {266, 328, 346, 239},
  {267, 328, 347, 242},
  {268, 329, 348, 245},
  {269, 329, 349, 248},
  {270, 329, 350, 251},
  {271, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{50,
 {{272, 323, 336, 209},
  {273, 324, 337, 212},
  {274, 324, 338, 215},
  {275, 325, 339, 218},
  {276, 325, 340, 221},
  {277, 326, 341, 224},
  {278, 326, 342, 227},
  {279, 327, 343, 230},
  {280, 327, 344, 233},
  {281, 328, 345, 236},
  {282, 328, 346, 239},
  {283, 328, 347, 242},
  {284, 329, 348, 245},
  {285, 329, 349, 248},
  {286, 329, 350, 251},
  {287, 330, 351, 254}},
 2.4f, 4, 6, 6, 0},

{51,
 {{288, 323, 336, 209},
  {289, 324, 337, 212},
  {290, 324, 338, 215},
  {291, 325, 339, 218},
  {292, 325, 340, 221},
  {293, 326, 341, 224},
  {294, 326, 342, 227},
  {295, 327, 343, 230},
  {296, 327, 344, 233},
  {297, 328, 345, 236},
  {298, 328, 346, 239},
  {299, 328, 347, 242},
  {300, 329, 348, 245},
  {301, 329, 349, 248},
  {302, 329, 350, 251},
  {303, 330, 351, 254}},
 4.0f, 3, 6, 6, 0},

{52,
 {{320, 329, 340, 221},
  {321, 330, 336, 209},
  {322, 324, 337, 212},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 4.0f, 3, 0, 6, 0},

{72,
 {{368, 323, 0, 257},
  {369, 324, 0, 260},
  {370, 325, 0, 263},
  {371, 326, 0, 266},
  {372, 327, 0, 269},
  {373, 328, 0, 272},
  {374, 329, 0, 275},
  {375, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 8.5f, 1, 6, 12, 0},

{73,
 {{376, 323, 0, 257},
  {377, 324, 0, 260},
  {378, 325, 0, 263},
  {379, 326, 0, 266},
  {380, 327, 0, 269},
  {381, 328, 0, 272},
  {382, 329, 0, 275},
  {383, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 8.5f, 1, 6, 12, 0},

{74,
 {{392, 323, 0, 257},
  {393, 324, 0, 260},
  {394, 325, 0, 263},
  {395, 326, 0, 266},
  {396, 327, 0, 269},
  {397, 328, 0, 272},
  {398, 329, 0, 275},
  {399, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 7.0f, 2, 6, 12, 0},

{75,
 {{400, 323, 0, 257},
  {401, 324, 0, 260},
  {402, 325, 0, 263},
  {403, 326, 0, 266},
  {404, 327, 0, 269},
  {405, 328, 0, 272},
  {406, 329, 0, 275},
  {407, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 6.0f, 2, 6, 12, 0},

{76,
 {{408, 323, 0, 257},
  {409, 324, 0, 260},
  {410, 325, 0, 263},
  {411, 326, 0, 266},
  {412, 327, 0, 269},
  {413, 328, 0, 272},
  {414, 329, 0, 275},
  {415, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 7.0f, 2, 6, 12, 0},

{77,
 {{416, 323, 0, 257},
  {417, 324, 0, 260},
  {418, 325, 0, 263},
  {419, 326, 0, 266},
  {420, 327, 0, 269},
  {421, 328, 0, 272},
  {422, 329, 0, 275},
  {423, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 7.0f, 1, 6, 12, 0},

{78,
 {{424, 323, 0, 257},
  {425, 324, 0, 260},
  {426, 325, 0, 263},
  {427, 326, 0, 266},
  {428, 327, 0, 269},
  {429, 328, 0, 272},
  {430, 329, 0, 275},
  {431, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 7.0f, 1, 6, 12, 0},

{108,
 {{-1, -1, 0, 263},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
6.0f, 2, 7, 12, 0},

{109,
 {{-1, -1, 0, 260},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
6.0f, 2, 8, 12, 0},

{110,
 {{-1, -1, 0, 266},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 2, 9, 12, 0},

{111,
 {{-1, -1, 0, 275},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 2, 10, 12, 0},

{81,
 {{444, 323, 0, 257},
  {445, 324, 0, 260},
  {446, 325, 0, 263},
  {447, 326, 0, 266},
  {448, 327, 0, 269},
  {449, 328, 0, 272},
  {450, 329, 0, 275},
  {451, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 10.f, 1, 6, 12, 0},

{79,
 {{384, 323, 0, 257},
  {385, 324, 0, 260},
  {386, 325, 0, 263},
  {387, 326, 0, 266},
  {388, 327, 0, 269},
  {389, 328, 0, 272},
  {390, 329, 0, 275},
  {391, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 7.0f, 1, 6, 12, 0},

{82,
 {{452, 323, 0, 257},
  {453, 324, 0, 260},
  {454, 325, 0, 263},
  {455, 326, 0, 266},
  {456, 327, 0, 269},
  {457, 328, 0, 272},
  {458, 329, 0, 275},
  {459, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 4.0f, 3, 6, 12, 0},

{80,
 {{432, 0, 0, 281},
  {433, 0, 0, 284},
  {434, 0, 0, 287},
  {435, 0, 0, 290},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 14.f, 0, 6, 60, 0},

{315,
 {{548, 0, 0, 257},
  {549, 0, 0, 260},
  {550, 0, 0, 263},
  {551, 0, 0, 266},
  {552, 0, 0, 269},
  {553, 0, 0, 272},
  {554, 0, 0, 275},
  {555, 0, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 12.f, 2, 6, 60, 0},

{107,
 {{468, 323, 336, 209},
  {469, 324, 337, 212},
  {470, 324, 338, 215},
  {471, 325, 339, 218},
  {472, 325, 340, 221},
  {473, 326, 341, 224},
  {474, 326, 342, 227},
  {475, 327, 343, 230},
  {476, 327, 344, 233},
  {477, 328, 345, 236},
  {478, 328, 346, 239},
  {479, 328, 347, 242},
  {480, 329, 348, 245},
  {481, 329, 349, 248},
  {482, 329, 350, 251},
  {483, 330, 351, 254}},
 2.4f, 5, 6, 8, 0},

{53,
 {{144, 323, 336, 209},
  {145, 324, 337, 212},
  {146, 324, 338, 215},
  {147, 325, 339, 218},
  {148, 325, 340, 221},
  {149, 326, 341, 224},
  {150, 326, 342, 227},
  {151, 327, 343, 230},
  {152, 327, 344, 233},
  {153, 328, 345, 236},
  {154, 328, 346, 239},
  {155, 328, 347, 242},
  {156, 329, 348, 245},
  {157, 329, 349, 248},
  {158, 329, 350, 251},
  {159, 330, 351, 254}},
 3.2f, 4, 6, 8, 0},

{54,
 {{192, 323, 336, 209},
  {193, 324, 337, 212},
  {194, 324, 338, 215},
  {195, 325, 339, 218},
  {196, 325, 340, 221},
  {197, 326, 341, 224},
  {198, 326, 342, 227},
  {199, 327, 343, 230},
  {200, 327, 344, 233},
  {201, 328, 345, 236},
  {202, 328, 346, 239},
  {203, 328, 347, 242},
  {204, 329, 348, 245},
  {205, 329, 349, 248},
  {206, 329, 350, 251},
  {207, 330, 351, 254}},
 3.2f, 4, 6, 8, 0},

{55,
 {{304, 323, 336, 209},
  {305, 324, 337, 212},
  {306, 324, 338, 215},
  {307, 325, 339, 218},
  {308, 325, 340, 221},
  {309, 326, 341, 224},
  {310, 326, 342, 227},
  {311, 327, 343, 230},
  {312, 327, 344, 233},
  {313, 328, 345, 236},
  {314, 328, 346, 239},
  {315, 328, 347, 242},
  {316, 329, 348, 245},
  {317, 329, 349, 248},
  {318, 329, 350, 251},
  {319, 330, 351, 254}},
 4.0f, 3, 6, 8, 0},

{158,
 {{352, -1, 336, 209},
  {353, -1, 337, 212},
  {354, -1, 338, 215},
  {355, -1, 339, 218},
  {356, -1, 340, 221},
  {357, -1, 341, 224},
  {358, -1, 342, 227},
  {359, -1, 343, 230},
  {360, -1, 344, 233},
  {361, -1, 345, 236},
  {362, -1, 346, 239},
  {363, -1, 347, 242},
  {364, -1, 348, 245},
  {365, -1, 349, 248},
  {366, -1, 350, 251},
  {367, -1, 351, 254}},
 4.0f, 2, 6, 30, 0},

{317,
 {{-1, -1, 0, 260},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 3, 6, 12, 0},

{318,
 {{-1, -1, 0, 266},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 3, 6, 12, 0},

{319,
 {{-1, -1, 0, 275},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 3, 6, 12, 0},

{320,
 {{-1, -1, 0, 263},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm},
  {NONEm}},
4.0f, 3, 6, 12, 0},

{321,
 {{568, 323, 0, 257},
  {569, 324, 0, 260},
  {570, 325, 0, 263},
  {571, 326, 0, 266},
  {572, 327, 0, 269},
  {573, 328, 0, 272},
  {574, 329, 0, 275},
  {575, 330, 0, 278},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 5.0f, 2, 6, 12, 0},

{324,
 {{580, 323, 0, 257 },
  {581, 324, 0, 260 },
  {582, 325, 0, 263 },
  {583, 326, 0, 266 },
  {584, 327, 0, 269 },
  {585, 328, 0, 272 },
  {586, 329, 0, 275 },
  {587, 330, 0, 278 },
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 10.0f, 1, 6, 12, 0},

{325,
 {{588, 323, 0, 257 },
  {589, 324, 0, 260 },
  {590, 325, 0, 263 },
  {591, 326, 0, 266 },
  {592, 327, 0, 269 },
  {593, 328, 0, 272 },
  {594, 329, 0, 275 },
  {595, 330, 0, 278 },
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 10.0f, 1, 6, 12, 0},

{326,
 {{596, 0, 0, 281 },
  {597, 0, 0, 284 },
  {598, 0, 0, 287 },
  {599, 0, 0, 290 },
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 28.0f, 0, 6, 60, 0},

{327,
 {{596, 0, 0, 281 },
  {597, 0, 0, 284 },
  {598, 0, 0, 287 },
  {599, 0, 0, 290 },
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE},
  {NONE}},
 28.0f, 0, 6, 60, 0
},

{35,
 {{  0, 323, 336, 161},
  {  1, 324, 337, 164},
  {  2, 324, 338, 167},
  {  3, 325, 339, 170},
  {  4, 325, 340, 173},
  {  5, 326, 341, 176},
  {  6, 326, 342, 179},
  {  7, 327, 343, 182},
  {  8, 327, 344, 185},
  {  9, 328, 345, 188},
  { 10, 328, 346, 191},
  { 11, 328, 347, 194},
  { 12, 329, 348, 197},
  { 13, 329, 349, 200},
  { 14, 329, 350, 203},
  { 15, 330, 351, 206}},
 2.4f, 5, 6, 6, 0},

};
