#!/usr/bin/env python3
"""Generate the chiptune sound effects and music loop for Gunner.

Run from the repository root: python3 tools/gen_sounds.py
Writes WAV files to assets/sounds/. Uses only the standard library.
"""

import math
import os
import random
import struct
import wave

RATE = 22050
OUT = os.path.join("assets", "sounds")

random.seed(7)


def square(phase, duty=0.5):
    return 1.0 if (phase % 1.0) < duty else -1.0


def triangle(phase):
    p = phase % 1.0
    return 4.0 * p - 1.0 if p < 0.5 else 3.0 - 4.0 * p


def saw(phase):
    return 2.0 * (phase % 1.0) - 1.0


def noise(_phase):
    return random.uniform(-1.0, 1.0)


def tone(duration, f0, f1=None, wave_fn=square, vol=0.5, attack=0.005,
         curve=1.0, duty=0.5, vibrato=0.0):
    """Pitch sweep from f0 to f1 with an exponential-ish decay envelope."""
    f1 = f0 if f1 is None else f1
    n = int(duration * RATE)
    out = []
    phase = 0.0
    for i in range(n):
        t = i / n
        freq = f0 + (f1 - f0) * t
        if vibrato:
            freq *= 1.0 + 0.03 * math.sin(i / RATE * vibrato * 2 * math.pi)
        phase += freq / RATE
        env = min(1.0, (i / RATE) / attack) * (1.0 - t) ** curve
        if wave_fn is square:
            s = square(phase, duty)
        else:
            s = wave_fn(phase)
        out.append(s * env * vol)
    return out


def hum(duration, freq, wave_fn=square, vol=0.5, duty=0.5, wobble=0.0):
    """Steady tone with no envelope that loops without a click.

    Keep duration * freq and duration * wobble whole numbers so every cycle
    ends where the loop starts again.
    """
    n = int(duration * RATE)
    out = []
    phase = 0.0
    for i in range(n):
        f = freq
        if wobble:
            f *= 1.0 + 0.03 * math.sin(i / RATE * wobble * 2 * math.pi)
        phase += f / RATE
        if wave_fn is square:
            s = square(phase, duty)
        else:
            s = wave_fn(phase)
        out.append(s * vol)
    return out


def crushed_noise(duration, vol=0.6, hold_start=1, hold_end=12, curve=1.5):
    """Sample-and-hold noise; a growing hold lowers the pitch over time."""
    n = int(duration * RATE)
    out = []
    held = 0.0
    for i in range(n):
        t = i / n
        hold = int(hold_start + (hold_end - hold_start) * t)
        if i % max(1, hold) == 0:
            held = random.uniform(-1.0, 1.0)
        out.append(held * vol * (1.0 - t) ** curve)
    return out


def mix(*tracks):
    n = max(len(t) for t in tracks)
    out = [0.0] * n
    for t in tracks:
        for i, s in enumerate(t):
            out[i] += s
    return out


def concat(*tracks):
    out = []
    for t in tracks:
        out.extend(t)
    return out


def write(name, samples):
    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, name)
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767))
            for s in samples)
        w.writeframes(frames)
    print("wrote", path)


def note_freq(semitone):
    """Semitone offset from A4."""
    return 440.0 * 2 ** (semitone / 12.0)


