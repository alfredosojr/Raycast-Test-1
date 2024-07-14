#include <SDL.h>
#include <iostream>
#include <math.h>
#include <SDL_Image.h>
#include <vector>

using namespace std;

const int WIDTH = 640;
const int HEIGHT = 640;
const double FOV = 60 * (M_PI / 180);
//(viewWindowBox.width/2)/m.tan((fov/2)*(m.pi/180))
const double PLANEDIST = (WIDTH / 2) / tan(FOV/2);
const int BLOCKSIZE = 64;

const int VIEWDIST = 64 *10;
const double VIEWDISTDOUBLE = (double)VIEWDIST;

int main(int argc, char* argv[]) {
	bool running = true;
	SDL_Event e;
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window* window = SDL_CreateWindow("jskfjlskjfksdf", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	cout << SDL_GetError() << endl;

	//load texture
	SDL_Surface* tileSheet = IMG_Load("assets/tileSheet.png");
	SDL_Texture* tileTexture = SDL_CreateTextureFromSurface(renderer, tileSheet);

	//vector<SDL_Texture>* wallsVec = nullptr;
	//vector<SDL_Texture>* decorVec = nullptr;

	//playerdata
	double playerX = 0;
	double playerY = 0;
	double playerAngle = M_PI;

	string mapString =
		"10100000000000000000 " //32 64x64 blocks per row.
		"01000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000000000000000 "
		"00000000001111000111 "
		"00000000000000000001 "
		"00000000010001000111 "
		"00000000010001000100 "
		"00000000010001000100 "
		"03333300010003000100 "
		"03000000000000000100 "
		"03000000000400000100 "
		"00030030060000000100 "
		"000000000000000&0100 ";

	//string mapString =
	//	"00000000000000000000 "
	//	"00&01050000000000000 "
	//	"40000050000000000000 "
	//	"00302000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 "
	//	"00000000000000000000 ";

	//32x20 blocks
	int MAPWIDTH = 32 * BLOCKSIZE;
	int MAPHEIGHT = 20 * BLOCKSIZE;

	vector<vector<int>> mapVec(MAPHEIGHT, vector<int>(MAPWIDTH));

	int iterX = 0;
	int iterY = 0;
	int num = 0;

	for (auto b : mapString) {
		switch (b) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			num = b - 48 - 1; //gets num from char, ascii starts at 48. -1 for index
			std::cout << "b: " << b << endl;

			// top
			for (int i = 0; i < BLOCKSIZE; i++) {
				mapVec[iterY * BLOCKSIZE][iterX * BLOCKSIZE + i] = ((64 * num) + i);
			}

			//sides
			for (int j = 1; j < BLOCKSIZE-1; j++) {
				mapVec[(iterY * BLOCKSIZE) + j][(iterX * BLOCKSIZE) + 0] = ((64 * num) + j);
				mapVec[(iterY * BLOCKSIZE) + j][(iterX * BLOCKSIZE) + 64-1] = ((64 * num) + j);
			}

			//bottom
			for (int i = 0; i < BLOCKSIZE; i++) {
				mapVec[iterY * BLOCKSIZE + 64-1][iterX * BLOCKSIZE + i] = ((64 * num) + i);
			}

			break;
		case '&':
			//set player x to mapVec[iterX*64 + 32]
			playerX = iterX * BLOCKSIZE + BLOCKSIZE/2;
			playerY = iterY * BLOCKSIZE + BLOCKSIZE/2;
			for (int i = 0; i < BLOCKSIZE; i++) {
				for (int j = 0; j < 64; j++) {
					mapVec[iterY * BLOCKSIZE + i][iterX * BLOCKSIZE + j] = 0;
				}
			}
			break;
			/*cout << playerX << ", " << playerY << endl;*/
			break;
		case ' ':
			cout << "NEW Y" << endl;
			iterY += 1;
			iterX = 0;
			break;
		default: //0
			for (int i = 0; i < BLOCKSIZE; i++) {
				for (int j = 0; j < 64; j++) {
					mapVec[iterY * BLOCKSIZE + i][iterX * BLOCKSIZE + j] = 0;
				}
			}
			break;

		}

		iterX += 1;
	}

	cout << "DONE" << endl;
	cout << mapVec[0][64] << endl;

	//for (auto& rect : mapRects) {
	//	int x = rect.x;
	//	int y = rect.y;
	//	int w = rect.w;
	//	int h = rect.h;

	cout << "MAP BUILT" << endl;

	SDL_Color colors[7]{
		SDL_Color{255, 255, 255, 255},
		SDL_Color{ 255, 0, 0, 255 },
		SDL_Color{ 0, 255, 0, 255 },
		SDL_Color{ 0, 0, 255, 255 },
		SDL_Color{255, 255, 0, 255},
		SDL_Color{80, 0, 255, 255},
		SDL_Color{0, 255, 255, 255}
	};
	cout << colors[6].r << endl;

	//playersetup
	double pX = 0;
	double pY = 0;
	int rayLen = 0;
	bool TURNLEFT = false;
	bool TURNRIGHT = false;
	bool FORWARDS = false;
	bool BACKWARDS = false;
	bool LSTRAFE = false;
	bool RSTRAFE = false;
	double rayAngle = NULL;
	double rayIncrement = NULL;
	SDL_Color col = colors[0];
	double rayLenCos = 0;
	int tileIndex = 0;
	double shade;
	SDL_Rect r; //stretch rect
	SDL_Rect cr; //clip rect
	cr.x = 0;
	cr.y = 0;
	cr.w = 1;
	cr.h = 64;


	while (running) {
		//SDL_FillRect(windowSurface, NULL, (0, 0, 0, 255));
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		cout << SDL_GetError() << endl;
		SDL_RenderClear(renderer);

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_TAB:
					cout << playerAngle << endl;
					break;
				case SDLK_LEFT:
					/*cout << "TURN LEFT" << endl;*/
					TURNLEFT = true;
					break;
				case SDLK_RIGHT:
					/*cout << "TURN RIGHT" << endl;*/
					TURNRIGHT = true;
					break;
				case SDLK_w:
					FORWARDS = true;
					break;
				case SDLK_s:
					BACKWARDS = true;
					break;
				case SDLK_a:
					LSTRAFE = true;
					break;
				case SDLK_d:
					RSTRAFE = true;
					break;
				}
			}
			else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					TURNLEFT = false;
					break;
				case SDLK_RIGHT:
					TURNRIGHT = false;
					break;
				case SDLK_w:
					FORWARDS = false;
					break;
				case SDLK_s:
					BACKWARDS = false;
					break;
				case SDLK_a:
					LSTRAFE = false;
					break;
				case SDLK_d:
					RSTRAFE = false;
					break;
				}
			}
		}

		if (TURNLEFT) {
			playerAngle += 3 * M_PI / 180;
			if (playerAngle > 2 * M_PI) {
				playerAngle -= 2 * M_PI;
			}
		}
		if (TURNRIGHT) {
			playerAngle -= 3 * M_PI/ 180;
			if (playerAngle < 0) {
				playerAngle += 2 * M_PI;
			}
		}
		if (FORWARDS) {
			playerX += cos(playerAngle)*1.8;
			playerY += sin(playerAngle)*1.8;
		}
		if (BACKWARDS) {
			playerX -= cos(playerAngle) * 1.2;
			playerY -= sin(playerAngle) * 1.2;
			}
		if (LSTRAFE) {
			playerX -= cos(playerAngle - M_PI/2) * 1.2;
			playerY -= sin(playerAngle - M_PI / 2) * 1.2;
		}
		if (RSTRAFE) {
			playerX -= cos(playerAngle + M_PI / 2) * 1.2;
			playerY -= sin(playerAngle + M_PI / 2) * 1.2;
		}
		/*cout << playerX << endl;*/
		rayAngle = playerAngle + (FOV/2);
		rayIncrement = FOV / WIDTH; // subtract rayIncrement from rayAngle each iteration
		for (int screenX = 0; screenX < WIDTH; screenX++) {
			rayLen = 0;
			tileIndex = 0;
			pX = playerX;
			pY = playerY;
			while (rayLen <= VIEWDIST) {
				pX += cos(rayAngle);
				pY += sin(rayAngle);

				if (pX > 0 && pX < MAPWIDTH - 1 && pY > 0 && pY < MAPHEIGHT - 1) { // check if p is in range
					if (mapVec[(int)pY][(int)pX] > 0) { // if we hit a wall (or anything), stop loop
						tileIndex = mapVec[(int)pY][(int)pX];
						/*cout << tileIndex << endl;*/
						break;
					}
				}
				else { // draw border textures
					if (pX > 0 && pX < MAPWIDTH - 1) {
						tileIndex = (int)pX % 64;
					}
					else {
						tileIndex = (int)pY % 64;
					}
					break;
				}
				rayLen += 1;
			}

			if (rayLen < VIEWDIST) {
				rayLenCos = rayLen * (cos(rayAngle-playerAngle));
				r.x = screenX;
				r.w = 1;
				r.h = (int)((64/rayLenCos)*PLANEDIST);
				r.y = (int)(HEIGHT / 2) - (r.h / 2);
				
				//col = colors[tileIndex];
				//shade = ((VIEWDIST - rayLenCos) / VIEWDIST);
				//SDL_SetRenderDrawColor(renderer, (int)col.r*shade, (int)col.g*shade, (int)col.b*shade, 0);
				//SDL_RenderFillRect(renderer, &r);
				cr.x = tileIndex;
				SDL_RenderCopy(renderer, tileTexture, &cr, &r);
			}

			//increment rayAngle
			rayAngle -= rayIncrement;
		}

		SDL_RenderPresent(renderer);
		/*SDL_UpdateWindowSurface(window);*/

		//SDL_SetRenderDrawColor(renderer, rColor.r, rColor.g, rColor.b, 255);
		SDL_RenderFillRect(renderer, &r); //draw filled rect r
		SDL_Delay(20);
	}
	IMG_Quit();
	SDL_Quit();
	return 0;
}