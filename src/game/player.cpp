#include "./player.h"

#include <algorithm>
#include <cmath>

#include "../controls.h"
#include "../globals.h"
#include "./world.h"

namespace {

constexpr float MOVE_SPEED = 400.0F;
constexpr float GROUND_ACCEL = 4200.0F;
constexpr float AIR_ACCEL = 2600.0F;
constexpr float FRICTION = 3200.0F;
constexpr float GRAVITY = 2400.0F;
constexpr float JUMP_SPEED = 820.0F;
constexpr float DOUBLE_JUMP_SPEED = 720.0F;

constexpr float DASH_SPEED = 1150.0F;
constexpr float DASH_TIME = 0.16F;
constexpr float DASH_COOLDOWN = 0.8F;

constexpr float FIRE_DELAY = 0.17F;
constexpr float RAPID_FIRE_DELAY = 0.065F;
constexpr float BULLET_SPEED = 950.0F;
constexpr float BULLET_DAMAGE = 10.0F;
constexpr float SPREAD_ANGLE = 0.14F;

constexpr float LASER_DPS = 150.0F;
constexpr float LASER_LENGTH = 1400.0F;

constexpr float INVULN_TIME = 1.0F;
constexpr float RESPAWN_INVULN_TIME = 2.5F;

constexpr float GUN_LENGTH = 26.0F;

constexpr std::array<float, PICKUP_TYPE_COUNT> POWER_DURATIONS = {
    0.0F,   // Health is instant
    8.0F,   // Rapid fire
    10.0F,  // Ricochet
    10.0F,  // Spread
    6.0F,   // Laser
};

// Pan for the laser hum as the gun moves. asw::sound::play_at pans once, when
// the sound starts, so the playing loop uses the same curve here. The gun is
// always on screen, so there is no edge fade to match.
float laser_pan(float x) {
  const float center = SCREEN_W / 2.0F;
  return std::clamp(((x - center) / center) * 0.7F, -1.0F, 1.0F);
}

// Filled bar from a to b, for the beam and the gun barrel
void draw_bar(const asw::Vec2f& a,
              const asw::Vec2f& b,
              float thickness,
              asw::Color color) {
  const auto mid = (a + b) * 0.5F;
  const float length = a.distance(b);
  asw::draw::rect_fill_rotate(
      asw::Quad<float>(mid.x - (length / 2.0F), mid.y - (thickness / 2.0F),
                       length, thickness),
      (b - a).angle(), color);
}

}  // namespace

void Player::reset() {
  health = MAX_HEALTH;
  alive = true;
  power_timers.fill(0.0F);
  ghosts.clear();
  respawn();
  invuln_timer = 0.0F;
}

void Player::respawn() {
  pos = {(SCREEN_W - WIDTH) / 2.0F, GROUND_Y - HEIGHT};
  vel = {0.0F, 0.0F};
  health = MAX_HEALTH;
  alive = true;
  on_ground = true;
  jumps_left = 2;
  fire_timer = 0.0F;
  hurt_timer = 0.0F;
  dash_timer = 0.0F;
  dash_cooldown = 0.0F;
  set_laser(false);
  invuln_timer = RESPAWN_INVULN_TIME;
}

asw::Vec2f Player::get_gun_tip() const {
  const auto shoulder = get_center() + asw::Vec2f(0.0F, -2.0F);
  return shoulder + asw::Vec2f::from_angle(aim, GUN_LENGTH);
}

float Player::get_dash_ready() const {
  return std::clamp(1.0F - (dash_cooldown / DASH_COOLDOWN), 0.0F, 1.0F);
}

