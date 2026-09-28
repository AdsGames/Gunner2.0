#include "./entities.h"

#include <algorithm>
#include <cmath>

#include "../globals.h"
#include "./world.h"

// --- Bullet ---

void Bullet::update(float dt) {
  pos += vel * dt;
  life -= dt;

  // Ricochet bullets bounce off the walls, ceiling and ground
  if (bounces > 0) {
    bool bounced = false;
    if (pos.x < radius || pos.x > SCREEN_W - radius) {
      vel.x = -vel.x;
      pos.x = std::clamp(pos.x, radius, SCREEN_W - radius);
      bounced = true;
    }
    if (pos.y < radius || pos.y > GROUND_Y - radius) {
      vel.y = -vel.y;
      pos.y = std::clamp(pos.y, radius, GROUND_Y - radius);
      bounced = true;
    }
    if (bounced) {
      bounces--;
    }
  }

  const float margin = 40.0F;
  if (life <= 0.0F || pos.x < -margin || pos.x > SCREEN_W + margin ||
      pos.y < -margin || pos.y > GROUND_Y + 4.0F) {
    alive = false;
  }
}

void Bullet::draw() const {
  if (from_player) {
    // Short tracer behind the bullet
    const auto tail = pos - (vel * 0.025F);
    const auto color =
        bounces > 0 ? asw::Color(120, 255, 120) : asw::Color(255, 240, 120);
    asw::draw::line(tail, pos, color);
    asw::draw::line(tail + asw::Vec2f(0, 1), pos + asw::Vec2f(0, 1), color);
    asw::draw::rect_fill(asw::Quad<float>(pos.x - radius, pos.y - radius,
                                          radius * 2.0F, radius * 2.0F),
                         asw::Color(255, 255, 255));
  } else {
    asw::draw::circle_fill(pos, radius + 2.0F, asw::Color(120, 0, 0));
    asw::draw::circle_fill(pos, radius, asw::Color(255, 40, 40));
    asw::draw::circle_fill(pos, radius * 0.4F, asw::Color(255, 220, 200));
  }
}

// --- Mine ---

bool Mine::update(float dt, World& world) {
  if (!landed) {
    vel.y += 900.0F * dt;
    pos += vel * dt;
    if (pos.y >= GROUND_Y) {
      pos.y = GROUND_Y;
      vel = {0.0F, 0.0F};
      landed = true;
      world.get_effects().sparks(pos, asw::Color(200, 160, 100), 8, -PI / 2,
                                 1.2F, 200.0F);
    }
    return false;
  }

  fuse -= dt;

  // Blink faster as the fuse burns down
  const float rate = fuse < 1.5F ? 0.1F : 0.4F;
  blink += dt;
  if (blink > rate) {
    blink = 0.0F;
    if (fuse < 1.5F) {
      audio::play("mine", 0.3F, pos.x);
    }
  }

  const auto player_center = world.get_player().get_center();
  const bool near = player_center.distance(get_center()) < TRIGGER_RANGE &&
                    world.get_player().is_alive();

  // Arm time so a mine cannot go off the instant it lands on the player
  if (near && fuse < 4.4F) {
    fuse = std::min(fuse, 0.25F);
  }

  return fuse <= 0.0F;
}

void Mine::draw() const {
  const auto bounds = get_bounds();
  asw::draw::sprite(asw::assets::get_texture("mine"), bounds.position);

  // Warning light
  const bool lit = landed && blink < (fuse < 1.5F ? 0.05F : 0.2F);
  asw::draw::circle_fill(
      asw::Vec2f(pos.x, bounds.position.y + 4.0F), 3.0F,
      lit ? asw::Color(255, 60, 60) : asw::Color(90, 20, 20));
}

// --- Pickup ---

const PickupInfo& get_pickup_info(PickupType type) {
  static const std::array<PickupInfo, PICKUP_TYPE_COUNT> infos = {{
      {"crate_health", "HEALTH", asw::Color(80, 255, 80)},
      {"crate_rapidfire", "RAPID FIRE", asw::Color(255, 200, 60)},
      {"crate_ricochet", "RICOCHET", asw::Color(120, 255, 120)},
      {"crate", "SPREAD SHOT", asw::Color(120, 200, 255)},
      {"box_laserbeam", "LASER", asw::Color(255, 80, 255)},
  }};
  return infos.at(static_cast<size_t>(type));
}

asw::Quad<float> Pickup::get_bounds() const {
  const auto size = asw::util::get_texture_size(
      asw::assets::get_texture(get_pickup_info(type).texture));
  return {pos.x - (size.x / 2.0F), pos.y - size.y, size.x, size.y};
}

void Pickup::update(float dt) {
  if (!landed) {
    // Parachute drift
    sway += dt * 3.0F;
    vel_y = std::min(vel_y + (400.0F * dt), 110.0F);
    pos.y += vel_y * dt;
    pos.x += std::sin(sway) * 30.0F * dt;
    pos.x = std::clamp(pos.x, 40.0F, SCREEN_W - 40.0F);
    if (pos.y >= GROUND_Y) {
      pos.y = GROUND_Y;
      landed = true;
    }
  } else {
    life -= dt;
    if (life <= 0.0F) {
      alive = false;
    }
  }
}

void Pickup::draw() const {
  // Blink out before vanishing
  if (landed && life < 2.0F && std::fmod(life, 0.2F) < 0.1F) {
    return;
  }

  const auto bounds = get_bounds();
  const auto& info = get_pickup_info(type);

  if (!landed) {
    // Parachute
    const auto top = bounds.position;
    const asw::Vec2f canopy(pos.x, top.y - 48.0F);
    asw::draw::line(top, canopy + asw::Vec2f(-30, 4), asw::Color(60, 60, 60));
    asw::draw::line(top + asw::Vec2f(bounds.size.x, 0),
                    canopy + asw::Vec2f(30, 4), asw::Color(60, 60, 60));
    asw::draw::rect_fill(asw::Quad<float>(canopy.x - 34, canopy.y - 10, 68, 12),
                         asw::Color(240, 240, 240));
    asw::draw::rect_fill(asw::Quad<float>(canopy.x - 24, canopy.y - 18, 48, 8),
                         asw::Color(240, 240, 240));
    asw::draw::rect_fill(asw::Quad<float>(canopy.x - 34, canopy.y - 10, 12, 12),
                         info.color);
    asw::draw::rect_fill(asw::Quad<float>(canopy.x + 22, canopy.y - 10, 12, 12),
                         info.color);
  }

  // Pulsing outline so crates read as collectable
  const float pulse = (std::sin((sway + life) * 8.0F) * 0.5F) + 0.5F;
  asw::draw::rect(
      asw::Quad<float>(bounds.position.x - 3, bounds.position.y - 3,
                       bounds.size.x + 6, bounds.size.y + 6),
      info.color.with_alpha(static_cast<uint8_t>(120 + (135 * pulse))));
  asw::draw::sprite(asw::assets::get_texture(info.texture), bounds.position);
}
