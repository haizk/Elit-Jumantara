#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

class Object : public sf::Sprite {
public:
    Object();
    Object(float veloX, float veloY, int x);
    void setHitbox(const sf::FloatRect& hitbox);
    sf::FloatRect getGlobalHitbox() const;
    void setHealth(int x);
    int getHealth();
    void moves(float deltaTime);
private:
    sf::FloatRect m_hitbox;
    int health;
    float velocityX, velocityY;
};

sf::Vector2f direction;
sf::Vector2f currentVelocity(0.0f, 0.0f);
std::vector<sf::RectangleShape> bullets;
std::vector<Object> enemies;

float maxVelocity = 10.0f;
float acceleration = 1.5f;
float drag = 0.5f;
float multiplier = 30.0f;
float shootTimer = 0;
float waitTimer = 0;
float prePlayTimer = 0;
float textCounter = 0;
float frames = 0;
bool play = false;
bool lose = false;
bool prePlay = false;
bool enemyCheck[5];

void control(Object& player, sf::Text& healthText, float deltaTime, sf::RenderWindow& window);
void waiting(Object& player, sf::Text& playText, float deltaTime, sf::RenderWindow& window);
void enemyControl(sf::Texture& enemyTexture, float deltaTime, sf::RenderWindow& window);
void enemySpawner1(sf::Texture& enemyTexture);
void enemySpawner2(sf::Texture& enemyTexture);
void enemySpawner3(sf::Texture& enemyTexture);
void enemySpawner4(sf::Texture& enemyTexture);
void congratulation(sf::Text& playText, float deltaTime, sf::RenderWindow& window);
std::string to_stringX(int x);

int main() {
    sf::Music musik;
    musik.openFromFile("Resources/Music/BeepBox-Song.wav");
    musik.play();
    musik.setLoop(true);
    
    sf::RenderWindow window(sf::VideoMode(1500, 500), "Elit Jumantara", sf::Style::Close);

    sf::Clock clock;
    float deltaTime = 0, seconds = 0;

    sf::Font font;
    font.loadFromFile("Resources/Font/Haizk.ttf");

    sf::Text playText("HAIZK!", font, 100), healthText("030", font, 20);
    playText.setFillColor(sf::Color(222, 222, 222));
    playText.setOutlineColor(sf::Color(0, 0, 0));
    healthText.setFillColor(sf::Color(222, 222, 222));
    healthText.setOutlineColor(sf::Color(0, 0, 0));
    healthText.setPosition(10, 5);

    sf::Texture playerTexture, bgTexture[4], enemyTexture;
    playerTexture.loadFromFile("Resources/Images/player.png");
    bgTexture[0].loadFromFile("Resources/Images/bg1.png");
    bgTexture[1].loadFromFile("Resources/Images/bg2.png");
    bgTexture[2].loadFromFile("Resources/Images/bg3.png");
    bgTexture[3].loadFromFile("Resources/Images/bg4.png");
    enemyTexture.loadFromFile("Resources/Images/enemy.png");

    Object player, bg;
    player.setTexture(playerTexture);
    player.setTextureRect(sf::IntRect(13, 19, 68, 62));
    player.setHitbox(sf::FloatRect(0, 6, 68, 50));
    player.setPosition(10, 219);
    player.setColor(sf::Color(255, 255, 255, 0));
    bg.setColor(sf::Color(150, 150, 150));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

        deltaTime = clock.restart().asSeconds();
        bg.setTexture(bgTexture[(int)std::floor(seconds += deltaTime) % 4]);

        window.clear();

        window.draw(bg);
        if (play) {
            frames += deltaTime;
            control(player, healthText, deltaTime, window);
            if (frames <= 28)
                enemyControl(enemyTexture, deltaTime, window);
            else
                congratulation(playText, deltaTime, window);
        }
        else {
            waiting(player, playText, deltaTime, window);
        }

        window.display();
    }
    return 0;
}

