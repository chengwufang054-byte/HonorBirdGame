#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// Core constants
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

const float WORLD_WIDTH = 3000.f;
const float WORLD_HEIGHT = 900.f;

const float DEFAULT_VIEW_WIDTH = 1600.f;
const float DEFAULT_VIEW_HEIGHT = 900.f;

const float GROUND_TOP = 760.f;
const sf::Vector2f DEFAULT_VIEW_CENTER(930.f, 450.f);
const sf::Vector2f DEFAULT_BIRD_START(180.f, 700.f);

enum class GameState
{
    StartMenu,
    Playing,
    LevelClear,
    Win,
    Lose
};

enum class BirdType
{
    Normal,
    Charlotte,
    Mulan
};

enum class EnemyType
{
    Normal,
    Boss
};

enum class BlockType
{
    Pillar,
    Beam,
    DecoyPillar
};

struct BirdStats
{
    sf::Color color;
    float launchPower;
    int blockDamage;
    int enemyDamage;
};

struct Enemy
{
    sf::CircleShape shape;
    EnemyType type = EnemyType::Normal;
    int maxHp = 1;
    int hp = 1;
    bool alive = true;
    float hitFlashTimer = 0.f;
};

struct Block
{
    sf::RectangleShape shape;
    BlockType type = BlockType::Pillar;
    int maxHp = 2;
    int hp = 2;
    bool alive = true;
    bool collisionHandled = false;
    bool canFall = false;
    bool falling = false;
    int fallDamage = 2;
    std::vector<int> supportIndices;
    std::vector<int> damagedEnemyIndices;
    bool isKeySupport = false;
    float hitFlashTimer = 0.f;
};

struct EnemySpawnData
{
    EnemyType type = EnemyType::Normal;
    sf::Vector2f position{ 0.f, 0.f };
    int hp = 1;
};

struct BlockSpawnData
{
    BlockType type = BlockType::Pillar;
    sf::Vector2f position{ 0.f, 0.f };
    sf::Vector2f size{ 40.f, 160.f };
    int hp = 2;
    bool canFall = false;
    int fallDamage = 2;
    std::vector<int> supportIndices;
    bool isKeySupport = false;
};

struct LevelData
{
    sf::Vector2f birdStartPos = DEFAULT_BIRD_START;
    sf::Vector2f homeViewCenter = DEFAULT_VIEW_CENTER;
    sf::Vector2f homeViewSize = sf::Vector2f(DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT);
    std::vector<BirdType> birdQueue;
    std::vector<EnemySpawnData> enemySpawns;
    std::vector<BlockSpawnData> blockSpawns;
};

struct RuntimeState
{
    GameState gameState = GameState::StartMenu;
    int currentLevel = 1;
    int nextLevel = 1;

    bool birdReady = true;
    bool isDragging = false;
    bool roundOver = false;

    int birdsRemaining = 0;
    int currentBirdIndex = 0;

    bool skillUsedThisBird = false;
    bool skillSequenceActive = false;
    bool skillDamageApplied = false;
    float skillEffectTimer = 0.f;
    sf::Vector2f skillEffectCenter{ 0.f, 0.f };
    float skillEffectRadius = 220.f;
    float skillEffectDuration = 0.92f;
    float skillVoiceDuration = 1.20f;

    float birdSpeedX = 0.f;
    float birdSpeedY = 0.f;
    float preLaunchViewOffsetX = 0.f;
    float gravity = 0.0012f;
    float maxDragDistance = 150.f;

    BirdType currentBirdType = BirdType::Normal;
    BirdStats currentBirdStats{ sf::Color::Red, 0.03f, 1, 1 };
    float launchPower = 0.03f;
};
