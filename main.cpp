#include <SFML/Graphics.hpp>
#include <SFML\Audio.hpp>
#include <Windows.h>
#include "Player.h"
#include "Bullet.h"
#include "background.h"
#include <vector>
#include <sstream>
#include "Menu.h"
#include <cstdlib>

using namespace sf;
using namespace std;

static const float VIEW_HEIGHT = 864.0f;
static const float VIEW_WIDHT = 564.0f;

float detalTime = 0.0f;       // ตั้งค่าความเร็วให้ตัวละครเดิน 
float totaltime = 0;		  // ตั้งค่าความเร็วให้ตัวละครเดิน 	
float speed_player = 150.0f;  // ตั้งค่าความเร็วให้ตัวละครเดิน
int   animationFrame = 0; // แฟรมของตัวละคร ที่ 0 


///////////////////////////////// VIEW //////////////////////////////////////////

//ฟังชั่น ในการเซ้ทค่า display ให้คงที่ตลอดหลังจากปรับขยายหรือหดหน้า
void ResizeView(const sf::RenderWindow& window, sf::View& view)
{
	// aspectRatio คือการเซ้ทค่าของหน้าจอให้คงที่ตลอดหลังจากมีการขยายหน้าจอ เป็นต้น
	float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(VIEW_HEIGHT * aspectRatio, VIEW_WIDHT * aspectRatio);  // การให้มันคงที่ค่าในแกน x ภาพหน้าจะเหมือนเดิม
}