void control(Object& player, sf::Text& healthText, float deltaTime, sf::RenderWindow& window) {
    //Draw player's health
    healthText.setString("HEALTH: " + to_stringX((int)player.getHealth()));
    window.draw(healthText);

    //Check player's health
    if (player.getHealth() <= 0) {
        enemies.clear();
        bullets.clear();
        play = false;
        lose = true;
        return;
    }

    //Draw player
    window.draw(player);

    //Direction check
    direction = sf::Vector2f(((float)sf::Keyboard::isKeyPressed(sf::Keyboard::D) - (float)sf::Keyboard::isKeyPressed(sf::Keyboard::A)), ((float)sf::Keyboard::isKeyPressed(sf::Keyboard::S) - (float)sf::Keyboard::isKeyPressed(sf::Keyboard::W)));

    //Velocity change
    if (currentVelocity.x > -maxVelocity && currentVelocity.x < maxVelocity)
        currentVelocity.x += acceleration * direction.x * deltaTime * multiplier;
    if (currentVelocity.y > -maxVelocity && currentVelocity.y < maxVelocity)
        currentVelocity.y += acceleration * direction.y * deltaTime * multiplier;

    //Drag till stop
    if (currentVelocity.x > 0.0f) {
        currentVelocity.x -= drag * deltaTime * multiplier;
        if (currentVelocity.x < 0.0f)
            currentVelocity.x = 0.0f;
    }
    else if (currentVelocity.x < 0.0f) {
        currentVelocity.x += drag * deltaTime * multiplier;
        if (currentVelocity.x > 0.0f)
            currentVelocity.x = 0.0f;
    }
    if (currentVelocity.y > 0.0f) {
        currentVelocity.y -= drag * deltaTime * multiplier;
        if (currentVelocity.y < 0.0f)
            currentVelocity.y = 0.0f;
    }
    else if (currentVelocity.y < 0.0f) {
        currentVelocity.y += drag * deltaTime * multiplier;
        if (currentVelocity.y > 0.0f)
            currentVelocity.y = 0.0f;
    }

    //Player mover
    if (player.getPosition().x + (currentVelocity.x * deltaTime * multiplier) >= -10 && player.getPosition().x + (currentVelocity.x * deltaTime * multiplier) <= 1440)
        player.move(currentVelocity.x * deltaTime * multiplier, 0);
    else
        currentVelocity.x = 0;
    if (player.getPosition().y + (currentVelocity.y * deltaTime * multiplier) >= -5 && player.getPosition().y + (currentVelocity.y * deltaTime * multiplier) <= 443)
        player.move(0, currentVelocity.y * deltaTime * multiplier);
    else
        currentVelocity.y = 0;

    //Player sprite control
    if (currentVelocity.y > 0) {
        if (currentVelocity.y >= maxVelocity - 3)
            player.setTextureRect(sf::IntRect(14, 213, 68, 62));
        else
            player.setTextureRect(sf::IntRect(12, 115, 68, 62));
    }
    else if (currentVelocity.y < 0) {
        if (currentVelocity.y <= -maxVelocity + 3)
            player.setTextureRect(sf::IntRect(211, 126, 68, 62));
        else
            player.setTextureRect(sf::IntRect(112, 123, 68, 62));
    }
    else
        player.setTextureRect(sf::IntRect(13, 19, 68, 62));

    //Shoot
    shootTimer += deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shootTimer >= 0.3f) {
        shootTimer = 0;

        sf::RectangleShape bullet;
        bullet.setSize(sf::Vector2f(20, 4));
        bullet.setFillColor(sf::Color(222, 222, 222));
        bullet.setOutlineColor(sf::Color(0, 0, 0));

        //Bullet position depends on player's sprite
        if (currentVelocity.y < 0) {
            if (currentVelocity.y <= -maxVelocity + 3)
                bullet.setPosition(player.getGlobalHitbox().left + 60, player.getGlobalHitbox().top + -6);
            else
                bullet.setPosition(player.getGlobalHitbox().left + 60, player.getGlobalHitbox().top + 10);
        }
        else if (currentVelocity.y > 0) {
            if (currentVelocity.y >= maxVelocity - 3)
                bullet.setPosition(player.getGlobalHitbox().left + 60, player.getGlobalHitbox().top + 52);
            else
                bullet.setPosition(player.getGlobalHitbox().left + 60, player.getGlobalHitbox().top + 36);
        }
        else
            bullet.setPosition(player.getGlobalHitbox().left + 60, player.getGlobalHitbox().top + 23);

        bullets.push_back(bullet);
    }

    //Draw bullets
    for (std::vector<sf::RectangleShape>::iterator ite = bullets.begin(); ite != bullets.end(); ite++) {
        window.draw(*ite);
        (*ite).move(50 * deltaTime * multiplier, 0);

        if ((*ite).getPosition().x >= 1500) {
            ite = bullets.erase(ite);
            if (ite == bullets.end())
                break;
        }
    }

    //Player collision
    for (std::vector<Object>::iterator ite = enemies.begin(); ite != enemies.end(); ite++) {
        if (player.getGlobalHitbox().intersects((*ite).getGlobalBounds())) {
            player.setHealth(player.getHealth() - 1);
            (*ite).setHealth((*ite).getHealth() - 1);
        }
    }
}

