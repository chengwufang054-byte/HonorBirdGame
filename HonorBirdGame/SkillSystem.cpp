#include "SkillSystem.h"
#include "GameLogic.h"

#include <cmath>

void applyCharlotteAreaSkill(
    const sf::CircleShape& bird,
    std::vector<Enemy>& enemies,
    std::vector<Block>& blocks,
    RuntimeState& state)
{
    const sf::Vector2f birdCenter = getCircleCenter(bird);
    const float radius = state.skillEffectRadius;

    for (Enemy& enemy : enemies)
    {
        if (!enemy.alive)
        {
            continue;
        }

        const sf::Vector2f enemyCenter = getCircleCenter(enemy.shape);
        const float dx = birdCenter.x - enemyCenter.x;
        const float dy = birdCenter.y - enemyCenter.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= radius)
        {
            applyEnemyDamage(enemy, 1);
            enemy.hitFlashTimer = 0.26f;
        }
    }

    for (Block& block : blocks)
    {
        if (!block.alive)
        {
            continue;
        }

        const sf::Vector2f blockCenter = getRectCenter(block.shape);
        const float dx = birdCenter.x - blockCenter.x;
        const float dy = birdCenter.y - blockCenter.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= radius)
        {
            block.hp -= 1;
            block.hitFlashTimer = 0.26f;

            if (block.hp == 2)
            {
                block.shape.setFillColor(sf::Color(100, 75, 50));
            }
            else if (block.hp == 1)
            {
                block.shape.setFillColor(sf::Color(80, 60, 40));
            }

            if (block.hp <= 0)
            {
                block.hp = 0;
                block.alive = false;
            }
        }
    }
}

void drawPentagram(
    sf::RenderWindow& window,
    const sf::Vector2f& center,
    float outerRadius,
    const sf::Color& color)
{
    sf::Vector2f points[5];

    for (int i = 0; i < 5; i++)
    {
        float angleDeg = -90.f + 72.f * i;
        float angleRad = angleDeg * 3.1415926f / 180.f;

        points[i].x = center.x + outerRadius * std::cos(angleRad);
        points[i].y = center.y + outerRadius * std::sin(angleRad);
    }

    int order[6] = { 0, 2, 4, 1, 3, 0 };

    for (int i = 0; i < 5; i++)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(points[order[i]], color),
            sf::Vertex(points[order[i + 1]], color)
        };

        window.draw(line, 2, sf::Lines);
    }
}

sf::ConvexShape makeRoundedPetal(
    const sf::Vector2f& center,
    float angleDeg,
    float length,
    float halfWidth,
    const sf::Color& color)
{
    sf::ConvexShape petal;
    petal.setPointCount(8);

    float angleRad = angleDeg * 3.1415926f / 180.f;
    float ux = std::cos(angleRad);
    float uy = std::sin(angleRad);

    float px = -uy;
    float py = ux;

    sf::Vector2f p0 = center;
    sf::Vector2f p1(center.x + ux * (length * 0.20f) + px * (halfWidth * 0.55f),
                    center.y + uy * (length * 0.20f) + py * (halfWidth * 0.55f));
    sf::Vector2f p2(center.x + ux * (length * 0.45f) + px * halfWidth,
                    center.y + uy * (length * 0.45f) + py * halfWidth);
    sf::Vector2f p3(center.x + ux * (length * 0.78f) + px * (halfWidth * 0.42f),
                    center.y + uy * (length * 0.78f) + py * (halfWidth * 0.42f));
    sf::Vector2f p4(center.x + ux * length,
                    center.y + uy * length);
    sf::Vector2f p5(center.x + ux * (length * 0.78f) - px * (halfWidth * 0.42f),
                    center.y + uy * (length * 0.78f) - py * (halfWidth * 0.42f));
    sf::Vector2f p6(center.x + ux * (length * 0.45f) - px * halfWidth,
                    center.y + uy * (length * 0.45f) - py * halfWidth);
    sf::Vector2f p7(center.x + ux * (length * 0.20f) - px * (halfWidth * 0.55f),
                    center.y + uy * (length * 0.20f) - py * (halfWidth * 0.55f));

    petal.setPoint(0, p0);
    petal.setPoint(1, p1);
    petal.setPoint(2, p2);
    petal.setPoint(3, p3);
    petal.setPoint(4, p4);
    petal.setPoint(5, p5);
    petal.setPoint(6, p6);
    petal.setPoint(7, p7);

    petal.setFillColor(color);
    return petal;
}