///////////////////////////////// MAIN /////////////////////////////////////////
int main()
{	
	int  hit = 0, hit_again = 0;  // rand HP
	int  Scorecurrent = 0;

	sf::Font word;
	if (!word.loadFromFile("font/Jelly Crazies.ttf")); {

	}
	
	sf::Text score;
	score.setFont(word);
	score.setCharacterSize(10);
	score.setStyle(sf::Text::Regular);
	score.setPosition(400, 50);
	score.setString("SCORE . ");

	sf::Text scoreCurrent;
	scoreCurrent.setFont(word);
	scoreCurrent.setCharacterSize(10);
	scoreCurrent.setStyle(sf::Text::Regular);
	scoreCurrent.setPosition(500, 50);
	

	///////////////////////////////////////////////////////////////////////////
	int screen = 0; // หน้าจอตอนให้ตัวละครเดินขึ้นมา
	sf::Clock clock;
	srand(time(NULL));

	//////////////////////// DISPLAY ,  ICON GANE ///////////////////////////////////////
	
	sf::RenderWindow window(sf::VideoMode(564.0f, 864.0f), "student N I N J A" );  //ขนาดหน้าจอ "ชื่อหัวเกม"
	sf::Image icon;
		if (!icon.loadFromFile("./sprite/ninjaweaponRED.png"))
			{
				return -1;
			}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());  // เซ้ท icon เกม

	///////////////////////////////// MENU ///////////////////////////////////////

	Menu menu(window.getSize().x,window.getSize().y);

	///////////////////////////////// VIEW //////////////////////////////////////////
	sf::View viewCharacter;
	viewCharacter.setSize(sf::Vector2f(564.0f, 864.0f));
	viewCharacter.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));   // ให้พอดีขนาดหน้าจอ
	//viewCharacter.zoom(1.2f);  // ขยายภาพพื้นหลัง
	//sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(564.0f, 864.0f));   // ทำให้การปรับหน้าจอภาพให้มีขนาดตงที่

	/////////////////////////////// BACKGROUND /////////////////////////////////////

	// ลองใส่พื้นหลัง
	//sf::RectangleShape background(sf::Vector2f(564.0f, 864.0f)); // ให้รูปใหญ่เท่าไหร่ ก็เลือกขนาดเอา
	sf::Texture backgroundTexture[2];
	backgroundTexture[0].loadFromFile("./sprite/background_game.png");
	//backgroundTexture[1].loadFromFile("./sprite/background_game2.jpg");
	std::vector<Background>backgrounds;
	backgrounds.push_back(Background(&backgroundTexture[0], 110.0f));
	//backgrounds.push_back(Background(&backgroundTexture[1], 80.0f));


	///////////////////////////////// ITEM //////////////////////////////////////////
	// Item
	sf::RectangleShape Heart(sf::Vector2f(42.0f, 43.0f));
	sf::Texture Heartlove;
	Heartlove.loadFromFile("./sprite/Heart.png");
	Heart.setTexture(&Heartlove);
	Heart.setPosition({ 270,340 });
	//Heart.setOrigin(Heart.getSize() / 2.0f);

	///////////////////////////////// HP //////////////////////////////////////////
	sf::RectangleShape HP(sf::Vector2f(230.0f, 70.0f));
	sf::Texture HPplayer;
	HPplayer.loadFromFile("sprite/HP/HP_1_100p.png");
	HP.setTexture(&HPplayer);
	HP.setPosition({ 50,30 });
	
	///////////////////////////////// SHAPE //////////////////////////////////////////
	// เป็นของวงกลม
	sf::CircleShape shape(70,5);  // (ขนาด,มุมที่ให้เกิด เช่นอันนี้เป็น 5 มุม)
	shape.setPosition({ 450,400 });  // ตำแหน่งวงกลม (x,y)
	shape.setFillColor(sf::Color::White);  //สีของรูป
	
	///////////////////////////////// PLAYER //////////////////////////////////////////
	
	sf::Texture playerTexture;													
	playerTexture.loadFromFile("sprite/playermainful.png");
	// animation ใน class		//การแบ่งสโคปของรูปภาพ (4,4)	//ความเร็วในการรันเฟรม (0.5f)	// ความเร็วของตัวละคร (100.0f)
	Player player(&playerTexture, sf::Vector2u(4, 6), 0.2f, 150.0f);

	///////////////////////////////// ENEMY //////////////////////////////////////////
	//v.1  white
	sf::Vector2f playerCenter;  // เอามาจาก code ด้านล่าง
	sf::RectangleShape enemy(sf::Vector2f(120.0f, 100.0f));  // ของ monepic
	sf::Texture enemies_photo;
	enemies_photo.loadFromFile("sprite/monepicWHITE.png");
	enemy.setTexture(&enemies_photo);
	enemy.setPosition({200,100});
	
	std::vector<RectangleShape> enemies;
	enemies.push_back(RectangleShape(enemy));

	// v.2  red
	sf::RectangleShape enemy2(sf::Vector2f(120.0f, 100.0f));  // ของ monepic
	sf::Texture enemies_photo2;
	enemies_photo2.loadFromFile("sprite/monepicRED.png");
	enemy2.setTexture(&enemies_photo2);
	enemy2.setPosition({ 200,100 });

	std::vector<RectangleShape> enemies2;
	enemies2.push_back(RectangleShape(enemy2));

	int enemySpawnTimer = 0;
	///////////////////////////////// SHOT //////////////////////////////////////////

	sf::Clock shotclock;   // delay กระสุน
	float shotDelay = shotclock.getElapsedTime().asSeconds();
	sf::RectangleShape shoot(sf::Vector2f(30.0f, 30.0f));  // ขนาดกระสุน
	sf::Texture shootz_photo;
	shootz_photo.loadFromFile("sprite/ninjaweaponWHITE.png");
	shoot.setTexture(&shootz_photo);
	std::vector<RectangleShape> shoots;
	shoots.push_back(RectangleShape(shoot));

	///////////////////////////////// SOUND //////////////////////////////////////////
	
	sf::Music start_sound; // เพลงเริ่มเกม
	if (!start_sound.openFromFile("sound/songstart.ogg"))
	{
		printf("ERROR\n");
	}
	start_sound.play(); //เปิดเพลง

	///////////////////////////////// SHOT SOUND //////////////////////////////////////////

	sf::SoundBuffer soundeffect;
	soundeffect.loadFromFile("sound/Soundshot.wav");
	sf::Sound shooteffect;
	shooteffect.setBuffer(soundeffect);

	///////////////////////////////// pickup SOUND //////////////////////////////////////////

	sf::SoundBuffer soundpick;
	soundpick.loadFromFile("sound/pickup_item.wav");
	sf::Sound Pickupsound;
	Pickupsound.setBuffer(soundpick);


	//******************************************************************************

	while (window.isOpen())
	{
		shotDelay = shotclock.getElapsedTime().asSeconds();     
		window.setFramerateLimit(60);
		
		
			detalTime = clock.restart().asSeconds(); // คำสั่งหน่วงให้น้องเดินช้า
			
			sf::Event evnt;
			while (window.pollEvent(evnt))	// while นี้ทำให้สามารถใช้เม้ากดเต็มจอหรือใช้ขยาย กว้างยาวได้
			{
				switch (evnt.type)
				{

	   /////////////////////////////  MENU //////////////////////////////////////////

				case sf::Event::KeyReleased:
					switch (evnt.key.code)
					{
					
					case sf::Keyboard::Up:
						menu.MoveUp();
						break;

					case sf::Keyboard::Down:
						menu.MoveDown();
						break;

					case sf::Keyboard::Return:
						switch (menu.GetPressedItem())
						{
						case 0:    // การรับค่าหน้า menu เข้ามา 
					
							printf("Play button has been pressed\n");			
							break;

						case 1:
							printf("Options button has been pressed\n");
							break;

						case 2:  // [2] array ที่ 2 จะให้ออกจากการทำงานทั้งหมด
							window.close();
							break;
						}
					}

					break;

				case sf::Event::Closed:

					window.close();
					break;

				case sf::Event::Resized:

					ResizeView(window, viewCharacter);
					break;

				}
			}

			///////////////////////////////// LOOP BG //////////////////////////////////////////	

			for (Background& background : backgrounds) {
				background.Update(detalTime);
			}

			for (Background& background : backgrounds) {   // ลูปให้รูปวน 
				background.Draw(window);
			}

			/// ///////////////////////////////////////////////////////////////////////////////

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				window.close();  // กด esc เพื่อออกจากโปรแกรม
			}

			////////////////////////// Check Item //////////////////////////////////////////

			/*if (player.GetGlobal().intersects(Heart.getGlobalBounds()))  // ทำเช็คชนกับตัวไอเท็ม
			{
			
				if (hit_again == 1) {   // เซ้ทการชนแล้วเลือดค่อยๆลด
					HPplayer.loadFromFile("sprite/HP/HP_1_100p.png");

				}

				if (hit_again == 2) {
					HPplayer.loadFromFile("sprite/HP/HP_2_70p.png");

				}

				if (hit_again == 3) {
					HPplayer.loadFromFile("sprite/HP/HP_3_50p.png");

				}

			}*/

			/////////////////////////////  20/11/20  Update shoot //////////////////////////////////////////

			playerCenter = sf::Vector2f(player.getPosition().x - 15, player.getPosition().y + 10); // ตำแหน่งกระสุนออกจากตัว

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) & shotDelay > 0.25)
			{
				shoot.setPosition(playerCenter);
				shoots.push_back(RectangleShape(shoot));
				shotDelay = shotclock.restart().asSeconds();
				shooteffect.play();

			}

			for (size_t i = 1; i < shoots.size(); i++)
			{
				shoots[i].move(0.0f, -10.0f);

				if (shoots[i].getPosition().y <= 0)
				{
					shoots.erase(shoots.begin()+i);
				}
			}


			/////////////////////////////  spawn monster  //////////////////////////////////////////
			//v.1  mon_white
			if (Scorecurrent <= 50) {  // คะแนนไม่เกิน 150 ให้มอนปกติออกมา 

				if (enemySpawnTimer < 30)   // การ spawn mon  (กำหนดลิมิตการสปอน)
				{
					enemySpawnTimer++;
				}


				int rand_x = (rand() % int(window.getSize().x - enemy.getSize().x));

				if (enemySpawnTimer >= 70)
				{
					enemy.setPosition(Vector2f(rand_x, -20.0f));
					enemies.push_back(RectangleShape(enemy));
					enemySpawnTimer = 0;
				}

				enemySpawnTimer++;
			}

				for (size_t i = 1; i < enemies.size(); i++)
				{
					enemies[i].move(0.0f, 1.5f);

					if (enemies[i].getPosition().y > window.getSize().y)  // สปอน มอนให้เกิด
					{
						enemies.erase(enemies.begin() + i);

						if (hit == 0) {
							hit++;
							hit_again += hit;
							HPplayer.loadFromFile("sprite/HP/HP_2_70p.png");
						}

						if (hit_again == 2) {   // เซ้ทการชนแล้วเลือดค่อยๆลด
							HPplayer.loadFromFile("sprite/HP/HP_3_50p.png");
						}

						if (hit_again == 3) {
							HPplayer.loadFromFile("sprite/HP/HP_4_30p.png");
						}

						if (hit_again == 4) {
							HPplayer.loadFromFile("sprite/HP/HP_5_0p.png");
							// ใส่คำว่า GAME OVER

						}

					}


				/*	if (player.GetGlobal().intersects(enemies[i].getGlobalBounds()))
					{

						if (hit == 0) {
							hit++;
							hit_again += hit;
							HPplayer.loadFromFile("sprite/HP/HP_2_70p.png");

						}

						if (hit_again == 2) {   // เซ้ทการชนแล้วเลือดค่อยๆลด
							HPplayer.loadFromFile("sprite/HP/HP_3_50p.png");

						}

						if (hit_again == 3) {
							HPplayer.loadFromFile("sprite/HP/HP_4_30p.png");

						}

						if (hit_again == 4) {
							HPplayer.loadFromFile("sprite/HP/HP_5_0p.png");
							// ใส่คำว่า GAME OVER

						}

					}*/

					window.draw(enemies[i]);     // วาดศัตรุออกมา
				}


				//v.2   mon_red
				if (Scorecurrent >= 50) {  // คะแนนไม่เกิน 150 ให้มอนปกติออกมา 

					if (enemySpawnTimer < 40)   // การ spawn mon  (กำหนดลิมิตการสปอน)
					{
						enemySpawnTimer++;
					}


					int rand_x = (rand() % int(window.getSize().x - enemy2.getSize().x));

					if (enemySpawnTimer >= 70)
					{
						enemy2.setPosition(Vector2f(rand_x, -20.0f));   
						enemies2.push_back(RectangleShape(enemy2));
						enemySpawnTimer = 0;
					}

					enemySpawnTimer++;
				}

				for (size_t i = 1; i < enemies2.size(); i++)
				{
					if (Scorecurrent >= 50 ) { enemies2[i].move(0.0f, 1.8f); } // ปรับให้เร็วขึ้น
					
					if (enemies2[i].getPosition().y > window.getSize().y)  // สปอน มอนให้เกิด
					{
						enemies2.erase(enemies2.begin() + i);

						if (hit == 0) {
							hit++;
							hit_again += hit;
							HPplayer.loadFromFile("sprite/HP/HP_2_70p.png");
						}

						if (hit_again == 2) {   // เซ้ทการชนแล้วเลือดค่อยๆลด
							HPplayer.loadFromFile("sprite/HP/HP_3_50p.png");
						}

						if (hit_again == 3) {
							HPplayer.loadFromFile("sprite/HP/HP_4_30p.png");
						}

						if (hit_again == 4) {
							HPplayer.loadFromFile("sprite/HP/HP_5_0p.png");
							// ใส่คำว่า GAME OVER

						}
					}

				/*	if (player.GetGlobal().intersects(enemies2[i].getGlobalBounds()))
					{

						if (hit == 0) {
							hit++;
							hit_again += hit;
							HPplayer.loadFromFile("sprite/HP/HP_2_70p.png");

						}

						if (hit_again == 2) {   // เซ้ทการชนแล้วเลือดค่อยๆลด
							HPplayer.loadFromFile("sprite/HP/HP_3_50p.png");

						}

						if (hit_again == 3) {
							HPplayer.loadFromFile("sprite/HP/HP_4_30p.png");

						}

						if (hit_again == 4) {
							HPplayer.loadFromFile("sprite/HP/HP_5_0p.png");
							// ใส่คำว่า GAME OVER


						}

					}  */


					window.draw(enemies2[i]);     // วาดศัตรุออกมา
				}


				for (size_t i = 1; i < shoots.size(); i++)
				{
					window.draw(shoots[i]);   // วาดกระสุนออกมาให้ยิง
				}


			//////////////////// collision mon - shoot //////////////////////////////////////////

			//v.1   mon_white
			for (size_t i = 1; i < shoots.size(); i++)
			{
				for (size_t k = 1; k < enemies.size(); k++)
				{

					if (shoots[i].getGlobalBounds().intersects(enemies[k].getGlobalBounds()))
					{
						Pickupsound.play();
						shoots.erase(shoots.begin() + i);
						enemies.erase(enemies.begin() + k);
						Scorecurrent += 5;
						
						break;
					}
							
				}
			}
			

			//v.2  mon_red
			for (size_t i = 1; i < shoots.size(); i++)
			{
				for (size_t k = 1; k < enemies2.size(); k++)
				{					

					if (shoots[i].getGlobalBounds().intersects(enemies2[k].getGlobalBounds()))
					{
						Pickupsound.play();
						shoots.erase(shoots.begin() + i);
						enemies2.erase(enemies2.begin() + k);
						Scorecurrent += 10;

						break;
					}

				}
			}
			hit = 0;  // เซ้ทค่าตอนอมนชนจอด้านล่างแล้วให้ครั้งที่ชนกลับเป็น 0 เพื่อเข้า ไปเช็คเงื่อนไขใหม่
			/////////////////////////////  DRAW //////////////////////////////////////////

			stringstream scoreShow;
			scoreShow << Scorecurrent;
			scoreCurrent.setString(scoreShow.str().c_str());    // วาด คะแนนตอนยิงได้
				
			window.draw(score);
			window.draw(scoreCurrent);
			menu.draw(window);
			window.draw(Heart);
			window.draw(HP);
			player.Draw(window);	
			player.Update(detalTime);

			window.display();
			window.clear(sf::Color::Transparent);
			
		}
		
	return 0;

}



