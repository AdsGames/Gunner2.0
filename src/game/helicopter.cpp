#include "./helicopter.h"

#include <algorithm>
#include <cmath>

#include "../globals.h"
#include "./world.h"

namespace {

constexpr float TEX_W = 200.0F;
constexpr float TEX_H = 62.0F;

struct HeliStats {
  float scale;
  float health;
  float health_per_wave;
  float speed;
  float fire_delay;
  int score;
  asw::Color tint;
};

HeliStats get_stats(HeliType type) {
  switch (type) {
    case HeliType::Scout:
      return {0.6F, 30.0F, 2.0F, 280.0F, 1.6F, 100, asw::Color(255, 225, 150)};
    case HeliType::Gunship:
      return {1.0F, 100.0F, 6.0F, 140.0F, 2.2F, 300, asw::Color(255, 255, 255)};
    case HeliType::Bomber:
      return {0.8F, 60.0F, 4.0F, 190.0F, 3.0F, 200, asw::Color(170, 175, 215)};
    case HeliType::Boss:
      return {
          1.8F, 700.0F, 120.0F, 110.0F, 1.3F, 5000, asw::Color(255, 140, 130)};
  }
  return {1.0F, 50.0F, 0.0F, 150.0F, 2.0F, 100, asw::Color(255, 255, 255)};
}

// Enemies speed up a little every wave
float difficulty(int wave) {
  return std::max(0.45F, 1.0F - (static_cast<float>(wave - 1) * 0.045F));
}

float bullet_speed(int wave) {
  return std::min(540.0F, 300.0F + (static_cast<float>(wave) * 14.0F));
}

}  // namespace

Helicopter::Helicopter(HeliType type, int wave) : type(type), wave(wave) {
  const auto stats = get_stats(type);
  scale = stats.scale;
  tint = stats.tint;
  speed = stats.speed;
  max_health = health =
      stats.health + (stats.health_per_wave * static_cast<float>(wave));
  fire_delay = stats.fire_delay * difficulty(wave);
  fire_timer = fire_delay * asw::random::between(0.6F, 1.2F);
  mine_timer = 1.5F;
  bob = asw::random::between(0.0F, 10.0F);

  const auto size = get_size();

  if (type == HeliType::Boss) {
    // Boss drops in from the top
    pos = {SCREEN_W / 2.0F, -size.y};
    base_y = target_y = 170.0F;
    dir = asw::random::chance() ? 1.0F : -1.0F;
    ring_timer = 2.0F;
  } else {
    dir = asw::random::chance() ? 1.0F : -1.0F;
    pos.x = dir > 0 ? -size.x / 2.0F : SCREEN_W + (size.x / 2.0F);
    const float max_y = type == HeliType::Scout ? 380.0F : 320.0F;
    base_y = target_y = asw::random::between(70.0F, max_y);
    pos.y = base_y;
  }
  retarget_timer = asw::random::between(2.0F, 5.0F);
}

asw::Vec2f Helicopter::get_size() const {
  return {TEX_W * scale, TEX_H * scale};
}

asw::Quad<float> Helicopter::get_hitbox() const {
  // Skip the thin rotor on top
  const auto size = get_size();
  return {pos.x - (size.x * 0.45F), pos.y - (size.y * 0.2F), size.x * 0.9F,
          size.y * 0.7F};
}

int Helicopter::get_score() const {
  return get_stats(type).score;
}

asw::Vec2f Helicopter::gun_position() const {
  // Sprite nose points left; it is mirrored when flying right
  const auto size = get_size();
  return {pos.x + (dir * size.x * 0.38F), pos.y + (size.y * 0.25F)};
}

bool Helicopter::damage(float amount) {
  if (health <= 0.0F) {
    return false;
  }
  health -= amount;
  hurt_timer = 0.05F;
  return health <= 0.0F;
}

