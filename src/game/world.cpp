#include "./world.h"

#include <algorithm>
#include <cmath>

#include "../globals.h"

namespace {

constexpr int START_LIVES = 3;
constexpr int MAX_LIVES = 5;
constexpr int EXTRA_LIFE_SCORE = 30000;
constexpr int MAX_MULTIPLIER = 8;
constexpr float COMBO_TIME = 3.0F;
constexpr int BOSS_EVERY = 5;
constexpr float MINE_DAMAGE = 25.0F;

const asw::Color GOLD(255, 220, 60);

std::string with_commas(int value) {
  auto digits = std::to_string(value);
  for (int i = static_cast<int>(digits.size()) - 3; i > 0; i -= 3) {
    digits.insert(static_cast<size_t>(i), ",");
  }
  return digits;
}

// Shortest distance from point p to segment a-b
float distance_to_segment(const asw::Vec2f& p,
                          const asw::Vec2f& a,
                          const asw::Vec2f& b) {
  const auto ab = b - a;
  const float len_sq = ab.dot(ab);
  const float t =
      len_sq > 0.0F ? std::clamp((p - a).dot(ab) / len_sq, 0.0F, 1.0F) : 0.0F;
  return p.distance(a + (ab * t));
}

asw::Quad<float> bullet_box(const Bullet& b) {
  return {b.pos.x - b.radius, b.pos.y - b.radius, b.radius * 2.0F,
          b.radius * 2.0F};
}

void draw_bar(const asw::Quad<float>& area,
              float fraction,
              asw::Color fill,
              asw::Color back = asw::Color(40, 20, 20)) {
  asw::draw::rect_fill(
      asw::Quad<float>(area.position.x - 2, area.position.y - 2,
                       area.size.x + 4, area.size.y + 4),
      asw::Color(0, 0, 0));
  asw::draw::rect_fill(area, back);
  asw::draw::rect_fill(
      asw::Quad<float>(area.position.x, area.position.y,
                       area.size.x * std::clamp(fraction, 0.0F, 1.0F),
                       area.size.y),
      fill);
}

}  // namespace

void World::reset(int best) {
  helicopters.clear();
  bullets.clear();
  mines.clear();
  pickups.clear();
  effects.clear();
  player.reset();

  score = 0;
  high_score = best;
  next_extra_life = EXTRA_LIFE_SCORE;
  combo = 0;
  combo_timer = 0.0F;
  lives = START_LIVES;
  respawn_timer = 0.0F;
  hit_stop = 0.0F;
  game_over = false;
  time = 0.0F;

  start_wave(1);
}

void World::start_wave(int number) {
  wave = number;
  phase = Phase::Intro;
  phase_timer = 2.2F;
  perfect_wave = true;
  spawn_queue.clear();
  spawn_timer = 0.0F;
  escort_timer = 6.0F;

  if (wave % BOSS_EVERY == 0) {
    spawn_queue.push_back(HeliType::Boss);
    max_alive = 3;
    spawn_interval = 0.5F;
    banner = "WARNING!";
    sub_banner = "BOSS APPROACHING";
    banner_warning = true;
    audio::play("boss", 0.9F);
  } else {
    const int count = std::min(3 + (wave * 2), 24);
    for (int i = 0; i < count; i++) {
      const float roll = asw::random::between(0.0F, 1.0F);
      if (wave >= 3 && roll < 0.22F) {
        spawn_queue.push_back(HeliType::Bomber);
      } else if (wave >= 2 && roll < 0.5F) {
        spawn_queue.push_back(HeliType::Gunship);
      } else {
        spawn_queue.push_back(HeliType::Scout);
      }
    }
    max_alive = static_cast<size_t>(std::min(2 + (wave / 2), 6));
    spawn_interval = std::max(0.5F, 1.8F - (static_cast<float>(wave) * 0.1F));
    banner = "WAVE " + std::to_string(wave);
    sub_banner = "GET READY";
    banner_warning = false;
    audio::play("wave", 0.8F);
  }
}

