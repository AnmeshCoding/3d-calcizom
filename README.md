# 3D Scientific Calculator vs Zombie Horde (100% Pure C)

A complete first-person 3D action defense game built entirely in **pure C (C99)** using the Win32 API and OpenGL. **Zero C++ was needed!**

---

## 🧟‍♂️ What's New & Features

1. **Enlarged Scientific Calculator**:
   - **No more clipping**: Elevated cleanly onto the command desk tabletop with an ergonomic tilt.
   - **Bigger & Chunkier**: 1.4x larger physical keys and wide panoramic 7-segment digital screen.
   - **Scientific Functions**:
     - `SIN`, `COS`, `TAN` (Trigonometric functions)
     - `SQRT` (Square roots)
     - `X^2` (Power/Squaring)
     - `PI` (Constant `3.1415927`)
     - Standard: `0-9`, `.`, `+`, `-`, `*`, `/`, `=`, `C`, `CE`, `+/-`

2. **First-Person Combat Blaster**:
   - **3D Gun Model**: Visible plasma pistol in hand with recoil kick, muzzle flash, and dynamic lighting.
   - **Semi-Automatic Shooting**: Press **Left Mouse Button (LMB)** to shoot incoming zombies.
   - **Realistic Gunfire Audio**: Procedural synthesized gunshot snaps and bass impacts.

3. **Zombie Horde AI**:
   - **Procedural 3D Shamblers**: Decayed green voxel humanoids with glowing crimson eyes, forward-outstretched shambling arms, and walking legs.
   - **Wave Spawner**: Zombies spawn around the room perimeter and hunt the player down.
   - **Damage & Death**: Hit flash animations, health bars, and death collapse animations.
   - **Melee Attack**: When close to the player, zombies attack and deplete health.

4. **Math Defense Objectives & EMP Nuke Shockwave**:
   - **Math Missions**: The HUD displays active defense questions (e.g. `SOLVE: [ 7 * 8 = ? ]` or `SQRT(64)`).
   - **Trigger EMP Nuke**: Compute the answer on the calculator and hit `=` (or type it):
     - An **EMP Shockwave** detonates, instantly disintegrating the entire zombie horde!
     - Heals your health by **+35 HP**!
     - Awards **+500 Points** + bonus score per zombie wiped out!
     - Advances the wave!

---

## 🎮 Controls

| Key / Input | Action |
|-------------|--------|
| **W, A, S, D** | Move / Walk around the room |
| **Mouse** | Aim & Look around (First-person mouselook) |
| **Left Click (LMB)** | **FIRE WEAPON** (Shoot zombies!) |
| **E** | **PRESS CALCULATOR BUTTON** you are aiming at |
| **Space** | Jump |
| **Tab / F** | Toggle **Desk Focus Mode** (Free mouse clicking on keys) |
| **Esc** | Unlock / Lock mouse cursor |
| **R** | Restart mission when Game Over |
| **Numpad / Keyboard** | Type directly on calculator (`0-9`, `+`, `-`, `*`, `/`, `Enter`, `C`) |

---

## 🚀 How to Run

In Command Prompt (CMD) or PowerShell:
```cmd
world_calculator.exe
```

To recompile at any time:
```cmd
build.bat
```