void Helicopter::update(float dt, World& world) {
  const auto size = get_size();
  const float half_w = size.x / 2.0F;
  const bool enraged = type == HeliType::Boss && health < max_health * 0.33F;
  const float move_speed = enraged ? speed * 1.6F : speed;

  hurt_timer -= dt;
  bob += dt;

  // Boss entry
  if (type == HeliType::Boss && entering) {
    pos.y += 120.0F * dt;
    if (pos.y >= target_y) {
      pos.y = target_y;
      entering = false;
    }
    tilt = 0.0F;
    return;
  }

  // Horizontal patrol with short hovers at each edge
  float vel_x = 0.0F;
  if (hover_timer > 0.0F) {
    hover_timer -= dt;
  } else {
    vel_x = dir * move_speed;
    pos.x += vel_x * dt;

    const float left = half_w + 10.0F;
    const float right = SCREEN_W - half_w - 10.0F;

    if (entering && pos.x > left && pos.x < right) {
      entering = false;
    }

    if (!entering) {
      const bool hit_edge =
          (dir > 0 && pos.x >= right) || (dir < 0 && pos.x <= left);
      // The occasional mid-screen turn keeps movement unpredictable
      const bool random_turn =
          type != HeliType::Boss && asw::random::chance(0.25F * dt);
      if (hit_edge || random_turn) {
        pos.x = std::clamp(pos.x, left, right);
        hover_timer = asw::random::between(0.3F, 1.1F);
        dir = -dir;
      }
    }
  }

  // Drift between altitudes
  retarget_timer -= dt;
  if (retarget_timer <= 0.0F) {
    retarget_timer = asw::random::between(2.5F, 5.0F);
    const float max_y = type == HeliType::Scout ? 400.0F : 320.0F;
    target_y = type == HeliType::Boss ? asw::random::between(120.0F, 260.0F)
                                      : asw::random::between(70.0F, max_y);
  }
  const float climb = 50.0F * dt;
  base_y += std::clamp(target_y - base_y, -climb, climb);
  pos.y = base_y + (std::sin(bob * 2.2F) * 8.0F * scale);

  // Nose dips toward the direction of travel
  const float target_tilt = 0.14F * vel_x / std::max(1.0F, move_speed);
  tilt += (target_tilt - tilt) * std::min(1.0F, 6.0F * dt);

  // Trail smoke when badly damaged
  if (health < max_health * 0.4F && asw::random::chance(12.0F * dt)) {
    world.get_effects().smoke_puff(
        pos + asw::Vec2f(-dir * size.x * 0.25F, -size.y * 0.1F));
  }

  // Only attack while on screen
  const bool on_screen = pos.x > 0.0F && pos.x < SCREEN_W;
  if (!on_screen || !world.get_player().is_alive()) {
    return;
  }

  if (type == HeliType::Boss) {
    update_boss_attacks(dt, world);
    return;
  }

  fire_timer -= dt;
  if (fire_timer <= 0.0F) {
    fire_timer = fire_delay * asw::random::between(0.8F, 1.2F);
    switch (type) {
      case HeliType::Scout:
        fire_aimed(world, 0.0F, 1, bullet_speed(wave) * 1.1F);
        break;
      case HeliType::Gunship:
        fire_aimed(world, 0.2F, wave >= 6 ? 5 : 3, bullet_speed(wave));
        break;
      case HeliType::Bomber:
        fire_aimed(world, 0.0F, 1, bullet_speed(wave) * 0.9F);
        break;
      case HeliType::Boss:
        break;
    }
  }

  if (type == HeliType::Bomber) {
    mine_timer -= dt;
    const float player_x = world.get_player().get_center().x;
    if (mine_timer <= 0.0F && std::abs(player_x - pos.x) < 260.0F) {
      mine_timer = 2.2F * difficulty(wave);
      world.spawn_mine(pos + asw::Vec2f(0.0F, size.y * 0.4F),
                       asw::Vec2f(vel_x * 0.5F, 0.0F));
    }
  }
}

void Helicopter::update_boss_attacks(float dt, World& world) {
  const float fraction = health / max_health;
  const auto size = get_size();

  fire_timer -= dt;
  mine_timer -= dt;
  ring_timer -= dt;

  // Phase 1: aimed spreads and mines
  // Phase 2: adds bullet rings
  // Phase 3: adds spirals, everything faster
  const bool phase2 = fraction < 0.66F;
  const bool phase3 = fraction < 0.33F;

  if (fire_timer <= 0.0F) {
    fire_timer = phase3 ? 0.9F : fire_delay;
    fire_aimed(world, 0.16F, phase2 ? 5 : 3, bullet_speed(wave));
  }

  if (mine_timer <= 0.0F) {
    mine_timer = phase3 ? 2.2F : 3.5F;
    world.spawn_mine(pos + asw::Vec2f(0.0F, size.y * 0.4F),
                     asw::Vec2f(asw::random::between(-150.0F, 150.0F), 0.0F));
  }

  if (phase2 && ring_timer <= 0.0F) {
    ring_timer = phase3 ? 3.2F : 2.4F;
    fire_ring(world, 14, bullet_speed(wave) * 0.75F,
              asw::random::between(0.0F, PI));
    if (phase3) {
      spiral_time = 1.6F;
    }
  }

  if (spiral_time > 0.0F) {
    spiral_time -= dt;
    spiral_shot_timer -= dt;
    if (spiral_shot_timer <= 0.0F) {
      spiral_shot_timer = 0.07F;
      spiral_angle += 0.35F;
      for (int i = 0; i < 2; i++) {
        const float angle = spiral_angle + (static_cast<float>(i) * PI);
        Bullet b;
        b.pos = pos;
        b.vel = {std::cos(angle) * 260.0F, std::sin(angle) * 260.0F};
        b.radius = 5.0F;
        b.damage = 8.0F;
        b.from_player = false;
        b.life = 5.0F;
        world.spawn_bullet(b);
      }
    }
  }
}