void Player::update(float dt, World& world) {
  using asw::input::get_action;
  using asw::input::get_action_down;
  using asw::input::get_action_strength;

  // Ghosts fade even while dead
  for (auto& g : ghosts) {
    g.life -= dt;
  }
  std::erase_if(ghosts, [](const Ghost& g) { return g.life <= 0.0F; });

  if (!alive) {
    set_laser(false);
    return;
  }

  // Timers
  fire_timer -= dt;
  hurt_timer -= dt;
  invuln_timer -= dt;
  dash_timer -= dt;
  dash_cooldown -= dt;
  for (auto& t : power_timers) {
    t = std::max(0.0F, t - dt);
  }

  // Horizontal input, analog on a stick
  const float input =
      get_action_strength("right") - get_action_strength("left");

  // Aim at the mouse, or along the right stick. With the stick released, aim
  // the way the player walks, or keep the last aim when standing still.
  if (controls::using_pad()) {
    const auto stick = asw::input::get_controller_stick(
        asw::input::ANY_CONTROLLER, asw::input::ControllerStick::Right);
    if (stick.x != 0.0F || stick.y != 0.0F) {
      aim = stick.angle();
    } else if (input != 0.0F) {
      aim = input < 0.0F ? PI : 0.0F;
    }
    facing_left = std::cos(aim) < 0.0F;
  } else {
    const auto mouse = asw::input::get_mouse().position;
    const auto shoulder = get_center() + asw::Vec2f(0.0F, -2.0F);
    aim = (mouse - shoulder).angle();
    facing_left = mouse.x < shoulder.x;
  }

  // Dash
  if (get_action_down("dash") && dash_cooldown <= 0.0F) {
    dash_dir = input != 0.0F ? input : (facing_left ? -1.0F : 1.0F);
    dash_timer = DASH_TIME;
    dash_cooldown = DASH_COOLDOWN;
    vel.y = std::min(vel.y, 0.0F);
    audio::play("jump", 0.6F, pos.x);
    world.get_effects().sparks(get_center(), asw::Color(160, 230, 255), 10,
                               dash_dir > 0 ? PI : 0.0F, 0.5F, 300.0F);
  }

  if (dashing()) {
    vel.x = dash_dir * DASH_SPEED;
    vel.y = 0.0F;
    ghosts.push_back({pos, 0.25F, facing_left});
  } else {
    const float accel = on_ground ? GROUND_ACCEL : AIR_ACCEL;
    if (input != 0.0F) {
      vel.x += input * accel * dt;
      vel.x = std::clamp(vel.x, -MOVE_SPEED, MOVE_SPEED);
    } else if (on_ground) {
      const float drop = FRICTION * dt;
      vel.x =
          std::abs(vel.x) <= drop ? 0.0F : vel.x - (std::copysign(drop, vel.x));
    }
    // Leftover dash speed bleeds off
    if (std::abs(vel.x) > MOVE_SPEED) {
      vel.x -= std::copysign(FRICTION * dt, vel.x);
    }

    // Jump and double jump
    if (get_action_down("jump") && jumps_left > 0) {
      vel.y = on_ground ? -JUMP_SPEED : -DOUBLE_JUMP_SPEED;
      if (!on_ground) {
        world.get_effects().sparks(asw::Vec2f(get_center().x, pos.y + HEIGHT),
                                   asw::Color(255, 255, 255), 8, PI / 2, 0.8F,
                                   200.0F);
      }
      jumps_left--;
      on_ground = false;
      audio::play("jump", 0.5F, pos.x);
    }

    // Let go early for a short hop
    if (!get_action("jump") && vel.y < -300.0F) {
      vel.y = -300.0F;
    }

    vel.y += GRAVITY * dt;
  }

  pos += vel * dt;

  // Walls
  if (pos.x < 0.0F) {
    pos.x = 0.0F;
    vel.x = 0.0F;
  }
  if (pos.x > SCREEN_W - WIDTH) {
    pos.x = SCREEN_W - WIDTH;
    vel.x = 0.0F;
  }

  // Ground
  if (pos.y >= GROUND_Y - HEIGHT) {
    if (!on_ground && vel.y > 600.0F) {
      world.get_effects().sparks(asw::Vec2f(get_center().x, GROUND_Y),
                                 asw::Color(230, 120, 40), 6, -PI / 2, 1.3F,
                                 180.0F);
    }
    pos.y = GROUND_Y - HEIGHT;
    vel.y = 0.0F;
    on_ground = true;
    jumps_left = 2;
  }

  // Shooting
  const bool trigger = get_action("fire");
  set_laser(trigger && get_power_time(PickupType::Laser) > 0.0F);

  if (laser_on) {
    update_laser(dt, world);
  } else if (trigger && fire_timer <= 0.0F) {
    fire(world);
  }
}

void Player::fire(World& world) {
  const bool rapid = get_power_time(PickupType::RapidFire) > 0.0F;
  const bool spread = get_power_time(PickupType::Spread) > 0.0F;
  const bool ricochet = get_power_time(PickupType::Ricochet) > 0.0F;

  fire_timer = rapid ? RAPID_FIRE_DELAY : FIRE_DELAY;

  const auto tip = get_gun_tip();
  const int shots = spread ? 3 : 1;

  for (int i = 0; i < shots; i++) {
    const float offset =
        spread ? static_cast<float>(i - 1) * SPREAD_ANGLE : 0.0F;
    // A little inaccuracy when spraying rapid fire
    const float jitter = rapid ? asw::random::between(-0.04F, 0.04F) : 0.0F;
    const float angle = aim + offset + jitter;

    Bullet b;
    b.pos = tip;
    b.vel = asw::Vec2f::from_angle(angle, BULLET_SPEED);
    b.radius = 3.0F;
    b.damage = BULLET_DAMAGE;
    b.from_player = true;
    b.bounces = ricochet ? 3 : 0;
    b.life = ricochet ? 3.0F : 1.5F;
    world.spawn_bullet(b);
  }

  world.get_effects().muzzle_flash(tip, aim);
  audio::play("shoot", rapid ? 0.35F : 0.5F, tip.x);
}

void Player::update_laser(float dt, World& world) {
  const auto tip = get_gun_tip();
  const auto dir = asw::Vec2f::from_angle(aim);
  world.laser_sweep(tip, dir, LASER_LENGTH, LASER_DPS * dt);

  // Restart the hum if a louder sound took its voice
  if (!laser_sound.is_playing()) {
    laser_sound = audio::loop("laser_loop", 0.3F, tip.x);
  }
  laser_sound.set_pan(laser_pan(tip.x));

  if (asw::random::chance(0.5F)) {
    world.get_effects().sparks(tip, asw::Color(255, 120, 255), 1, aim, 0.4F,
                               250.0F);
  }
  world.get_effects().shake(1.5F);
}