void waiting(Object& player, sf::Text& playText, float deltaTime, sf::RenderWindow& window) {
    waitTimer += deltaTime;

    //Display press to play text
    if (!lose) {
        if (!prePlay) {
            playText.setCharacterSize(100);
            playText.setString("PRESS ENTER TO PLAY!");
            playText.setPosition(sf::Vector2f(190, 184));
            if (waitTimer >= 0.2) {
                window.draw(playText);
                if (waitTimer >= 0.8)
                    waitTimer = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                player.setPosition(10, 219);
                player.setTextureRect(sf::IntRect(13, 19, 68, 62));
                currentVelocity = direction = sf::Vector2f(0, 0);
                prePlay = true;
                prePlayTimer = 0;
                waitTimer = 0;
            }
        }

        //Fade effect
        else {
            window.draw(player);
            if (waitTimer >= 0.2) {
                prePlayTimer += 1;
                waitTimer = 0;
            }
            switch ((int)prePlayTimer) {
            case 0:
                player.setColor(sf::Color(255, 255, 255, 50));
                break;
            case 1:
                player.setColor(sf::Color(255, 255, 255, 100));
                break;
            case 2:
                player.setColor(sf::Color(255, 255, 255, 150));
                break;
            case 3:
                player.setColor(sf::Color(255, 255, 255, 200));
                break;
            case 4:
                player.setColor(sf::Color(255, 255, 255, 255));
                player.setHealth(5);
                play = true;
                prePlay = false;
                frames = 0;
                for (int x = 0; x < 5; x++)
                    enemyCheck[x] = false;
                break;
            default:
                break;
            }
        }
    }

    //Display lose text
    else {
        playText.setCharacterSize(100);
        playText.setString("YOU LOSE!");
        playText.setPosition(sf::Vector2f(505, 184));
        if (waitTimer >= 0.2) {
            window.draw(playText);
            if (waitTimer >= 0.8) {
                waitTimer = 0;
                textCounter++;
            }
        }
        if (textCounter >= 3) {
            lose = false;
            textCounter = 0;
            waitTimer = 0;
        }
    }
}

void enemyControl(sf::Texture& enemyTexture, float deltaTime, sf::RenderWindow& window) {
    //Enemy-bullet collision
    for (std::vector<Object>::iterator iteE = enemies.begin(); iteE != enemies.end(); iteE++) {
        for (std::vector<sf::RectangleShape>::iterator iteB = bullets.begin(); iteB != bullets.end(); iteB++) {
            if ((*iteE).getGlobalBounds().intersects((*iteB).getGlobalBounds())) {
                (*iteE).setHealth((*iteE).getHealth() - 1);
                iteB = bullets.erase(iteB);
                if (iteB == bullets.end())
                    break;
            }
        }
    }

    //Enemy's behavior
    for (std::vector<Object>::iterator ite = enemies.begin(); ite != enemies.end(); ite++) {
        window.draw((*ite));
        (*ite).moves(deltaTime);

        if ((*ite).getPosition().x <= -200 || (*ite).getPosition().x >= 1700 || (*ite).getPosition().y <= -200 || (*ite).getPosition().y >= 700 || (*ite).getHealth() <= 0) {
            ite = enemies.erase(ite);
            if (ite == enemies.end())
                break;
        }
    }

    //Enemy spawner
    enemySpawner1(enemyTexture);
}