void World::update_waves(float dt) {
  phase_timer -= dt;

  switch (phase) {
    case Phase::Intro:
      if (phase_timer <= 0.0F) {
        phase = Phase::Fighting;
        banner.clear();
      }
      break;

    case Phase::Fighting: {
      spawn_timer -= dt;
      if (!spawn_queue.empty() && spawn_timer <= 0.0F &&
          helicopters.size() < max_alive) {
        helicopters.emplace_back(spawn_queue.front(), wave);
        spawn_queue.erase(spawn_queue.begin());
        spawn_timer = spawn_interval;
      }

      // Scouts escort the boss
      const bool boss_alive = std::ranges::any_of(
          helicopters,
          [](const Helicopter& h) { return h.get_type() == HeliType::Boss; });
      if (boss_alive) {
        escort_timer -= dt;
        if (escort_timer <= 0.0F && helicopters.size() < max_alive) {
          escort_timer = 7.0F;
          helicopters.emplace_back(HeliType::Scout, wave);
        }
      }

      if (spawn_queue.empty() && helicopters.empty()) {
        phase = Phase::Clear;
        phase_timer = 3.0F;

        int bonus = wave * 1000;
        banner = "WAVE CLEAR!";
        sub_banner = "BONUS " + with_commas(bonus);
        if (perfect_wave) {
          bonus += wave * 1000;
          sub_banner = "PERFECT! BONUS " + with_commas(bonus);
        }
        banner_warning = false;
        add_score(bonus);
        clear_enemy_bullets();
        audio::play("pickup", 0.8F);
      }
      break;
    }

    case Phase::Clear:
      if (phase_timer <= 0.0F) {
        start_wave(wave + 1);
      }
      break;
  }
}

void World::update(float dt) {
  time += dt;
  background.update(dt);

  // Freeze frames on big hits sell the impact
  if (hit_stop > 0.0F) {
    hit_stop -= dt;
    effects.update(0.0F);
    return;
  }

  effects.update(dt);

  if (game_over) {
    return;
  }

  player.update(dt, *this);

  for (auto& heli : helicopters) {
    heli.update(dt, *this);
  }

  for (auto& b : bullets) {
    b.update(dt);
  }

  for (auto& p : pickups) {
    p.update(dt);
  }

  for (auto& m : mines) {
    if (m.alive && m.update(dt, *this)) {
      explode_mine(m, false);
    }
  }

  update_collisions();

  std::erase_if(helicopters, [](const Helicopter& h) { return !h.is_alive(); });
  std::erase_if(bullets, [](const Bullet& b) { return !b.alive; });
  std::erase_if(mines, [](const Mine& m) { return !m.alive; });
  std::erase_if(pickups, [](const Pickup& p) { return !p.alive; });

  // Combo decays
  if (combo > 0) {
    combo_timer -= dt;
    if (combo_timer <= 0.0F) {
      combo = 0;
    }
  }

  // Respawn or end the game
  if (!player.is_alive()) {
    respawn_timer -= dt;
    if (respawn_timer <= 0.0F) {
      if (lives > 0) {
        player.respawn();
        clear_enemy_bullets();
      } else {
        game_over = true;
        audio::play("gameover", 0.9F);
      }
    }
  }

  update_waves(dt);
}

void World::update_collisions() {
  const auto player_box = player.get_hitbox();

  for (auto& b : bullets) {
    if (!b.alive) {
      continue;
    }

    const auto box = bullet_box(b);

    if (!b.from_player) {
      if (player.is_alive() && box.collides(player_box)) {
        if (player.hurt(b.damage, *this, b.pos)) {
          b.alive = false;
        }
      }
      continue;
    }

    for (auto& heli : helicopters) {
      if (!heli.is_alive() || !box.collides(heli.get_hitbox())) {
        continue;
      }
      b.alive = false;
      effects.sparks(b.pos, asw::Color(255, 230, 120), 5,
                     std::atan2(-b.vel.y, -b.vel.x), 0.8F, 250.0F);
      audio::play("hit", 0.3F, b.pos.x);
      add_score(5);
      if (heli.damage(b.damage)) {
        kill_helicopter(heli);
      }
      break;
    }

    if (!b.alive) {
      continue;
    }

    for (auto& m : mines) {
      if (m.alive && box.collides(m.get_bounds())) {
        b.alive = false;
        explode_mine(m, true);
        break;
      }
    }
  }

  // Pickups
  if (player.is_alive()) {
    const auto grab = player.get_pickup_box();
    for (auto& p : pickups) {
      if (p.alive && grab.collides(p.get_bounds())) {
        p.alive = false;
        player.give_power(p.type);
        const auto& info = get_pickup_info(p.type);
        effects.float_text(p.pos + asw::Vec2f(0.0F, -60.0F), info.label,
                           info.color, true);
        effects.sparks(p.pos + asw::Vec2f(0.0F, -15.0F), info.color, 20,
                       -PI / 2, PI, 350.0F);
        audio::play("pickup", 0.8F, p.pos.x);
        add_score(100);
      }
    }
  }
}