void Helicopter::fire_aimed(World& world,
                            float spread,
                            int count,
                            float bullet_speed) {
  const auto gun = gun_position();
  const auto target = world.get_player().get_center();
  const float base = std::atan2(target.y - gun.y, target.x - gun.x) +
                     asw::random::between(-0.05F, 0.05F);

  for (int i = 0; i < count; i++) {
    const float offset =
        (static_cast<float>(i) - (static_cast<float>(count - 1) / 2.0F)) *
        spread;
    const float angle = base + offset;
    Bullet b;
    b.pos = gun;
    b.vel = {std::cos(angle) * bullet_speed, std::sin(angle) * bullet_speed};
    b.radius = 5.0F;
    b.damage = 10.0F;
    b.from_player = false;
    b.life = 5.0F;
    world.spawn_bullet(b);
  }

  audio::play("enemy_shoot", 0.35F, gun.x);
}

void Helicopter::fire_ring(World& world,
                           int count,
                           float bullet_speed,
                           float offset) {
  for (int i = 0; i < count; i++) {
    const float angle = offset + (2.0F * PI * static_cast<float>(i) /
                                  static_cast<float>(count));
    Bullet b;
    b.pos = pos;
    b.vel = {std::cos(angle) * bullet_speed, std::sin(angle) * bullet_speed};
    b.radius = 6.0F;
    b.damage = 10.0F;
    b.from_player = false;
    b.life = 5.0F;
    world.spawn_bullet(b);
  }
  audio::play("enemy_shoot", 0.6F, pos.x);
}

void Helicopter::draw() const {
  const auto size = get_size();
  const asw::Quad<float> dest(pos.x - (size.x / 2.0F), pos.y - (size.y / 2.0F),
                              size.x, size.y);
  const bool flip = dir > 0;

  if (hurt_timer > 0.0F) {
    gfx::sprite_ex(asw::assets::get_texture("helicopter_hurt"), dest, tilt,
                   flip);
  } else {
    const auto tex = asw::assets::get_texture("helicopter");
    auto color = tint;

    // Enraged boss pulses red
    if (type == HeliType::Boss && health < max_health * 0.33F) {
      const float pulse = (std::sin(bob * 12.0F) * 0.5F) + 0.5F;
      color = asw::Color(255, static_cast<uint8_t>(80 + (60 * pulse)),
                         static_cast<uint8_t>(70 + (60 * pulse)));
    }

    asw::draw::set_tint(tex, color);
    gfx::sprite_ex(tex, dest, tilt, flip);
    asw::draw::set_tint(tex, asw::Color(255, 255, 255));
  }

  // Spinning rotor blur
  if (std::fmod(bob, 0.08F) < 0.04F) {
    const float rotor_y = dest.position.y + (size.y * 0.16F);
    const float rotor_x = pos.x + (dir * size.x * 0.1F);
    asw::draw::line(asw::Vec2f(rotor_x - (size.x * 0.4F), rotor_y),
                    asw::Vec2f(rotor_x + (size.x * 0.4F), rotor_y),
                    asw::Color(40, 40, 40, 160));
  }

  // Health bar for regular enemies once damaged
  if (type != HeliType::Boss && health < max_health) {
    const float w = std::max(40.0F, size.x * 0.5F);
    const asw::Quad<float> back(pos.x - (w / 2.0F), dest.position.y - 10.0F, w,
                                5.0F);
    asw::draw::rect_fill(back, asw::Color(0, 0, 0, 180));
    asw::draw::rect_fill(
        asw::Quad<float>(back.position.x, back.position.y,
                         w * std::max(0.0F, health / max_health), 5.0F),
        asw::Color(255, 70, 50));
  }
}