void enemySpawner1(sf::Texture& enemyTexture) {
    if ((int)std::floor(frames) == 1 && !enemyCheck[0]) {
        enemyCheck[0] = true;

        Object enemy1(-5, 0, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 10);
        enemies.push_back(enemy1);

        Object enemy2(-5, 0, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 60);
        enemies.push_back(enemy2);

        Object enemy3(-5, 0, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 110);
        enemies.push_back(enemy3);

        Object enemy4(-5, 0, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 160);
        enemies.push_back(enemy4);

        Object enemy5(-5, 0, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 210);
        enemies.push_back(enemy5);

        Object enemy6(-5, 0, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 260);
        enemies.push_back(enemy6);

        Object enemy7(-5, 0, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 310);
        enemies.push_back(enemy7);

        Object enemy8(-5, 0, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 360);
        enemies.push_back(enemy8);

        Object enemy9(-5, 0, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 410);
        enemies.push_back(enemy9);

        Object enemy0(-5, 0, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(1600, 460);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 2 && !enemyCheck[1]) {
        enemyCheck[1] = true;

        Object enemy1(-5, 0, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 10);
        enemies.push_back(enemy1);

        Object enemy2(-5, 0, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 60);
        enemies.push_back(enemy2);

        Object enemy3(-5, 0, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 110);
        enemies.push_back(enemy3);

        Object enemy4(-5, 0, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 160);
        enemies.push_back(enemy4);

        Object enemy5(-5, 0, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 210);
        enemies.push_back(enemy5);

        Object enemy6(-5, 0, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 260);
        enemies.push_back(enemy6);

        Object enemy7(-5, 0, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 310);
        enemies.push_back(enemy7);

        Object enemy8(-5, 0, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 360);
        enemies.push_back(enemy8);

        Object enemy9(-5, 0, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 410);
        enemies.push_back(enemy9);

        Object enemy0(-5, 0, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(1600, 460);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 5 && !enemyCheck[2]) {
        enemyCheck[2] = true;

        Object enemy1(-5, 3, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1000, -100);
        enemies.push_back(enemy1);

        Object enemy2(-5, 3, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(930, -100);
        enemies.push_back(enemy2);

        Object enemy3(-5, 3, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(860, -100);
        enemies.push_back(enemy3);

        Object enemy4(-5, 3, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(790, -100);
        enemies.push_back(enemy4);

        Object enemy5(-5, 3, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(720, -100);
        enemies.push_back(enemy5);

        Object enemy6(-5, 3, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(650, -100);
        enemies.push_back(enemy6);

        Object enemy7(-5, 3, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(580, -100);
        enemies.push_back(enemy7);

        Object enemy8(-5, 3, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(510, -100);
        enemies.push_back(enemy8);

        Object enemy9(-5, 3, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(440, -100);
        enemies.push_back(enemy9);

        Object enemy0(-5, 3, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(370, -100);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 5 && !enemyCheck[3]) {
        enemyCheck[3] = true;

        Object enemy1(-5, -3, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1000, 600);
        enemies.push_back(enemy1);

        Object enemy2(-5, -3, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(930, 600);
        enemies.push_back(enemy2);

        Object enemy3(-5, -3, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(860, 600);
        enemies.push_back(enemy3);

        Object enemy4(-5, -3, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(790, 600);
        enemies.push_back(enemy4);

        Object enemy5(-5, -3, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(720, 600);
        enemies.push_back(enemy5);

        Object enemy6(-5, -3, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(650, 600);
        enemies.push_back(enemy6);

        Object enemy7(-5, -3, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(580, 600);
        enemies.push_back(enemy7);

        Object enemy8(-5, -3, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(510, 600);
        enemies.push_back(enemy8);

        Object enemy9(-5, -3, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(440, 600);
        enemies.push_back(enemy9);

        Object enemy0(-5, -3, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(370, 600);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 9 && !enemyCheck[4]) {
        enemyCheck[4] = true;

        Object enemy1(-5, 0, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 10);
        enemies.push_back(enemy1);

        Object enemy2(-5, 0, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 60);
        enemies.push_back(enemy2);

        Object enemy3(-5, 0, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 110);
        enemies.push_back(enemy3);

        Object enemy4(-5, 0, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 160);
        enemies.push_back(enemy4);

        Object enemy5(-5, 0, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 210);
        enemies.push_back(enemy5);

        Object enemy6(-5, 0, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 260);
        enemies.push_back(enemy6);

        Object enemy7(-5, 0, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 310);
        enemies.push_back(enemy7);

        Object enemy8(-5, 0, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 360);
        enemies.push_back(enemy8);

        Object enemy9(-5, 0, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 410);
        enemies.push_back(enemy9);

        Object enemy0(-5, 0, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(1600, 460);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) >= 10) {
        if ((int)std::floor(frames) == 10)
            for (int x = 0; x < 5; x++)
                enemyCheck[x] = false;
        enemySpawner2(enemyTexture);
    }
}

void enemySpawner2(sf::Texture& enemyTexture) {
    if ((int)std::floor(frames) == 11 && !enemyCheck[0]) {
        enemyCheck[0] = true;

        Object enemy1(-5, 0, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 25);
        enemies.push_back(enemy1);

        Object enemy2(-5, 0, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 75);
        enemies.push_back(enemy2);

        Object enemy3(-5, 0, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 125);
        enemies.push_back(enemy3);

        Object enemy4(-5, 0, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 175);
        enemies.push_back(enemy4);

        Object enemy5(-5, 0, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 225);
        enemies.push_back(enemy5);

        Object enemy6(-5, 0, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 275);
        enemies.push_back(enemy6);

        Object enemy7(-5, 0, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 325);
        enemies.push_back(enemy7);

        Object enemy8(-5, 0, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 375);
        enemies.push_back(enemy8);

        Object enemy9(-5, 0, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 425);
        enemies.push_back(enemy9);
    }

    if ((int)std::floor(frames) == 12 && !enemyCheck[1]) {
        enemyCheck[1] = true;

        Object enemy1(-9, 0, 2);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 10);
        enemies.push_back(enemy1);

        Object enemy2(-9, 0, 2);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 60);
        enemies.push_back(enemy2);

        Object enemy3(-9, 0, 2);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 110);
        enemies.push_back(enemy3);

        Object enemy4(-9, 0, 2);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 160);
        enemies.push_back(enemy4);

        Object enemy5(-9, 0, 2);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 210);
        enemies.push_back(enemy5);

        Object enemy6(-9, 0, 2);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 260);
        enemies.push_back(enemy6);

        Object enemy7(-9, 0, 2);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 310);
        enemies.push_back(enemy7);

        Object enemy8(-9, 0, 2);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 360);
        enemies.push_back(enemy8);

        Object enemy9(-9, 0, 2);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 410);
        enemies.push_back(enemy9);

        Object enemy0(-9, 0, 2);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(1600, 460);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 13 && !enemyCheck[2]) {
        enemyCheck[2] = true;

        Object enemy1(-5, -4, 2);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1400, 600);
        enemies.push_back(enemy1);

        Object enemy2(-5, -4, 2);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1330, 600);
        enemies.push_back(enemy2);

        Object enemy3(-5, -4, 2);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1260, 600);
        enemies.push_back(enemy3);

        Object enemy4(-5, -4, 2);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1190, 600);
        enemies.push_back(enemy4);

        Object enemy5(-5, -4, 2);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1120, 600);
        enemies.push_back(enemy5);

        Object enemy6(-5, -4, 2);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1050, 600);
        enemies.push_back(enemy6);

        Object enemy7(-5, -4, 2);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(980, 600);
        enemies.push_back(enemy7);

        Object enemy8(-5, -4, 2);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(910, 600);
        enemies.push_back(enemy8);

        Object enemy9(-5, -4, 2);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(840, 600);
        enemies.push_back(enemy9);

        Object enemy0(-5, -4, 2);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 41, 43, 32));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(770, 600);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) >= 14) {
        if ((int)std::floor(frames) == 14)
            for (int x = 0; x < 5; x++)
                enemyCheck[x] = false;
        enemySpawner3(enemyTexture);
    }
}

void enemySpawner3(sf::Texture& enemyTexture) {
    if ((int)std::floor(frames) == 15 && !enemyCheck[0]) {
        enemyCheck[0] = true;

        Object enemy1(-5, -3, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1000, 600);
        enemies.push_back(enemy1);

        Object enemy2(-5, -3, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(930, 600);
        enemies.push_back(enemy2);

        Object enemy3(-5, -3, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(860, 600);
        enemies.push_back(enemy3);

        Object enemy4(-5, -3, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(790, 600);
        enemies.push_back(enemy4);

        Object enemy5(-5, -3, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(720, 600);
        enemies.push_back(enemy5);

        Object enemy6(-5, -3, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(650, 600);
        enemies.push_back(enemy6);

        Object enemy7(-5, -3, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(580, 600);
        enemies.push_back(enemy7);

        Object enemy8(-5, -3, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(510, 600);
        enemies.push_back(enemy8);

        Object enemy9(-5, -3, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(440, 600);
        enemies.push_back(enemy9);

        Object enemy0(-5, -3, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(370, 600);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 16 && !enemyCheck[1]) {
        enemyCheck[1] = true;

        Object enemy15(-5, -3, 1);
        enemy15.setTexture(enemyTexture);
        enemy15.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy15.setColor(sf::Color(255, 240, 255));
        enemy15.setPosition(1420, 600);
        enemies.push_back(enemy15);

        Object enemy14(-5, -3, 1);
        enemy14.setTexture(enemyTexture);
        enemy14.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy14.setColor(sf::Color(255, 240, 255));
        enemy14.setPosition(1350, 600);
        enemies.push_back(enemy14);

        Object enemy13(-5, -3, 1);
        enemy13.setTexture(enemyTexture);
        enemy13.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy13.setColor(sf::Color(255, 240, 255));
        enemy13.setPosition(1280, 600);
        enemies.push_back(enemy13);

        Object enemy12(-5, -3, 1);
        enemy12.setTexture(enemyTexture);
        enemy12.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy12.setColor(sf::Color(255, 240, 255));
        enemy12.setPosition(1210, 600);
        enemies.push_back(enemy12);

        Object enemy11(-5, -3, 1);
        enemy11.setTexture(enemyTexture);
        enemy11.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy11.setColor(sf::Color(255, 240, 255));
        enemy11.setPosition(1140, 600);
        enemies.push_back(enemy11);

        Object enemy10(-5, -3, 1);
        enemy10.setTexture(enemyTexture);
        enemy10.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy10.setColor(sf::Color(255, 240, 255));
        enemy10.setPosition(1070, 600);
        enemies.push_back(enemy10);

        Object enemy1(-5, -3, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1000, 600);
        enemies.push_back(enemy1);

        Object enemy2(-5, -3, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(930, 600);
        enemies.push_back(enemy2);

        Object enemy3(-5, -3, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(860, 600);
        enemies.push_back(enemy3);

        Object enemy4(-5, -3, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(790, 600);
        enemies.push_back(enemy4);

        Object enemy5(-5, -3, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(720, 600);
        enemies.push_back(enemy5);

        Object enemy6(-5, -3, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(650, 600);
        enemies.push_back(enemy6);

        Object enemy7(-5, -3, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(580, 600);
        enemies.push_back(enemy7);

        Object enemy8(-5, -3, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(510, 600);
        enemies.push_back(enemy8);

        Object enemy9(-5, -3, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(440, 600);
        enemies.push_back(enemy9);

        Object enemy0(-5, -3, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(370, 600);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) == 18 && !enemyCheck[2]) {
        enemyCheck[2] = true;

        Object enemy15(-5, 3, 1);
        enemy15.setTexture(enemyTexture);
        enemy15.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy15.setColor(sf::Color(255, 240, 255));
        enemy15.setPosition(1420, -100);
        enemies.push_back(enemy15);

        Object enemy14(-5, 3, 1);
        enemy14.setTexture(enemyTexture);
        enemy14.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy14.setColor(sf::Color(255, 240, 255));
        enemy14.setPosition(1350, -100);
        enemies.push_back(enemy14);

        Object enemy13(-5, 3, 1);
        enemy13.setTexture(enemyTexture);
        enemy13.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy13.setColor(sf::Color(255, 240, 255));
        enemy13.setPosition(1280, -100);
        enemies.push_back(enemy13);

        Object enemy12(-5, 3, 1);
        enemy12.setTexture(enemyTexture);
        enemy12.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy12.setColor(sf::Color(255, 240, 255));
        enemy12.setPosition(1210, -100);
        enemies.push_back(enemy12);

        Object enemy11(-5, 3, 1);
        enemy11.setTexture(enemyTexture);
        enemy11.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy11.setColor(sf::Color(255, 240, 255));
        enemy11.setPosition(1140, -100);
        enemies.push_back(enemy11);

        Object enemy10(-5, 3, 1);
        enemy10.setTexture(enemyTexture);
        enemy10.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy10.setColor(sf::Color(255, 240, 255));
        enemy10.setPosition(1070, -100);
        enemies.push_back(enemy10);

        Object enemy1(-5, 3, 1);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1000, -100);
        enemies.push_back(enemy1);

        Object enemy2(-5, 3, 1);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(930, -100);
        enemies.push_back(enemy2);

        Object enemy3(-5, 3, 1);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(860, -100);
        enemies.push_back(enemy3);

        Object enemy4(-5, 3, 1);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(790, -100);
        enemies.push_back(enemy4);

        Object enemy5(-5, 3, 1);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(720, -100);
        enemies.push_back(enemy5);

        Object enemy6(-5, 3, 1);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(650, -100);
        enemies.push_back(enemy6);

        Object enemy7(-5, 3, 1);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(580, -100);
        enemies.push_back(enemy7);

        Object enemy8(-5, 3, 1);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(510, -100);
        enemies.push_back(enemy8);

        Object enemy9(-5, 3, 1);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(440, -100);
        enemies.push_back(enemy9);

        Object enemy0(-5, 3, 1);
        enemy0.setTexture(enemyTexture);
        enemy0.setTextureRect(sf::IntRect(6, 6, 43, 29));
        enemy0.setColor(sf::Color(255, 240, 255));
        enemy0.setPosition(370, -100);
        enemies.push_back(enemy0);
    }

    if ((int)std::floor(frames) >= 19) {
        if ((int)std::floor(frames) == 19)
            for (int x = 0; x < 5; x++)
                enemyCheck[x] = false;
        enemySpawner4(enemyTexture);
    }
}

void enemySpawner4(sf::Texture& enemyTexture) {
    if ((int)std::floor(frames) == 20 && !enemyCheck[0]) {
        enemyCheck[0] = true;

        Object enemy1(-11, 0, 3);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 25);
        enemies.push_back(enemy1);

        Object enemy2(-11, 0, 3);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 75);
        enemies.push_back(enemy2);

        Object enemy3(-11, 0, 3);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 125);
        enemies.push_back(enemy3);

        Object enemy4(-11, 0, 3);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 175);
        enemies.push_back(enemy4);

        Object enemy5(-11, 0, 3);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 225);
        enemies.push_back(enemy5);

        Object enemy6(-11, 0, 3);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 275);
        enemies.push_back(enemy6);

        Object enemy7(-11, 0, 3);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 325);
        enemies.push_back(enemy7);

        Object enemy8(-11, 0, 3);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 375);
        enemies.push_back(enemy8);

        Object enemy9(-11, 0, 3);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 425);
        enemies.push_back(enemy9);
    }

    if ((int)std::floor(frames) == 21 && !enemyCheck[1]) {
        enemyCheck[1] = true;

        Object enemy1(-11, 0, 3);
        enemy1.setTexture(enemyTexture);
        enemy1.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy1.setColor(sf::Color(255, 240, 255));
        enemy1.setPosition(1600, 25);
        enemies.push_back(enemy1);

        Object enemy2(-11, 0, 3);
        enemy2.setTexture(enemyTexture);
        enemy2.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy2.setColor(sf::Color(255, 240, 255));
        enemy2.setPosition(1600, 75);
        enemies.push_back(enemy2);

        Object enemy3(-11, 0, 3);
        enemy3.setTexture(enemyTexture);
        enemy3.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy3.setColor(sf::Color(255, 240, 255));
        enemy3.setPosition(1600, 125);
        enemies.push_back(enemy3);

        Object enemy4(-11, 0, 3);
        enemy4.setTexture(enemyTexture);
        enemy4.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy4.setColor(sf::Color(255, 240, 255));
        enemy4.setPosition(1600, 175);
        enemies.push_back(enemy4);

        Object enemy5(-11, 0, 3);
        enemy5.setTexture(enemyTexture);
        enemy5.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy5.setColor(sf::Color(255, 240, 255));
        enemy5.setPosition(1600, 225);
        enemies.push_back(enemy5);

        Object enemy6(-11, 0, 3);
        enemy6.setTexture(enemyTexture);
        enemy6.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy6.setColor(sf::Color(255, 240, 255));
        enemy6.setPosition(1600, 275);
        enemies.push_back(enemy6);

        Object enemy7(-11, 0, 3);
        enemy7.setTexture(enemyTexture);
        enemy7.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy7.setColor(sf::Color(255, 240, 255));
        enemy7.setPosition(1600, 325);
        enemies.push_back(enemy7);

        Object enemy8(-11, 0, 3);
        enemy8.setTexture(enemyTexture);
        enemy8.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy8.setColor(sf::Color(255, 240, 255));
        enemy8.setPosition(1600, 375);
        enemies.push_back(enemy8);

        Object enemy9(-11, 0, 3);
        enemy9.setTexture(enemyTexture);
        enemy9.setTextureRect(sf::IntRect(6, 78, 43, 44));
        enemy9.setColor(sf::Color(255, 240, 255));
        enemy9.setPosition(1600, 425);
        enemies.push_back(enemy9);
    }
}

void congratulation(sf::Text& playText, float deltaTime, sf::RenderWindow& window) {
    waitTimer += deltaTime;

    playText.setString("CONGRATULATION!");
    playText.setPosition(sf::Vector2f(316, 184));

    if (waitTimer >= 0.2) {
        window.draw(playText);
        if (waitTimer >= 0.8) {
            waitTimer = 0;
            textCounter++;
        }
    }
    if (textCounter >= 10) {
        textCounter = 0;
        waitTimer = 0;
        play = false;
    }
}

std::string to_stringX(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

Object::Object() {
    velocityX = velocityY = 0;
    health = 0;
}

Object::Object(float veloX, float veloY, int x) {
    health = x;
    velocityX = veloX;
    velocityY = veloY;
}

void Object::setHitbox(const sf::FloatRect& hitbox) {
    m_hitbox = hitbox;
}

sf::FloatRect Object::getGlobalHitbox() const {
    return getTransform().transformRect(m_hitbox);
}

void Object::setHealth(int x) {
    health = x;
}

int Object::getHealth() {
    return health;
}

void Object::moves(float deltaTime) {
    (*this).move(velocityX * deltaTime * multiplier, velocityY * deltaTime * multiplier);
}