void World::laser_sweep(const asw::Vec2f& origin,
                        const asw::Vec2f& dir,
                        float length,
                        float damage) {
  const auto end = origin + (dir * length);

  for (auto& heli : helicopters) {
    if (!heli.is_alive()) {
      continue;
    }
    const float reach = heli.get_size().y * 0.35F;
    if (distance_to_segment(heli.get_center(), origin, end) < reach) {
      if (asw::random::chance(0.3F)) {
        effects.sparks(heli.get_center(), asw::Color(255, 150, 255), 3,
                       std::atan2(-dir.y, -dir.x), 1.0F, 250.0F);
      }
      if (heli.damage(damage)) {
        kill_helicopter(heli);
      }
    }
  }

  // The beam also burns enemy bullets out of the air
  for (auto& b : bullets) {
    if (b.alive && !b.from_player &&
        distance_to_segment(b.pos, origin, end) < b.radius + 6.0F) {
      b.alive = false;
      effects.sparks(b.pos, asw::Color(255, 120, 120), 4, 0.0F, PI, 150.0F);
      add_score(5);
    }
  }

  for (auto& m : mines) {
    if (m.alive && distance_to_segment(m.get_center(), origin, end) < 14.0F) {
      explode_mine(m, true);
    }
  }
}

void World::kill_helicopter(Helicopter& heli) {
  const auto center = heli.get_center();
  const bool boss = heli.get_type() == HeliType::Boss;

  const int before = get_multiplier();
  combo++;
  combo_timer = COMBO_TIME;
  const int multiplier = get_multiplier();
  if (multiplier > before) {
    effects.float_text(center + asw::Vec2f(0.0F, -40.0F),
                       "x" + std::to_string(multiplier) + " COMBO!", GOLD,
                       true);
    audio::play("combo", 0.7F, center.x);
  }

  const int points = heli.get_score() * multiplier;
  add_score(points);
  effects.float_text(center, "+" + with_commas(points),
                     asw::Color(255, 255, 255));

  const float scale = heli.get_size().x / 150.0F;
  effects.explosion(center, scale);
  effects.shake(boss ? 25.0F : 6.0F + (4.0F * scale));
  hit_stop = boss ? 0.3F : 0.035F;
  audio::play(boss ? "big_explosion" : "explosion", 0.8F, center.x);

  if (boss) {
    effects.chain_explosion(center, heli.get_size() * 0.5F, 10, 1.8F);
    effects.flash(asw::Color(255, 255, 255, 220), 0.5F);
    effects.float_text(asw::Vec2f(SCREEN_W / 2.0F, 300.0F), "BOSS DESTROYED!",
                       GOLD, true);
    clear_enemy_bullets();
  }

  drop_pickups(heli);
}

void World::drop_pickups(const Helicopter& heli) {
  float chance = 0.0F;
  int count = 1;
  switch (heli.get_type()) {
    case HeliType::Scout:
      chance = 0.12F;
      break;
    case HeliType::Bomber:
      chance = 0.3F;
      break;
    case HeliType::Gunship:
      chance = 0.38F;
      break;
    case HeliType::Boss:
      chance = 1.0F;
      count = 3;
      break;
  }

  if (!asw::random::chance(chance)) {
    return;
  }

  for (int i = 0; i < count; i++) {
    // Favour health when the player is hurting
    const bool low = player.get_health() < Player::MAX_HEALTH * 0.5F;
    const std::array<int, PICKUP_TYPE_COUNT> weights = {low ? 6 : 2, 3, 2, 3,
                                                        wave >= 2 ? 2 : 0};
    int total = 0;
    for (int w : weights) {
      total += w;
    }
    int roll = asw::random::between(0, total - 1);
    size_t type = 0;
    for (; type < weights.size(); type++) {
      roll -= weights.at(type);
      if (roll < 0) {
        break;
      }
    }

    const float offset = (static_cast<float>(i) - (count - 1) / 2.0F) * 90.0F;
    spawn_pickup(heli.get_center() + asw::Vec2f(offset, 0.0F),
                 static_cast<PickupType>(type));
  }
}