void drawCharlotteSkillEffect(sf::RenderWindow& window, const RuntimeState& state)
{
    if (!state.skillSequenceActive || state.skillEffectTimer >= state.skillEffectDuration)
    {
        return;
    }

    float t = state.skillEffectTimer;
    float voiceEnd = state.skillVoiceDuration;
    float totalEnd = state.skillEffectDuration;
    float damageTime = state.skillVoiceDuration;

    if (t < voiceEnd + 0.10f)
    {
        float appear = t / (voiceEnd * 0.65f);
        if (appear > 1.f)
        {
            appear = 1.f;
        }

        float fade = 1.f;
        if (t > voiceEnd)
        {
            fade = 1.f - (t - voiceEnd) / 0.10f;
            if (fade < 0.f)
            {
                fade = 0.f;
            }
        }

        float alphaFactor = appear * fade;
        int alpha = static_cast<int>(220.f * alphaFactor);

        float pulse = 0.85f + 0.15f * std::sin(t * 18.f);
        float starRadius = (22.f + 50.f * appear) * pulse;

        sf::CircleShape core(12.f + 18.f * appear);
        core.setOrigin(core.getRadius(), core.getRadius());
        core.setPosition(state.skillEffectCenter);
        core.setFillColor(sf::Color(255, 228, 235, alpha));
        window.draw(core);

        drawPentagram(
            window,
            state.skillEffectCenter,
            starRadius,
            sf::Color(220, 50, 80, alpha));

        drawPentagram(
            window,
            state.skillEffectCenter,
            starRadius * 0.62f,
            sf::Color(255, 170, 195, alpha));
    }

    float flowerStart = voiceEnd - 0.08f;
    if (flowerStart < 0.f)
    {
        flowerStart = 0.f;
    }

    if (t >= flowerStart && t < totalEnd)
    {
        float bloom = (t - flowerStart) / 0.34f;
        if (bloom > 1.f)
        {
            bloom = 1.f;
        }

        float fade = 1.f;
        float fadeStart = damageTime + 0.34f;

        if (t > fadeStart)
        {
            float fadeDuration = totalEnd - fadeStart;
            if (fadeDuration < 0.01f)
            {
                fadeDuration = 0.01f;
            }

            fade = 1.f - (t - fadeStart) / fadeDuration;
            if (fade < 0.f)
            {
                fade = 0.f;
            }
        }

        float alphaFactor = bloom * fade;

        int outerAlpha = static_cast<int>(205.f * alphaFactor);
        int innerAlpha = static_cast<int>(240.f * alphaFactor);

        float petalLength = 52.f + 145.f * bloom;
        float petalHalfWidth = 14.f + 18.f * bloom;
        float petalRotate = 10.f * bloom;

        float petalAngles[5] = { -90.f, -18.f, 54.f, 126.f, 198.f };

        for (int i = 0; i < 5; ++i)
        {
            sf::ConvexShape outerPetal = makeRoundedPetal(
                state.skillEffectCenter,
                petalAngles[i] + petalRotate,
                petalLength,
                petalHalfWidth,
                sf::Color(170, 28, 52, outerAlpha));

            sf::ConvexShape innerPetal = makeRoundedPetal(
                state.skillEffectCenter,
                petalAngles[i] + petalRotate,
                petalLength * 0.76f,
                petalHalfWidth * 0.45f,
                sf::Color(248, 112, 138, innerAlpha));

            window.draw(outerPetal);
            window.draw(innerPetal);
        }

        float ringAlpha = 135.f * fade;
        float ringRadius = 72.f + 138.f * bloom;

        sf::CircleShape outerRing(ringRadius);
        outerRing.setOrigin(ringRadius, ringRadius);
        outerRing.setPosition(state.skillEffectCenter);
        outerRing.setFillColor(sf::Color::Transparent);
        outerRing.setOutlineThickness(4.f);
        outerRing.setOutlineColor(sf::Color(220, 50, 80, static_cast<int>(ringAlpha)));
        window.draw(outerRing);

        sf::CircleShape innerRing(ringRadius * 0.58f);
        innerRing.setOrigin(innerRing.getRadius(), innerRing.getRadius());
        innerRing.setPosition(state.skillEffectCenter);
        innerRing.setFillColor(sf::Color::Transparent);
        innerRing.setOutlineThickness(2.f);
        innerRing.setOutlineColor(sf::Color(255, 185, 205, static_cast<int>(ringAlpha * 0.75f)));
        window.draw(innerRing);
    }
}