void Player::set_laser(bool on) {
  if (on && !laser_on) {
    const auto tip = get_gun_tip();
    audio::play("laser", 0.4F, tip.x);
    laser_sound = audio::loop("laser_loop", 0.3F, tip.x);
  } else if (!on && laser_on) {
    laser_sound.stop(0.08F);
  }
  laser_on = on;
}

void Player::pause_sounds(bool paused) {
  if (paused) {
    laser_sound.pause();
  } else {
    laser_sound.resume();
  }
}

void Player::stop_sounds() {
  set_laser(false);
}

bool Player::hurt(float damage, World& world, const asw::Vec2f& from) {
  if (!alive || is_invulnerable()) {
    return false;
  }

  health -= damage;
  hurt_timer = 0.15F;
  invuln_timer = INVULN_TIME;

  // Knock away from the source
  const float away = get_center().x < from.x ? -1.0F : 1.0F;
  vel.x = away * 350.0F;
  vel.y = -380.0F;
  on_ground = false;

  world.get_effects().shake(10.0F);
  world.get_effects().flash(asw::Color(255, 0, 0, 90), 0.25F);
  world.get_effects().sparks(get_center(), asw::Color(255, 60, 60), 14,
                             away > 0 ? 0.0F : PI, 1.2F, 350.0F);
  audio::play("hurt", 0.8F, pos.x);

  if (health <= 0.0F) {
    health = 0.0F;
    alive = false;
    set_laser(false);
    world.on_player_death();
  } else {
    world.on_player_damaged();
  }

  return true;
}

void Player::heal(float amount) {
  health = std::min(MAX_HEALTH, health + amount);
}

void Player::give_power(PickupType type) {
  if (type == PickupType::Health) {
    heal(35.0F);
    return;
  }
  auto& timer = power_timers.at(static_cast<size_t>(type));
  timer = std::max(timer, 0.0F) + POWER_DURATIONS.at(static_cast<size_t>(type));
}

void Player::draw() const {
  const auto player_tex = asw::assets::get_texture("player");

  // Dash afterimages
  for (const auto& g : ghosts) {
    asw::draw::set_alpha(player_tex, g.life * 1.6F);
    asw::draw::set_tint(player_tex, asw::Color(120, 220, 255));
    asw::draw::stretch_sprite_rotate(player_tex,
                                     asw::Quad<float>(g.pos, {WIDTH, HEIGHT}),
                                     0.0F, g.facing_left);
  }
  asw::draw::set_alpha(player_tex, 1.0F);
  asw::draw::set_tint(player_tex, asw::Color(255, 255, 255));

  if (!alive) {
    return;
  }

  // Blink while invulnerable
  if (invuln_timer > 0.0F && !dashing() &&
      std::fmod(invuln_timer, 0.16F) < 0.08F) {
    return;
  }

  // Laser beam behind the player
  if (laser_on) {
    const auto tip = get_gun_tip();
    const auto dir = asw::Vec2f::from_angle(aim);
    const asw::Vec2f normal(-dir.y, dir.x);
    const auto end = tip + (dir * LASER_LENGTH);
    const float wobble = asw::random::between(0.0F, 2.0F);

    // The beam texture has the beam on its left half, so rotating it half a
    // turn past the aim points the beam from the gun outwards
    const auto beam = asw::assets::get_texture("laserbeam");
    asw::draw::stretch_sprite_rotate(
        beam, asw::Quad<float>(tip.x - 800.0F, tip.y - 20.0F, 1600.0F, 40.0F),
        aim + PI, false);

    // Soft purple edge, pink body and a white hot core
    const auto offset = normal * (wobble - 1.0F);
    draw_bar(tip + offset, end + offset, 9.0F, asw::Color(200, 40, 200, 160));
    draw_bar(tip + offset, end + offset, 5.0F, asw::Color(255, 150, 255));
    draw_bar(tip + offset, end + offset, 3.0F, asw::Color(255, 255, 255));
    asw::draw::circle_fill(tip, 7.0F + wobble, asw::Color(255, 200, 255));
  }

  // Sprite faces right
  const auto tex =
      hurt_timer > 0.0F ? asw::assets::get_texture("player_hurt") : player_tex;
  asw::draw::stretch_sprite_rotate(tex, asw::Quad<float>(pos, {WIDTH, HEIGHT}),
                                   0.0F, facing_left);

  // Gun barrel
  const auto shoulder = get_center() + asw::Vec2f(0.0F, -2.0F);
  const auto tip = get_gun_tip();
  draw_bar(shoulder, tip, 5.0F, asw::Color(20, 20, 20));
  draw_bar(shoulder, tip, 3.0F, asw::Color(70, 70, 80));
}