void World::explode_mine(Mine& mine, bool by_player) {
  if (!mine.alive) {
    return;
  }
  mine.alive = false;

  const auto center = mine.get_center();
  effects.explosion(center, 0.8F);
  effects.shake(8.0F);
  audio::play("explosion", 0.6F, center.x);

  if (player.is_alive() &&
      player.get_center().distance(center) < Mine::BLAST_RADIUS) {
    player.hurt(MINE_DAMAGE, *this, center);
  }

  if (by_player) {
    const int points = 50 * get_multiplier();
    add_score(points);
    effects.float_text(center + asw::Vec2f(0.0F, -20.0F),
                       "+" + std::to_string(points), asw::Color(255, 255, 255));
  }
}

void World::clear_enemy_bullets() {
  for (auto& b : bullets) {
    if (b.alive && !b.from_player) {
      b.alive = false;
      effects.sparks(b.pos, GOLD, 3, 0.0F, PI, 120.0F);
    }
  }
}

void World::spawn_bullet(const Bullet& bullet) {
  bullets.push_back(bullet);
}

void World::spawn_mine(const asw::Vec2f& pos, const asw::Vec2f& vel) {
  Mine m;
  m.pos = pos;
  m.vel = vel;
  mines.push_back(m);
}

void World::spawn_pickup(const asw::Vec2f& pos, PickupType type) {
  Pickup p;
  p.type = type;
  p.pos = {std::clamp(pos.x, 40.0F, SCREEN_W - 40.0F), pos.y};
  pickups.push_back(p);
}

void World::on_player_damaged() {
  perfect_wave = false;
  combo = 0;
}

void World::on_player_death() {
  perfect_wave = false;
  combo = 0;
  lives--;
  respawn_timer = 2.0F;

  const auto center = player.get_center();
  effects.explosion(center, 1.2F);
  effects.shake(22.0F);
  effects.flash(asw::Color(255, 60, 20, 160), 0.6F);
  hit_stop = 0.2F;
  audio::play("big_explosion", 0.9F, center.x);
}

void World::add_score(int points) {
  score += points;
  if (score > high_score) {
    high_score = score;
  }

  // Extra life every few thousand points
  while (score >= next_extra_life) {
    next_extra_life += EXTRA_LIFE_SCORE;
    if (lives < MAX_LIVES) {
      lives++;
      effects.float_text(player.get_center() + asw::Vec2f(0.0F, -50.0F), "1UP!",
                         asw::Color(120, 255, 120), true);
      audio::play("pickup", 1.0F);
    }
  }
}

int World::get_multiplier() const {
  if (combo <= 0) {
    return 1;
  }
  return std::min(MAX_MULTIPLIER, 1 + ((combo - 1) / 3));
}

void World::draw() const {
  background.draw();

  for (const auto& p : pickups) {
    p.draw();
  }
  for (const auto& m : mines) {
    m.draw();
  }
  for (const auto& h : helicopters) {
    h.draw();
  }

  player.draw();

  for (const auto& b : bullets) {
    b.draw();
  }

  effects.draw_particles();
  effects.draw_text();
}

void World::draw_banner() const {
  if (banner.empty()) {
    return;
  }

  // Slide in from the side
  const float total = phase == Phase::Intro ? 2.2F : 3.0F;
  const float t = std::clamp((total - phase_timer) / 0.35F, 0.0F, 1.0F);
  const float x = asw::easing::ease(-400.0F, SCREEN_W / 2.0F, t,
                                    asw::easing::ease_out_back);

  // Warning banner flashes
  if (banner_warning && std::fmod(time, 0.4F) < 0.2F) {
    asw::draw::rect_fill(asw::Quad<float>(0, 270, SCREEN_W, 120),
                         asw::Color(180, 0, 0, 120));
  } else {
    asw::draw::rect_fill(asw::Quad<float>(0, 270, SCREEN_W, 120),
                         asw::Color(0, 0, 0, 110));
  }

  const auto color = banner_warning ? asw::Color(255, 80, 60) : GOLD;
  gfx::text_shadow("font_xl", banner, asw::Vec2f(x, 290.0F), color,
                   asw::TextJustify::Center, 5.0F);
  gfx::text_shadow("font_m", sub_banner, asw::Vec2f(SCREEN_W - x, 355.0F),
                   asw::Color(255, 255, 255), asw::TextJustify::Center);
}