def sfx():
    write("shoot.wav", tone(0.07, 1400, 500, square, 0.25, duty=0.25))
    write("enemy_shoot.wav", tone(0.09, 600, 250, square, 0.18, duty=0.5))
    write("laser.wav", mix(tone(0.3, 180, 160, saw, 0.3, curve=0.4),
                           tone(0.3, 900, 1100, square, 0.08, curve=0.4,
                                duty=0.125)))
    write("hit.wav", mix(crushed_noise(0.06, 0.35, 1, 3),
                         tone(0.06, 300, 120, square, 0.2)))
    write("explosion.wav", mix(crushed_noise(0.7, 0.7, 2, 30, 1.8),
                               tone(0.5, 140, 40, triangle, 0.6, curve=2)))
    write("big_explosion.wav", mix(crushed_noise(1.6, 0.8, 3, 60, 1.4),
                                   tone(1.4, 110, 25, triangle, 0.7,
                                        curve=1.6)))
    write("pickup.wav", concat(tone(0.06, note_freq(3), None, square, 0.25,
                                    duty=0.25, curve=0.3),
                               tone(0.06, note_freq(7), None, square, 0.25,
                                    duty=0.25, curve=0.3),
                               tone(0.06, note_freq(10), None, square, 0.25,
                                    duty=0.25, curve=0.3),
                               tone(0.14, note_freq(15), None, square, 0.25,
                                    duty=0.25)))
    write("jump.wav", tone(0.14, 250, 700, square, 0.18, duty=0.25))
    write("hurt.wav", mix(tone(0.25, 500, 90, square, 0.3, duty=0.5),
                          crushed_noise(0.12, 0.25, 1, 4)))
    write("select.wav", tone(0.05, 1200, 1200, square, 0.2, duty=0.25,
                             curve=0.5))
    write("combo.wav", tone(0.12, 800, 1600, triangle, 0.3, curve=0.6))
    write("wave.wav", concat(
        tone(0.12, note_freq(0), None, square, 0.25, curve=0.2),
        tone(0.12, note_freq(4), None, square, 0.25, curve=0.2),
        tone(0.12, note_freq(7), None, square, 0.25, curve=0.2),
        tone(0.4, note_freq(12), None, square, 0.25, curve=0.8,
             vibrato=6)))
    write("boss.wav", concat(
        tone(0.25, note_freq(-12), None, saw, 0.35, curve=0.3),
        tone(0.25, note_freq(-11), None, saw, 0.35, curve=0.3),
        tone(0.25, note_freq(-12), None, saw, 0.35, curve=0.3),
        tone(0.6, note_freq(-6), None, saw, 0.35, curve=0.8, vibrato=5)))
    write("gameover.wav", concat(
        tone(0.25, note_freq(7), None, square, 0.25, curve=0.3),
        tone(0.25, note_freq(3), None, square, 0.25, curve=0.3),
        tone(0.25, note_freq(0), None, square, 0.25, curve=0.3),
        tone(0.9, note_freq(-5), note_freq(-7), square, 0.25, curve=1.0,
             vibrato=5)))
    write("mine.wav", tone(0.1, 200, 200, square, 0.15, duty=0.125))
    # Held laser, looped while the beam is on
    write("laser_loop.wav", mix(hum(1.0, 170, saw, 0.22, wobble=6),
                                hum(1.0, 1000, square, 0.06, duty=0.125,
                                    wobble=6)))


def music():
    bpm = 150
    step = 60.0 / bpm / 4  # sixteenth note
    steps_per_bar = 16
    # A minor: Am, F, C, G then Am, F, G, E (in semitones from A4)
    chords = [
        (0, [0, 3, 7]), (-4, [0, 4, 7]), (3, [0, 4, 7]), (-2, [0, 4, 7]),
        (0, [0, 3, 7]), (-4, [0, 4, 7]), (-2, [0, 4, 7]), (-5, [0, 4, 7]),
    ]
    bars = len(chords)
    total = int(step * steps_per_bar * bars * RATE)
    out = [0.0] * total
    step_n = int(step * RATE)

    def place(samples, start):
        for i, s in enumerate(samples):
            if start + i < total:
                out[start + i] += s

    # Lead melody, one entry per eighth note (None is a rest)
    lead = [
        12, None, 15, 12, 19, None, 17, 15,
        12, None, 8, None, 12, 15, 17, None,
        15, None, 19, 15, 22, None, 20, 19,
        17, None, 14, None, 15, 17, 19, None,
        12, None, 15, 12, 19, None, 17, 15,
        20, None, 17, None, 20, 22, 24, None,
        22, None, 19, None, 17, 19, 22, None,
        19, 17, 16, None, 11, None, 16, None,
    ]

    for bar, (root, shape) in enumerate(chords):
        bar_start = bar * steps_per_bar * step_n
        for s in range(steps_per_bar):
            start = bar_start + s * step_n
            # Driving bass on eighths, octave jump on the off-beat
            if s % 2 == 0:
                octave = -24 if s % 4 == 0 else -12
                place(tone(step * 1.8, note_freq(root + octave), None,
                           triangle, 0.45, curve=0.6), start)
            # Arpeggio on sixteenths
            semis = root + shape[s % 3] + (12 if s % 6 >= 3 else 0)
            place(tone(step * 0.9, note_freq(semis), None, square, 0.07,
                       duty=0.125, curve=1.2), start)
            # Drums: kick on beats, snare on 2 and 4, hats on off-sixteenths
            if s % 4 == 0:
                place(tone(0.12, 160, 40, triangle, 0.6, curve=1.5), start)
            if s % 8 == 4:
                place(crushed_noise(0.14, 0.28, 1, 2, 1.6), start)
            if s % 2 == 1:
                place(crushed_noise(0.03, 0.08, 1, 1, 2.0), start)
        # Lead
        for e in range(8):
            n = lead[bar * 8 + e]
            if n is not None:
                place(tone(step * 1.9, note_freq(n), None, square, 0.12,
                           duty=0.25, curve=0.7, vibrato=5),
                      bar_start + e * 2 * step_n)

    peak = max(abs(s) for s in out) or 1.0
    write("music.wav", [s / peak * 0.8 for s in out])


if __name__ == "__main__":
    sfx()
    music()