void World::draw_hud() const {
  // Score
  gfx::text_shadow("font_s", "SCORE", asw::Vec2f(20, 16), GOLD);
  gfx::text_shadow("font_l", with_commas(score), asw::Vec2f(20, 34),
                   asw::Color(255, 255, 255));
  gfx::text_shadow("font_s", "HI " + with_commas(high_score),
                   asw::Vec2f(20, 70), asw::Color(200, 200, 200));

  // Combo multiplier with its decay timer
  const int multiplier = get_multiplier();
  if (multiplier > 1) {
    const float pulse = 1.0F + (std::sin(time * 12.0F) * 0.5F);
    gfx::text_shadow("font_l", "x" + std::to_string(multiplier),
                     asw::Vec2f(20, 94 - pulse), GOLD);
    draw_bar(asw::Quad<float>(90, 102, 80, 8), combo_timer / COMBO_TIME, GOLD,
             asw::Color(60, 50, 0));
  }

  // Wave
  gfx::text_shadow("font_m", "WAVE " + std::to_string(wave),
                   asw::Vec2f(SCREEN_W / 2.0F, 18), asw::Color(255, 255, 255),
                   asw::TextJustify::Center);

  // Boss health
  for (const auto& h : helicopters) {
    if (h.get_type() == HeliType::Boss) {
      gfx::text_shadow(
          "font_s", "IRON STALLION", asw::Vec2f(SCREEN_W / 2.0F, 46),
          asw::Color(255, 100, 80), asw::TextJustify::Center, 2.0F);
      draw_bar(asw::Quad<float>((SCREEN_W / 2.0F) - 250, 64, 500, 14),
               h.get_health() / h.get_max_health(), asw::Color(255, 60, 40));
    }
  }

  // Health
  const float right = SCREEN_W - 20.0F;
  const float health = player.get_health() / Player::MAX_HEALTH;
  const bool danger = health < 0.3F && std::fmod(time, 0.5F) < 0.25F;
  const auto health_color = health > 0.5F   ? asw::Color(80, 230, 80)
                            : health > 0.3F ? asw::Color(240, 200, 40)
                                            : asw::Color(240, 50, 40);
  gfx::text_shadow("font_s", "HEALTH", asw::Vec2f(right - 220, 16),
                   danger ? asw::Color(255, 60, 60) : GOLD);
  draw_bar(asw::Quad<float>(right - 220, 34, 220, 18), health, health_color);

  // Dash cooldown
  const float dash = player.get_dash_ready();
  draw_bar(asw::Quad<float>(right - 220, 58, 220, 5), dash,
           dash >= 1.0F ? asw::Color(120, 220, 255) : asw::Color(60, 110, 140),
           asw::Color(20, 30, 40));

  // Lives
  const auto icon = asw::assets::get_texture("player");
  for (int i = 0; i < lives; i++) {
    asw::draw::stretch_sprite(
        icon, asw::Quad<float>(right - 30 - (static_cast<float>(i) * 34), 70,
                               30, 28));
  }

  // Active power-ups
  float y = 110.0F;
  for (int i = 1; i < PICKUP_TYPE_COUNT; i++) {
    const auto type = static_cast<PickupType>(i);
    const float remaining = player.get_power_time(type);
    if (remaining <= 0.0F) {
      continue;
    }
    const auto& info = get_pickup_info(type);
    // Blink when about to run out
    if (remaining > 2.0F || std::fmod(remaining, 0.3F) < 0.15F) {
      gfx::text_shadow("font_s", info.label, asw::Vec2f(right, y), info.color,
                       asw::TextJustify::Right, 2.0F);
    }
    draw_bar(asw::Quad<float>(right - 140, y + 16, 140, 5),
             std::min(1.0F, remaining / 10.0F), info.color,
             asw::Color(30, 30, 30));
    y += 32.0F;
  }

  draw_banner();
}
